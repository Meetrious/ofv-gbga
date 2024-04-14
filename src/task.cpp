#include <ofv_bga/task.h>
#include <common/Timer.h>
#include <tiny_instruments/easy_random.h>
#include <exceptions.h>

#include <cmath>
#include <iostream>
#include <memory>
#include <cassert>

namespace {

[[nodiscard]] inline double
getMutation(const double L, const double R,
            const double gamma, const double mu) noexcept {
  return mu * (R - L) * std::pow(2, -16 * gamma);
}

}  // namespace

#define TEMPLATE_BGA_TASK(RETURN_TYPE) \
  template <typename StraightTaskType> RETURN_TYPE \
  BGA::Task<StraightTaskType>

TEMPLATE_BGA_TASK()::Task(const StraightTaskType&                 stRef,
                          const vector<feature_t::base::CnstPtr>& featureBasesPtrs,
                          const Parameters&                       bgaParams,
                          const std::string&                      dirForOutput)
  : Parameters{bgaParams},
    default_indiv{featureBasesPtrs},
    ios{dirForOutput} {

  // инициализируем объекты прямых задач в crew
  for (auto & worker: crew) {
    worker.m_static.ptr_to_st = std::make_unique(stRef);
  }

  m_params.amount_of_attributes = featureBasesPtrs.size();

  for (size_t i = 0; i < m_params.initial_p; ++i) {
    population.emplace_back(default_indiv);
  }
  population.shrink_to_fit();

  // this object is ready for the algorithm, kind of...

  ios.ConstructCoefEvoPlotScript(default_indiv.m_features);
  ios.ConstructAberEvoPlotScript();
  ios.ConstructMultiplotScript();

  ios.RestartCollector(m_params, default_indiv.m_features);

  ios.CSout.open(ios.m_dirForOutput + "RT/current/leaders_C.txt", std::ios_base::out);
  if (!ios.CSout) {
    std::cerr << "\n !!!stream for coefficients was not allocated"
              << " for some reason.\n Throwing exception!";
    throw external_file_allocation_error();
    return;
  }

  ios.CSout << '#' 
            << "mutation_param = " << m_params.mutation_val << "\t"
            << "recomb_param = " << m_params.recombination_val << "\t"
            << "N_gen" << m_params.amount_of_iterations << "\t"
            << "population : " << m_params.initial_p << " -> " << m_params.regular_p << std::endl;

  ios.CSout << "#";
  for (size_t i = 0; i <= m_params.amount_of_attributes; ++i) 
    ios.CSout << featureBasesPtrs[i]->m_base_name << "\t";

  ios.CSout << std::endl;

  ios.CSout.close();

  ios.Fout.open(ios.m_dirForOutput + "RT/current/leaders_F.txt", std::ios_base::out);
  if (!ios.Fout) {
    std::cerr << "\n !!!stream for dfi-values were not allocated"
              << " for some reason.\n Throwing exception!";
    getchar();
    return;
  }
  ios.Fout.close();
}



TEMPLATE_BGA_TASK(void)::SolveForOutput() {
  using namespace common;

  // кол-во, до которого можно пополнять популяцию перед появлением "новичков"
  unsigned int quoted_amount_for_reproduction =
    m_params.regular_p - (m_params.survived_p + m_params.recreated_p);

  assert(m_params.regular_p < quoted_amount_for_reproduction + m_params.survived_p + m_params.recreated_p);

  m_params.display();

  Timer timer;

  // for (uint16_t j = 0; j <= m_params.amount_of_attributes; ++j)
  //   crew[0].CoefsToVariate[j]->value = default_member.coefs_values[j];

  // calculating Aberration value of the default_member
  population[0].m_dfi_value = crew[0].STM.SolveForBGA();

  timer.ClickEnd();

  auto time = timer.CountInterval();

  std::cout << "\n \t initial relative aberration = " << population[0].m_dfi_value << std::endl;

  /* std::cout << "\t time spent on evaluation ~" <<
  std::chrono::duration_cast<std::chrono::microseconds>(time).count() << " ms" << std::endl
    << "\t approximate computation time:"
    << "\t\t" << dur_cast_to_ms(time * (m_params.initial_p + m_params.regular_p * (amount_of_iterations
  - 1)) / amount_of_threads).count() << " ms"
    << "\t\t" << dur_cast_to_sec(time * (m_params.initial_p + m_params.regular_p * (amount_of_iterations -
  1)) / amount_of_threads).count() << " sec"
    << "\t\t" << dur_cast_to_min(time * (m_params.initial_p + m_params.regular_p * (amount_of_iterations -
  1)) / amount_of_threads).count() << " min"; getchar(); // */

  /* let's process the first fraction of the population;
   * by that I mean calculate F_value (cind = current_individual) */

  timer.ClickStart();

  engage(1, m_params.survived_p);

  population[1] = population[0]; /// TODO: а надо ли?? и можно ли?

  // 
  for (uint16_t cit = 1; cit <= m_params.amount_of_iterations; cit++) {
    const bool it_is_time_to_print = (cit % (m_params.amount_of_threads * 2) == 0);

    if (it_is_time_to_print)
      std::cout << "\n" << cit << '/' << m_params.amount_of_iterations << "th iteration of BGA: " << std::endl;

    // распределение решения прямой задачи по потокам
    engage(m_params.survived_p,  population.size());

    // 2. Sorting vector container in ascending order of F_values
    Sort1stFrac(it_is_time_to_print);

    // 4. "Recombining"
    for (size_t cur_indiv_idx = m_params.survived_p;
                cur_indiv_idx < quoted_amount_for_reproduction;
                ++cur_indiv_idx) {
      Recombine(population[cur_indiv_idx]);
      Mutate(population[cur_indiv_idx]);
    }

    // 5*. Accepting new members to the Population
    for (size_t cur_indiv_idx = quoted_amount_for_reproduction;
                cur_indiv_idx <= m_params.regular_p;
                ++cur_indiv_idx) {
      population[cur_indiv_idx].randomize_in_base_bounds();
    }

    ios.WriteResult(population[1]);

    if (1 == cit) {
      population.erase(population.begin() + m_params.regular_p + 1, population.end());
      population.shrink_to_fit();
    }
  }

  ios.WriteBest(population[1]);
  ios.WriteStatData(population[1], "Raw_stat_N");

  timer.ClickEnd();
  time = timer.CountInterval();

  std::cout << "\n Reversed task is solved for now;" << std::endl;
  std::cout << "Final computation time:\n "
            << "\t\t" << dur_cast_to_ms(time).count() << " ms" << std::endl
            << "\t\t" << dur_cast_to_sec(time).count() << " sec" << std::endl
            << "\t\t" << dur_cast_to_min(time).count() << " min"
            << std::endl;
  std::cin.get();
  return;

}  // Task::SolveForOutput()


TEMPLATE_BGA_TASK(void)::engage(const size_t first_indiv_idx,
                                const size_t last_indiv_idx) {

  for (size_t worker_idx = 0;
              worker_idx < m_params.amount_of_threads; 
              ++worker_idx) {
    crew[worker_idx].start_to_work(population, first_indiv_idx, last_indiv_idx);
  }
  for (size_t worker_idx = 0;
              worker_idx < m_params.amount_of_threads;
              ++worker_idx) {
    crew[worker_idx].wait_utill_ready();
  }
}

// /* // Recombine
TEMPLATE_BGA_TASK(void)::Recombine(Individ& Ind) {
  size_t parent_idx[] = {
    static_cast<size_t>(my_rand::get(1.0, m_params.survived_p)),
    static_cast<size_t>(my_rand::get(1.0, m_params.survived_p))
  };

  const Individ& parent_1 = population[parent_idx[0]],
                 parent_2 = population[parent_idx[1]];

  Ind.replace_with_combination_of(parent_1, parent_2, m_params.recombination_val);
}

// /* // Mutate
TEMPLATE_BGA_TASK(void)::Mutate(Individ& Ind) {
  for (size_t j = 0; j <= m_params.amount_of_attributes; ++j) {
    
    const bool isDeviationToTheRight = static_cast<int32_t>(my_rand::get(1.0, 100.0)) % 2;

    const double gamma = my_rand::get(0.0, 1.0),
                 l_bound = Ind.m_features[j].get_left_boundary(),
                 r_bound = Ind.m_features[j].get_right_boundary(),
                 deviation = getMutation(l_bound, r_bound, gamma, m_params.mutation_val);

    if (isDeviationToTheRight)
      Ind.m_features[j].m_value += deviation;
    else
      Ind.m_features[j].m_value -= deviation;
  } 
} // */

#undef TEMPLATE_BGA_TASK

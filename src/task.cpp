#include <ofv_bga/task.h>


#include <cmath>
#include <iostream>
#include <common/Timer.h>
#include <memory>

namespace {

[[nodiscard]] inline double
getRecombination(const double x, const double y, const double alp) noexcept {
  return x + alp * (y - x);
}

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

  amount_of_attributes = featureBasesPtrs.size();

  // initialising default member by current default state of the ODE System in ST

  for (size_t i = 0; i < p_0; ++i) {
    population.emplace_back(default_member);
  }
  population.shrink_to_fit();

  // this object is ready for the algorithm, kind of...

  ios.ConstructCoefEvoPlotScript(bounds, Workers[0].CoefsToVariate);
  ios.ConstructAberEvoPlotScript();
  ios.ConstructMultiplotScript();

  ios.RestartCollector(*this, Workers[0].CoefsToVariate);

  ios.CSout.open(output_dir + "RT/current/leaders_C.txt", std::ios_base::out);
  if (!ios.CSout) {
    std::cerr << "\n !!!stream for coefficients was not allocated"
              << " for some reason.\n Throwing exception!";
    throw external_file_allocation_error();
    // getchar();
    return;
  }

  ios.CSout << '#' 
            << "mutation_param = " << mutation_val << "\t"
            << "recomb_param = " << recombination_val << "\t"
            << "N_gen" << amount_of_iterations << "\t"
            << "population : " << p_0 << " -> " << p << std::endl;

  ios.CSout << "#";
  for (size_t i = 0; i <= amount_of_attributes; i++) 
    ios.CSout << Workers[0].CoefsToVariate[i]->name << "\t";

  ios.CSout << std::endl;

  ios.CSout.close();

  ios.Fout.open(output_dir + "RT/current/leaders_F.txt", std::ios_base::out);
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

  // кол-во особей генерируемых от исходных границ после отбора
  unsigned int amount_of_newbies = newly_recreated_fraction * m_params.regular_p;

  // кол-во особей после отбора
  unsigned int amount_of_favorites = sorted_fraction * m_params.regular_p;

  // кол-во, до которого можно пополнять популяцию перед появлением "новичков"
  unsigned int quoted_amount_for_reproduction = m_params.regular_p - amount_of_newbies;

  assert(amount_of_newbies + amount_of_favorites > 100);

  m_params.display();

  Timer timer;

  for (uint16_t j = 0; j <= amount_of_attributes; ++j)
    crew[0].CoefsToVariate[j]->value = default_member.coefs_values[j];

  // calculating Aberration value of the default_member
  Population[0].F_value = crew[0].STM.SolveForBGA(crew[0].F);

  timer.ClickEnd();

  auto time = timer.CountInterval();

  std::cout << "\n \t initial relative aberration = " << Population[0].F_value << std::endl;

  /* std::cout << "\t time spent on evaluation ~" <<
  std::chrono::duration_cast<std::chrono::microseconds>(time).count() << " ms" << std::endl
    << "\t approximate computation time:"
    << "\t\t" << dur_cast_to_ms(time * (p_0 + p * (amount_of_iterations
  - 1)) / amount_of_threads).count() << " ms"
    << "\t\t" << dur_cast_to_sec(time * (p_0 + p * (amount_of_iterations -
  1)) / amount_of_threads).count() << " sec"
    << "\t\t" << dur_cast_to_min(time * (p_0 + p * (amount_of_iterations -
  1)) / amount_of_threads).count() << " min"; getchar(); // */

  /* let's process the first fraction of the population;
   * by that I mean calculate F_value (cind = current_individual) */

  timer.ClickStart();

  engage(1, m_params.amount_of_favorites)

  Population[1] = Population[0];

  // 
  for (uint16_t cit = 1; cit <= amount_of_iterations; cit++) {
    bool it_is_time_to_print = (cit % (amount_of_threads * 2) == 0);

    if (it_is_time_to_print)
      std::cout << "\n" << cit << '/' << amount_of_iterations << "th iteration of BGA: " << std::endl;

    // распределение решения прямой задачи по потокам
    engage(m_params.amount_of_favorites,  Population.size());

    // 2. Sorting vector container in ascending order of F_values
    Sort1stFrac(it_is_time_to_print);

    // 4. "Recombining"
    for (size_t cur_indiv_idx = amount_of_favorites;
                cur_indiv_idx < quoted_amount_for_reproduction;
                ++cur_indiv_idx) {
      Recombine(Population[cur_indiv_idx]);
      Mutate(Population[cur_indiv_idx]);
    }

    // 5*. Accepting new members to the Population
    for (size_t cur_indiv_idx = quoted_amount_for_reproduction;
                cur_indiv_idx <= m_params.regular_p;
                ++cur_indiv_idx) {
      for (size_t j = 0; j <= amount_of_attributes; ++j) {
        Population[cur_indiv_idx].RandomiseCoef(j, bounds[0][j], bounds[1][j]);
      }
    }

    ios.WriteResult(Population[1]);

    if (1 == cit) {
      Population.erase(Population.begin() + p + 1, Population.end());
      Population.shrink_to_fit();
    }
  }

  ios.WriteBest(Population[1]);
  ios.WriteStatData(Population[1], "Raw_stat_N");

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
  size_t prns[] = {(size_t)(random(1.0, amount_of_favorites)), (size_t)(random(1.0, amount_of_favorites))};

  for (size_t j = 0; j <= amount_of_attributes; ++j) {
    double alp = random(-rc, 1.0 + rc);

    Ind.m_id_features[j] =
        RecombExpr(Population[prns[0]].m_id_features[j], Population[prns[1]].m_id_features[j], alp);
  }
}  // */



// /* // Mutate
TEMPLATE_BGA_TASK(void)::Mutate(Individ& Ind) {
  for (size_t j = 0; j <= amount_of_attributes; j++) {
    bool isDeviationToTheRight = static_cast<int32_t>(random(1.0, 100.0)) % 2;
    double gamma = random(0.0, 1.0);
    double deviation = MutExpr(bounds[0][j], bounds[1][j], gamma, mu);

    if (isDeviationToTheRight)
      Ind.coefs_values[j] += deviation;
    else
      Ind.coefs_values[j] -= deviation;
  } 
} // */

#undef TEMPLATE_BGA_TASK

#ifndef TASK_HPP_
#define TASK_HPP_

#include <ofv_bga/task.h>
#include <common/Timer.h>
#include <tiny_instruments/easy_random.h>
#include <exceptions.h>

#include <cmath>
#include <iostream>
#include <memory>
#include <cassert>

#define TEMPLATE_BGA_TASK(RETURN_TYPE) \
  template <typename DockedStraightTaskType> RETURN_TYPE Task<DockedStraightTaskType>

namespace BGA {

[[nodiscard]] inline double
getMutation(const double l_bound, const double r_bound,
            const double gamma, const double mu) noexcept {
  return mu * (l_bound - r_bound) * std::pow(2, -16 * gamma);
}

template<typename DockedStraightTaskType>
  template<typename STBase, class coefType, class varType>
  Task<DockedStraightTaskType>
  Task<DockedStraightTaskType>::construct(
      STBase&                                     st_base,
      const std::vector<featureBaseCPtr>&         featureBasesPtrs,
      const std::vector<controlVarBaseCPtr>&      controlVarBasesPtr,
      const BGA::Parameters&                      bgaParams,
      const std::string&                          dirForOutput) {

  auto dst = DockedStraightTaskType(st_base, featureBasesPtrs, controlVarBasesPtr);
  return Task(std::move(dst), featureBasesPtrs, bgaParams, dirForOutput);

}

TEMPLATE_BGA_TASK()::Task(DockedStraightTaskType&&          stRef,
                          const vector<featureBaseCPtr>&    featureBasesPtrs,
                          const Parameters&                 bgaParams,
                          const std::string&                dirForOutput)
  : m_params{bgaParams},
    default_indiv{featureBasesPtrs},
    ios{dirForOutput} {

  // инициализируем объекты прямых задач в crew
  for (auto & worker: crew) {
    worker.ptr_to_st = std::make_unique<DockedStraightTaskType>(stRef);
  }

  m_params.amount_of_features = featureBasesPtrs.size();

   // нулевой член популяции - текущее состояние задачи
  population.emplace_back(default_indiv);
  
  for (size_t i = 1; i < m_params.initial_p; ++i) {
    population.emplace_back(featureBasesPtrs);
  }
  population.shrink_to_fit();

  ios.construct_feat_evo_plot_script(default_indiv.m_features);
  ios.construct_dfi_evo_plot_script();
  ios.construct_multiplot_script();

  ios.restart_chronicler(m_params, default_indiv.m_features);
}

TEMPLATE_BGA_TASK(void)::SolveForOutput() {
  using namespace common;

  // кол-во, до которого можно пополнять популяцию перед появлением "новичков"
  const unsigned int quoted_amount_for_reproduction =
    m_params.regular_p - (m_params.survived_p + m_params.recreated_p);

  assert(m_params.regular_p < quoted_amount_for_reproduction 
                                + m_params.survived_p 
                                + m_params.recreated_p);

  m_params.display();

  Timer timer;

  // calculating dfi value of the default_member
  crew[0].ptr_to_st->apply_individ(population[0]);
  population[0].m_dfi_value 
    = default_indiv.m_dfi_value = crew[0].ptr_to_st->SolveForBGA();

  timer.ClickEnd();

  std::cout << "\n Initial relative deviation from given ideal = " 
            << population[0].m_dfi_value << std::endl;

  auto time = timer.CountInterval(); 
  /*
  std::cout << "\t time spent on evaluation ~" <<
  std::chrono::duration_cast<std::chrono::microseconds>(time).count() << " ms" << std::endl
    << "\t approximate computation time:"
    << "\t\t" << dur_cast_to_ms(time * (m_params.initial_p + m_params.regular_p * (amount_of_iterations
  - 1)) / amount_of_threads).count() << " ms"
    << "\t\t" << dur_cast_to_sec(time * (m_params.initial_p + m_params.regular_p * (amount_of_iterations -
  1)) / amount_of_threads).count() << " sec"
    << "\t\t" << dur_cast_to_min(time * (m_params.initial_p + m_params.regular_p * (amount_of_iterations -
  1)) / amount_of_threads).count() << " min"; getchar(); // */

  /* let's process the first fraction of the population;
   * by that I mean calculate m_dfi_value (cind = current_individual)  // */

  timer.ClickStart();

  engage(1, m_params.survived_p);

  // default_individ должен участвовать в отборе
  population[1] = population[0];

  for (size_t gen_idx = 1; gen_idx <= m_params.amount_of_iterations; ++gen_idx) {
    const bool it_is_time_to_print = (gen_idx % (m_params.amount_of_threads * 2) == 0);

    if (it_is_time_to_print)
      std::cout << "\n" << gen_idx << '/' << m_params.amount_of_iterations 
                        << "th iteration of BGA: " << std::endl;

    // распределение решения прямой задачи по потокам
    engage(m_params.survived_p,  population.size());

    // 2. Сортируем
    sort_adapted_fraction(it_is_time_to_print);

    /* 4,5 Рекомбинация индивидуумов, пополнение популяции до приемлемого уровня
      * и их мгновенная мутация на удачу. */
    for (size_t cur_indiv_idx = m_params.survived_p,
                amount_level_of_locals =
                  m_params.survived_p + quoted_amount_for_reproduction;
                cur_indiv_idx < amount_level_of_locals;
                ++cur_indiv_idx) {
      Recombine(population[cur_indiv_idx]);
      Mutate(population[cur_indiv_idx]);
    }

    // 5*. Бонусом добавляем новых индивидуумов в популяцию, генерируя их из базовых границ
    for (size_t cur_indiv_idx = m_params.survived_p + quoted_amount_for_reproduction;
                cur_indiv_idx <= m_params.regular_p;
                ++cur_indiv_idx) {
      population[cur_indiv_idx].randomize_in_base_bounds();
    }

    ios.write_features_for_current_generation_out(population[1]);

    /* если это первая итерация, то текущий уровень популяции = исходной численности (initial_p),
       которая, возможно, была выбрана заведомо больше, чем регулярная численность (regular_p) */
    if (1u == gen_idx && !m_params.is_initial_size_regular()) {
      population.erase(population.begin() + m_params.regular_p + 1, population.end());
      population.shrink_to_fit();
    }
  }

  ios.write_features_out(population[1]);
  ios.write_full_result_for_current_generation_out(population[1], "Raw_stat_N");

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

TEMPLATE_BGA_TASK(void)::Recombine(Individ& indiv) {
  const size_t parent_idx[] = {
    static_cast<size_t>(my_rand::get(1.0, m_params.survived_p)),
    static_cast<size_t>(my_rand::get(1.0, m_params.survived_p))
  };

  const Individ& parent_1 = population[parent_idx[0]],
                 parent_2 = population[parent_idx[1]];

  indiv.replace_with_combination_of(parent_1, parent_2, m_params.recombination_val);
}

TEMPLATE_BGA_TASK(void)::Mutate(Individ& indiv) {
  for (size_t j = 0; j <= m_params.amount_of_features; ++j) {
    
    const bool isDeviationToTheRight = static_cast<int32_t>(my_rand::get(1.0, 100.0)) % 2;

    const double gamma = my_rand::get(0.0, 1.0),
                 l_bound = indiv.m_features[j].get_left_boundary(),
                 r_bound = indiv.m_features[j].get_right_boundary(),
                 deviation = getMutation(l_bound, r_bound, gamma, m_params.mutation_val);

    if (isDeviationToTheRight)
      indiv.m_features[j].m_value += deviation;
    else
      indiv.m_features[j].m_value -= deviation;
  } 
} // */


TEMPLATE_BGA_TASK(bool)::sort_adapted_fraction
(const bool it_is_time_to_print) {
  size_t min_ind;
  double tmp = 1000.0;

  bool isBestfound = true;

  // cycle to sort in ascending order best individials
  for (size_t cur_best_idx = 1; cur_best_idx <= m_params.survived_p; ++cur_best_idx) {
    // cycle to find best individual in the population beginning with (i)-th
    // individual
    for (size_t indiv_idx = cur_best_idx; indiv_idx < population.size(); ++indiv_idx) {
      if (population[indiv_idx] < tmp) {
        tmp = population[indiv_idx].m_dfi_value;
        min_ind = indiv_idx;
      }

    }  // min_ind is now an index of the individual whose m_dfi_value is to be next
       // in the accending order after (i-1)-th individual

    tmp = 1000;  // reseting buffer-variable

    // // if min_ind is in the right place...
    // if (min_ind == i) {

    //   // and if population[min_ind] is the leader of the previous iteration,
    //   // then new best solution was not found
    //   if (i == 1) isBestfound = false;

    //   // ...then it stays right where it is. And we go on searching next best
    // individual continue;
    // }

    // // otherwise we change its position in the current "leaderboard"
    // else SwapInVect(i, min_ind); // 

    if (min_ind != cur_best_idx) {
      std::swap(population[cur_best_idx], population[min_ind]);
    }
      
    // SwapInVect(i, min_ind);

    // outputting current state of the leaderboard with regards to the
    // first-third and amount_of_favorites'th leaders within current population // 
    if (it_is_time_to_print) {
      if (cur_best_idx <= 3) {
        std::cout << "\t" << min_ind << " ===> " << cur_best_idx
                  << " \t\t F = " << population[cur_best_idx].m_dfi_value << std::endl;
        continue;
      }

      if (cur_best_idx == m_params.survived_p) {
        std::cout << "\t ... \t ... \n"
                  << "\t" << min_ind << " ===> " << cur_best_idx
                  << " \t\t F = " << population[cur_best_idx].m_dfi_value << std::endl;
      } 
    }
  }
  return isBestfound;
}

}

#undef TEMPLATE_BGA_TASK

#endif  // TASK_HPP_

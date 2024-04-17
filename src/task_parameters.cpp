#include <ofv_bga/task_parameters.h>
#include <iostream>
#include <exception>
#include <cassert>

#define assertm(exp, msg) assert(((void)msg, exp))

namespace {
  bool shout_and_toggle_if_true(const bool toggle_expression, const char* shout) {
    if (toggle_expression == true) {
      std::cerr << shout << std::endl;
    }
    return toggle_expression;
  }
}


BGA::Parameters::Parameters(const unsigned iter_amount,
                            const unsigned thread_amount,
                            const unsigned init_pop_size,
                            const unsigned reg_pop_size,
                            const unsigned Sort_Fract,
                            const unsigned Recr_Fract,
                            const double recombination_parameter,
                            const double mutation_parameter
) :
    amount_of_iterations{iter_amount},
    amount_of_threads{thread_amount},
    initial_p{init_pop_size},
    regular_p{reg_pop_size},
    sorted_fraction{Sort_Fract},
    newly_recreated_fraction{Recr_Fract},
    recombination_val{recombination_parameter},
    mutation_val{mutation_parameter} {

  survived_p = sorted_fraction * (regular_p / 100);
  recreated_p = newly_recreated_fraction * (regular_p / 100);
  assert_incorrect_initialization();
}

void BGA::Parameters::display() noexcept {
  std::cout 
    << "\n BGA parameters: \n"
    << "\n\t amount of iterations = "           << amount_of_iterations
    << "\n\t amount of threads = "              << amount_of_threads 
    << "\n\t initial population size = "        << initial_p
    << "\n\t regular population size = "        << regular_p
    << "\n\t percent of selected population = " << sorted_fraction
    << "\n\t percent of newly recreated = "     << newly_recreated_fraction
    << "\n\t recombination parameter = "        << recombination_val
    << "\n\t mutation parameter = "             << mutation_val
    << std::endl;
}

void BGA::Parameters::assert_incorrect_initialization() {
  const bool initialization_is_bad = 
    shout_and_toggle_if_true(initial_p < 3u, "initial population size must be more, then 2")

    | shout_and_toggle_if_true(regular_p < 2u || initial_p < regular_p,
                               "regular population size must be more, then 0"
                               " and no more, then initial")

    | shout_and_toggle_if_true(regular_p <= survived_p, "")

    | shout_and_toggle_if_true(recreated_p > (regular_p - survived_p), "")

    | shout_and_toggle_if_true(sorted_fraction >= 100u || newly_recreated_fraction >= 100u,
                               "fractions are percentages; they can't be more then 100");

  if (initialization_is_bad) throw std::runtime_error("Bad BGA parameters");
}

#include <ofv_bga/task_parameters.h>
#include <iostream>

BGA::Parameters::Parameters(unsigned iter_amount,
                            unsigned thread_amount,
                            unsigned init_pop_size,
                            unsigned reg_pop_size,
                            unsigned Sort_Fract,
                            unsigned Recr_Fract,
                            double recombination_parameter,
                            double mutation_parameter
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

}  // BGA::Task::Parameters-constructor


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

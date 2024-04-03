#include <ofv_bga/task.h>
#include <iostream>

using namespace BGA;

template<typename ST_Method>
BGA::Task<ST_Method>::Parameters::Parameters(
    // parameters for ReverseTask
    unsigned iter_amount, unsigned thread_amount, unsigned init_pop_size,
    unsigned reg_pop_size, unsigned Sort_Fract, unsigned Recr_Fract,
    double recombination_parameter, double mutation_parameter
) : 
    amount_of_iterations{iter_amount},
    amount_of_threads{thread_amount},
    p_0{init_pop_size},
    p{reg_pop_size},
    SortedFraction{Sort_Fract},
    RecruitedFraction{Recr_Fract},
    rc{recombination_parameter},
    mu{mutation_parameter} {
      
  bounds.clear();
  bounds.emplace_back();
  bounds.emplace_back();
  bounds.shrink_to_fit();  

  // amount_of_attributes = SetCTVlist();

  Sp = SortedFraction * (p / 100);
  p_rec = RecruitedFraction * (p / 100);

}  // BGA::Task::Parameters-constructor


template <typename ST_Method> void
BGA::Task<ST_Method>::Parameters::display() noexcept {
  std::cout << "\n ReverseTask parameters: \n"
            << "\n\t amount of iterations = " << amount_of_iterations
            << "\n\t amount of threads = " << amount_of_threads 
            << "\n\t initial population size = " << p_0
            << "\n\t regular population size = " << p
            << "\n\t percent of selected population = " << SortedFraction
            << "\n\t percent of recruited population = " << RecruitedFraction
            << "\n\t recombination parameter = " << rc
            << "\n\t mutation parameter = " << mu
            << std::endl;

  std::cout << "\n\n StraightTask parameters: \n"
            << "\n\t grid-step size = " << N 
            << "\n\t width of a gap in the step method = " << gap_width
            << "\n\t full amount of gaps in the step method = " << full_amount_of_gaps 
            << std::endl;
}

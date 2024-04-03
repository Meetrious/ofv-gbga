#ifndef BGA_PARAMETERS_H_
#define BGA_PARAMETERS_H_

#include <cstdint>
#include <vector>

namespace ReverseTask::BGA {

class Parameters {
  friend class IOs;
  /* all fields are independent except SetCVlist member-function.
   * SetCTVlist is to be defined somewhere further after equation.h inclusion. */
  
  template<typename T>
  using matrix = std::vector<std::vector<double>>;
 
 public:
  Parameters
  ( // parameters for ReverseTask
    unsigned iter_amount = 200, unsigned thread_amount = 1, unsigned init_pop_size = 200,
    unsigned reg_pop_size = 100, unsigned Sort_Fract = 30, unsigned Recr_Fract = 10,
    double recombination_parameter = 0.05, double mutation_parameter = 0.01,
	  // parameters for StraightTask
    unsigned ST_gridPow = 1500, double ST_gapWidth = 24.0, unsigned ST_amountOfGaps = 1);

  // parameters for StraightTask
  	uint32_t N;  // amount of nods in a grid
  	double gap_width;
  	unsigned full_amount_of_gaps;  // crucial parameter in terms of dealing with delayed arguments in equations

  ~Parameters() = default;

 protected:
  size_t amount_of_attributes;

  // list of coefficients varying in the BGA
  // std::array<StraightTask::IConstant*, CTV_SIZE> CoefsToVariate;

  /* list of boundaries within which coefficient values are initially set by RNG :
   * dim = (2 x amount_of_attributes) */
  matrix<double> bounds;

  /** \brief первичная численность популяции 0 < p_0 */
  unsigned int p_0;

  /** \brief регулярная численность популяция 0 < p <= p_0 */
  unsigned int p;

  /** \brief кол-во особей после отбора 1 < Sp */
  unsigned int Sp;

  /** \brief процент выживания от регулярной численности
   *  после отбора 0 < SortFraction < 100 */
  unsigned int SortedFraction;

  /** \brief кол-во особей генерируемых от исходных границ после отбора */
  unsigned int p_rec;

  /** \brief процент популяции, порождаемой случайным образом
   *  после отбора от исходных границ */
  unsigned int RecruitedFraction;

  /** \brief кол-во генераций поколений в одном запуске BGA */
  size_t amount_of_iterations;

  /** \brief параметр рекомбинации */
  double rc;

  /** \brief мутационный параметр */
  double mu;

  unsigned int amount_of_threads;

  void display() noexcept;

 private:

  unsigned int GetFraction(unsigned int general) noexcept {
    return SortedFraction * (general / 100);
  }
  
};

}  // namespace ReverseTask::BGA

#endif  // BGA_PARAMETERS_H_

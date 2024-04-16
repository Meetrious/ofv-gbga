#ifndef BGA_PARAMETERS_H_
#define BGA_PARAMETERS_H_

#include <cstddef>

namespace BGA{
  struct Parameters {
  
 public:
  Parameters(const unsigned iter_amount = 200,
             const unsigned thread_amount = 1,
             const unsigned init_pop_size = 200,
             const unsigned reg_pop_size = 100,
             const unsigned Sort_Fract = 30,
             const unsigned Recr_Fract = 10,
             const double recombination_parameter = 0.05,
             const double mutation_parameter = 0.01);

  ~Parameters() = default;

  size_t amount_of_features;

  /** \brief первичная численность популяции 
   * \note 0 < p_0 */
  unsigned int initial_p;

  /** \brief регулярная численность популяции 
   * \note 0 < regular_p <= p_0 */
  unsigned int regular_p;

  /** \brief численность индивидуумов воссозданных
   *  от исходных границ после очередного отбора
   * \note 0 <= recreated_p < regular_p */
  unsigned int recreated_p;

  /** \brief численность индивидуумов, переживших отбор 
   * \note 0 < survived_p < regular_p */
  unsigned int survived_p;

  /** \brief процент выживания от регулярной численности
   *  после отбора \in (0, 100) */
  unsigned int sorted_fraction;

  /** \brief процент популяции, порождаемой случайным образом
   *  после отбора от исходных границ \in (0, 100) */
  unsigned int newly_recreated_fraction;

  /** \brief кол-во генераций поколений в одном запуске BGA */
  size_t amount_of_iterations;

  /** \brief параметр рекомбинации */
  double recombination_val;

  /** \brief мутационный параметр */
  double mutation_val;

  unsigned int amount_of_threads;

  void display() noexcept;
  
};  // class Task::Parameters

}  // namespace BGA

#endif  // BGA_PARAMETERS_H_

#ifndef BGA_PARAMETERS_H_
#define BGA_PARAMETERS_H_

#include <cstddef>

namespace BGA{
struct Parameters {
  explicit Parameters(unsigned iter_amount = 200,
                      unsigned thread_amount = 1,
                      unsigned init_pop_size = 200,
                      unsigned reg_pop_size = 100,
                      unsigned Sort_Fract = 30,
                      unsigned Recr_Fract = 10,
                      double recombination_parameter = 0.05,
                      double mutation_parameter = 0.01);

  ~Parameters() = default;

  size_t amount_of_features;

  /** \brief первичная численность популяции 
   * \note 2 < p_0 */
  unsigned int initial_p;

  /** \brief регулярная численность популяции 
   * \note 0 < regular_p <= p_0 */
  unsigned int regular_p;

  /** \brief численность индивидуумов воссозданных
   *  от исходных границ после очередного отбора
   * \note 0 <= recreated_p <= regular_p - survived_p;
   * Правое равенство удовлетворяется -> рекомбинации не происходит. */
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

  void display() const noexcept;

  inline bool is_initial_size_regular() const noexcept {
    return initial_p == regular_p;
  }
  
 private:

  void assert_incorrect_initialization();

};  // class Task::Parameters

}  // namespace BGA

#endif  // BGA_PARAMETERS_H_

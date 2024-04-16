#ifndef BGA_THREAD_H_
#define BGA_THREAD_H_

#include <ofv_bga/individ.h>

#include <cstdint>
#include <thread>
#include <vector>
#include <memory>
#include <sstream>

namespace BGA {

using std::vector;
using std::thread;
using std::array;

class IAggregateControls;


/** \brief тип-интерфейс связи прямой задачи с обратной.
 * \note DockedStraightTaskType-тип должен иметь move-конструктор,
 *  double SolveForBGA(...) метод, который возвращает значение функционала невязки */
template <typename DockedStraightTaskType>
class StraightTaskSolverThread {
  static size_t current_amount_of_workers;
  const size_t m_worker_index;
  std::unique_ptr<DockedStraightTaskType> ptr_to_st = nullptr;

  /** \brief подставляет индивид в объект задачи под указателем ptr_to_st,
   * и вызывает решатель прямой задачи, в следствие которой будет получено невязка = dif_value
   * Это происходит для текущего StraightTaskSolverThread-объекта-работника
   * вдоль всей популяции индивидуумов, начиная с first_indiv_idx */
  void operator()(vector<Individ> & population,
                  const size_t first_indiv_idx,
                  const size_t last_indiv_idx);

  thread m_thr;

 public:

  StraightTaskSolverThread(): m_worker_index(++current_amount_of_workers) {}

  /** \brief настраивается прямая задача, которую он будет решать. */
  // void PrepairToWork(const std::shared_ptr<vector<Individ>> & ptr_to_population,
  //                    const uint16_t amount_of_threads);

  void retrieve_straight_task(DockedStraightTaskType && fully_prepaired_straight_task);

  /** * \brief здесь выделяется поток. В нём из population-массива выбираются члены,
   * начиная с first_indiv_idx, и заканчивая last_indiv_idx, и через значение m_worker_index 
   * назначаются индивиды, для которых данный поток будет решать прямую задачу. 
   * m_worker_index уникален для каждого объекта этого класса;
   * каждый работник выбирает своих индивидуумов.
   * \param[in] first - индекс индивидуума популяции, 
   *  с которого начнётся расчёт прямой задачи
   * \param[in] last - индекс, ограничивающий численность популяции, 
   *  для которой надо провести расчёт */
  void start_to_work(vector<Individ> & population,
                     const size_t first_indiv_idx,
                     const size_t last_indiv_idx);

  inline void wait_utill_ready() { if (m_thr.joinable()) m_thr.join(); }

  ~StraightTaskSolverThread();

};

}  // namespace BGA

#endif  // BGA_THREAD_H_

#ifndef BGA_THREAD_H_
#define BGA_THREAD_H_

#include <ofv_bga/individ.h>

#include <cstdint>
#include <thread>
#include <vector>
#include <memory>

namespace ReverseTask::BGA {

using std::vector, std::thread, std::array;

class IAggregateControls;


/** \brief тип-интерфейс связи прямой задачи с обратной.
 * \note StraightTask_t-тип должен иметь move-конструктор,
 *  double SolveForBGA(...) метод, который возвращает значение функционала невязки */
template <typename StraightTask_t>
class StraightTaskSolverThread {
  static size_t current_amount_of_workers = 0;
  const size_t m_worker_index;
  std::unique_ptr<StraightTask_t> ptr_to_st = nullptr;

  void operator()(vector<Individ> & population,
                  const size_t first_indiv_idx,
                  const size_t last_indiv_idx);

  thread m_thr;

 public:

  // IAggregateControls F;

  //  // list of coefficients varying in the BGA
  // array<StraightTask::IConstant*, CTV_SIZE> CoefsToVariate;

  StraightTaskSolverThread(): m_worker_index(global_amount_of_workers++) {}

  /** \brief настраивается прямая задача, которую он будет решать. */
  void PrepairToWork(const std::shared_ptr<vector<Individ>> & ptr_to_population,
                     const uint16_t amount_of_threads);

  void retrieve_straight_task(StraightTask_t && fully_prepaired_straight_task);

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

  inline void wait_utill_ready() { if (thr.joinable()) thr.join(); }

  ~StraightTaskSolverThread();

};

}  // namespace ReverseTask::BGA

#endif  // BGA_THREAD_H_

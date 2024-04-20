#include <ofv_bga/evo_pipe.h>
#include <exceptions.h>
#include <iostream>
#include <string>

namespace BGA {
  using std::shared_ptr;
  using std::weak_ptr;
  using std::vector;

#define ST_THR_TEMPLATE(RETURN_TYPE) \
  template <typename StraightTask_t> RETURN_TYPE\
  StraightTaskSolverThread<StraightTask_t>

// ST_THR_TEMPLATE(void)::PrepairToWork(
//     const shared_ptr<vector<Individ>> & ptr_to_population,
//     const uint16_t amount_of_threads) {
//   // m_static.ptr_to_population = ptr_to_population;
//   current_amount_of_workers = amount_of_threads;
//   // STM.Mthd.Set(N, gap_width, full_amount_of_gaps); 
//   // STM.PrepairTheTask();
//   // F.GatherData();
// }

// ST_THR_TEMPLATE(void)::retrieve_straight_task(
//     StraightTask_t && fully_prepaired_straight_task) {
//   ptr_to_st = // передаём владение готовым straight-task-объектом сюда
//     std::make_unique<StraightTask_t>(std::move(fully_prepaired_straight_task));
// }

ST_THR_TEMPLATE(void)::operator() (vector<Individ> & population,
                                   const size_t first_indiv_idx,
                                   const size_t last_indiv_idx) {
  // индекс индивидуума (набора аттрибутов) в выделенной популяции.
  size_t cur_indiv_idx = first_indiv_idx + m_worker_index;

  while (cur_indiv_idx < last_indiv_idx) {

    ptr_to_st->apply_individ(population[cur_indiv_idx]);
    population[cur_indiv_idx].m_dfi_value = ptr_to_st->SolveForBGA();

    #ifdef _DEBUG
      std::cout << std::this_thread::get_id() 
                << " -- thread finished with " << m_worker_index 
                << " - " << cur_indiv_idx << " indiv\n"
                << std::endl;
    #endif
    cur_indiv_idx += current_amount_of_workers;
  }
}  // operator()(vector<Individ>, size_t, size_t)


ST_THR_TEMPLATE(void)::start_to_work(vector<Individ> & population,
                                     const size_t first_indiv_idx,
                                     const size_t last_indiv_idx) try {
  
  if (nullptr == ptr_to_st) {
    std::stringstream error_msg;
    error_msg << "Pointer to the Straight-Task-object is empty"
              << " at the moment of population-testing. "
              << "You should bound st_object with a real object "
              << " before BGA-execution via retrieve_straight_task method!";
    throw std::runtime_error(error_msg.str());
  }
  
  // отщепление в отдельный поток
  m_thr = std::thread(*this, population, first_indiv_idx, last_indiv_idx);
}
catch(const std::exception& e) {
  // TODO: throw and scream
}

ST_THR_TEMPLATE()::~StraightTaskSolverThread() try {
  wait_utill_ready(); 
}
catch (const std::exception& e) {
  std::cerr << e.what();
}

#undef ST_THR_TEMPLATE

}  // namespace BGA

#include <ofv_bga/st_solver_for_BGA.h>

#include <string>
#include <sstream>
#include <exception>
#include <cstring>
#include <cassert>
#include <fstream>
#include <iostream>

#define ST_FOR_BGA(RETURN_TYPE) \
  template<typename STBase, typename coefType, class varType> RETURN_TYPE\
  StraightTaskForBGA<STBase, coefType, varType>

namespace StraightTask {

ST_FOR_BGA()::StraightTaskForBGA(const STBase & base_st,
                                 const vector<featureBaseCPtr>& control_constants_base_ptrs,
                                 const vector<controlVarBaseCPtr>& control_variables_base_ptrs)
  : STBase(base_st) {

  if (control_constants_base_ptrs.empty() or
      control_variables_base_ptrs.empty()) {
    // кина не будет
  }
  
  /** здесь происходит
   * 1.1 выбор коэффициентов, по которым будет происходить варьирование
   *  т.е. установка связи ptrs_to_constants[i]-указателей с объектами констант в уравнениях */
  map_coefficients();
  if (coef_map.empty()) {
    // варьировать нечего, решать нельзя, бросаем исключение
  }

  for (const auto& feat_base_ptr: control_constants_base_ptrs) {
    if (coef_map.end() == coef_map.find(feat_base_ptr->m_base_name.c_str())) {
      // в списке размапленных коэффициентов не нашлось текущего m_base_name.c_str()

      continue;
    }
    // feat_base_ptr->m_base_name.c_str() найден. Надо добавить указатель в ptrs_to_constants
    const char* current_coef_name = feat_base_ptr->m_base_name.c_str();
    ptrs_to_constants.emplace_back(const_cast<coefType*>(coef_map[current_coef_name]));
  }

  /* 1.2 считается hash-сумма по собранным именам констант */

  /* 2. выбор зависимых переменных, вдоль которых считается невязка
   *  т.е. установка связи указателей data_rows[i].ptr_to_sol-указатели с
   *  членами this->STBase::Mthd.X_sol */
  map_variables();
  if (var_map.empty()) {
    // не по чему считать невязку, бросаем исключение
  }
  for(const auto& sds_base_ptr: control_variables_base_ptrs) {
    if (var_map.end() == var_map.find(sds_base_ptr->m_name.c_str())) {
      // в размапленном списке переменных не нашлось m_name.c_str() имени
      continue;
    }

    // sds_base_ptr->m_name.c_str() найден. Надо добавить указатель 
    const char* cur_variable_name = sds_base_ptr->m_name.c_str();
    data_rows.emplace_back(sds_base_ptr, var_map[cur_variable_name]);
  } 

}

ST_FOR_BGA(void)::recognize_collectable_iterations() {

  const double& grid_step = STBase::Mthd;
  const double& t_0 = STBase::Mthd;

  for (auto & row: data_rows) {

    vector<size_t>& iteration_idxs =
      row.m_ptr_to_base->iterations_when_to_collect;

    iteration_idxs.clear();
    
    assert(true == row.m_data.empty());

    for (auto & dat: row.m_data) {
      const double dist = dat.arg - t_0;
      assert(dist < 0.0);
      
      const int steps_to_cur_arg = dist/grid_step;
      iteration_idxs.emplace_back(steps_to_cur_arg - 1);
      iteration_idxs.emplace_back(steps_to_cur_arg);
      iteration_idxs.emplace_back(steps_to_cur_arg + 1);
    }

    if (iteration_idxs[0] > INT32_MAX) {
      // предохранение от индекса около t_0 = [0]
      iteration_idxs.erase(iteration_idxs.begin());
    }
  }
}

ST_FOR_BGA(double)::calculate_dfi() {
  double res = 0.0;
  for (const auto & row: data_rows) {
    res += DiscreteNorm::get_l2sqr(row.m_data);
  }
  res /= data_rows.size();
  return res;
}

ST_FOR_BGA(void)::apply_individ(const Individ & indiv) {
  if (/*std::memcmp() != 0*/ false) {
    std::stringstream error_msg;
    error_msg << "hashes did not match";
    throw std::runtime_error(error_msg.str());
  }

  for (size_t i = 0; i < ptrs_to_constants.size(); ++i) {
    ptrs_to_constants[i]->value = indiv.m_features[i].m_value;
  }

}

ST_FOR_BGA(void)::collect_calculation(const size_t grid_knot) {
  for (auto & row: data_rows) {
    
    const bool target_iteration_reached_the end =
      target_iteration == row.m_ptr_to_base->iteration_when_to_collect.end();

    if (target_iteration_reached_the) continue;
    if (grid_knot == (*row.target_iteration)) {
      row.cur_dat_to_fill->arg = STBase::Mthd.X_sol.tj;
      row.cur_dat_to_fill->calculation = (*row.m_ptr_to_sol);

      ++row.cur_dat_to_fill;
      ++row.target_iteration;
    }
  }
}

/* --------------------------- custom-definitions ---------------------------*/

ST_FOR_BGA(double)::SolveForBGA() {
  
  STBase::PrepairTheTask();

  uint16_t current_gap = 0u;
  double Tj;

  if (data_rows[0].m_ptr_to_base->iterations_when_to_collect.empty()){
    recognize_collectable_iterations();
  }

  for (auto & row: data_rows) {
    row.reset_iterators();
  }

  collect_calculation(0u);

  while (current_gap < STBase::Mthd.full_amount_of_gaps) {
    
    STBase::Mthd.X_prev = STBase::Mthd.X_init;  // previous step is defined by initial one at every
                                // begining of the gap

    Tj = STBase::Mthd.X_pred.tj = STBase::Mthd.X_prev.tj;  // synchronising the independent variable
    uint32_t Nj = 1;  // restating the number of the current step in num-method

    // 1st approximations for multistep-methods
    if (current_gap == 0)
      STBase::ApplyPrepStep(Nj, Tj);  // in one-step-methods it is void{return;}

    // cycle for processing current gap in <step_method>
    for (; Nj <= STBase::Mthd.N; Nj++) {
      // shifting independent variable on one step further for predicted solution
      Tj = STBase::Mthd.X_pred.tj += STBase::Mthd.H;

      // setting ret-values for Tj-time-moment in X_pred
      STBase::RetUpload(Nj);

      STBase::AssignSolData(current_gap, Nj, STBase::Mthd.X_pred);

      STBase::ApplyMethod();

      STBase::collect_calculation(Nj + current_gap);

      // updating RetArray(s) pushing X_prev.value of solution
      STBase::RetDataUpdate(Nj);

      // shifting X_prev next step further in one-step-methods and
      // X[1],X[2]... next step further in multistep-methods
      STBase::NodeShift();

    }  // for in the gap

    ++current_gap;

    // Checking if the any gap remained processed
    if (current_gap == STBase::Mthd.full_amount_of_gaps) {
    } else {
      STBase::Mthd.X_init = (*STBase::Mthd.X_sol);
      Nj = 1;
    }
  }  // while among gaps 
  
  return calculate_dfi();

}  // StraightTaskForBGA::SolveForBGA()

ST_FOR_BGA(void)::map_variables(){/*...*/}

ST_FOR_BGA(void)::map_coefficients(){/*...*/}

/* ========================== synched-data-section ========================== */

void
synched_data_storage::base::recognize_data_from_source() {
  std::ifstream in(m_path_to_source_with_data);
	if (!in) {
    std::stringstream error_msg;
		error_msg << "Couldn't open ifstream for the file in 'm_path_to_source_with_data'-field:"
              << "\t " << m_path_to_source_with_data << "\n"
              << "for initialising standart-values for '" << m_name <<"'-element;";
    throw std::runtime_error(error_msg.str());
	}

	for (size_t i = 0; !in.eof(); ++i) {
    m_src_data.emplace_back();
		in >> m_src_data.back().arg;
		in >> m_src_data.back().standart;
	}
  m_src_data.shrink_to_fit();
}

void
synched_data_storage::base::print_info() {
  std::cout << "dependent variable name: " << m_name;
  std::cout << "path to source data: " << m_path_to_source_with_data;
  std::cout << "collected_data: \n  arg  |  calculation  |  standart  \n";
  for (const auto & dat: m_src_data) {
    std::printf("%.5f | %.5f | %.5f\n", dat.arg, dat.calculation, dat.standart);
  }
}

synched_data_storage::base::base(const char* name,
                                 const char* src_path,
                                 const double st_grid_step,
                                 const double arg_0)
 : m_name(name), m_path_to_source_with_data(src_path) {
  recognize_data_from_source();
  
  #ifdef DEBUG_OUTPUT
    print_info();
  #endif

  if (!m_src_data.empty()) {
    for (const auto& dat: m_src_data) {
      iterations_when_to_collect.emplace_back(std::abs(dat.arg - arg_0)/st_grid_step);
    }
    iterations_when_to_collect.shrink_to_fit();
  }
}

synched_data_storage::synched_data_storage
                  (const synched_data_storage::base::CnstPtr & ptr_to_base,
                   const double* ptr_to_sol)
  : m_ptr_to_base(ptr_to_base),
    m_data(ptr_to_base->m_src_data),
    cur_dat_to_fill(m_data.begin()),
    target_iteration(ptr_to_base->iterations_when_to_collect.begin()),
    m_ptr_to_sol(ptr_to_sol) {}

void
synched_data_storage::reset_iterators() {
  cur_dat_to_fill = m_data.begin();
  target_iteration = m_ptr_to_base->iterations_when_to_collect.begin();
}

}  // namespace StraightTask

#undef ST_FOR_BGA

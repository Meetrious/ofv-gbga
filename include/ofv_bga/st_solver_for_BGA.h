#ifndef ST_SOLVER_FOR_BGA_H_
#define ST_SOLVER_FOR_BGA_H_

#include <ofv_bga/individ.h>
#include <norm_interface.h>

#include <vector>
#include <string>
#include <map>

// #include <openssl/sha.h>

namespace StraightTask {

using std::vector;

using BGA::Individ;
using common::synced_datum;

struct synched_data_storage {

  struct base: Tools::SharedPointerMod<base> {
    const std::string m_name;
    const std::string m_path_to_source_with_data;
    vector<synced_datum> m_src_data;
    vector<size_t> iterations_when_to_collect;

    base(const char* name,
         const char* src_path,
         const double st_grid_step,
         const double arg_0 = 0.0);

   private:  // процедуры конструктора
    void recognize_data_from_source();
    void print_info();
  };

  synched_data_storage(const base::CnstPtr & ptr_to_base,
                       const double* ptr_to_sol);

  const double* m_ptr_to_sol = nullptr;
  const base::CnstPtr m_ptr_to_base;

  vector<synced_datum> m_data;
  vector<synced_datum>::iterator cur_dat_to_fill;
  vector<size_t>::const_iterator target_iteration;

  void reset_iterators();

};

using featureBaseCPtr = BGA::feature_t::base::CnstPtr;
using controlVarBaseCPtr = synched_data_storage::base::CnstPtr;

template<typename STBase, typename coefType, class varType = double>
class StraightTaskForBGA: public STBase {

 public:
  StraightTaskForBGA(const STBase & base_st,
                     const vector<featureBaseCPtr>& control_constants,
                     const vector<controlVarBaseCPtr>& control_variables);

  /** \note метод заполняется для каждой задачи по своему. */
  double SolveForBGA();

  /** \brief приминяет индивида. Вызывается перед SolveForBGA. 
   * \note метод заполнен и не требуется в коррекции от задачи к задаче */
  void apply_individ(const Individ &);
  
private:
  
  /** \brief заполнение var_map поля указателями на места в памяти, 
   *  где будут храниться решения, привязанные, каждые к своей зависимой переменной,
   *  чтобы потом вдоль их значений можно было считать невязку. 
   * \note метод заполняется для каждой задачи по своему. */
  void map_variables();
  std::map<const char*, const varType*> var_map;
  vector<synched_data_storage> data_rows;

  /** \brief заполнение coef_map поля указателями на места в памяти,
   * где хранятся константы, варьированием которых BGA сможет решать обратную задачу. 
   * \note метод заполняется для каждой задачи по своему. */
  void map_coefficients();
  std::map<const char*, const coefType*> coef_map;
  vector<coefType*> ptrs_to_constants;
  
  /** \brief метод для заполнения iterations_when_to_collect 
   * не только итерациями решения прямой задачи,
   * на которые выпадает нужный момент времени, но и соседними с ними */
  [[deprecated]]void recognize_collectable_iterations();

  /** \brief проход по всем ячейкам, в которых нужно собрать данные для расчёта невязки,
   * в момент итерации Nj прямой задачи решения диф-уравнения */
  void collect_calculation(const size_t Nj);

  /** \brief по расчитанным решениям прямой задачи внутри data_rows,
   * вычисляет усреднённое относительное уклонение от установленного стандарта
   * Вызывается в конце метода SolveForBGA. */
  double calculate_dfi();

};

}

#endif  // ST_SOLVER_FOR_BGA_H_

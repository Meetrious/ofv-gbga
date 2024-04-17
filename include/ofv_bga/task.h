#ifndef BGA_TASK_H_
#define BGA_TASK_H_

#include <ofv_bga/task_parameters.h>
#include <ofv_bga/task_ios.h>

#include <ofv_bga/evo_pipe.h>
#include <ofv_bga/individ.h>
#include <ofv_bga/st_solver_for_BGA.h>

#define MAX_AMOUNT_OF_THREADS 8

namespace BGA {

using std::array;
using std::vector;
using StraightTask::controlVarBaseCPtr;

template <typename T>
using matrix = vector<vector<T>>;

template <typename DockedStraightTaskType>
class Task final {
 private: // составляющие члены

  /** \brief параметры BGA */
  Parameters m_params;

  /** \brief массив из решателей, которых ровно MAX_AMOUNT_OF_THREADS-штук */
  array<StraightTaskSolverThread<DockedStraightTaskType>, MAX_AMOUNT_OF_THREADS> crew;

  /** \brief индивид, инициализарованный коэф-тами, прописанными по умолчанию */
  Individ default_indiv;

  /** \brief массив из членов популяции в BGA */
  vector<Individ> population;

  /** \brief интерфейс ввода/вывода данных */
  IOs ios;

  // единственный конструктор
  Task(DockedStraightTaskType&&          stRef,
       const vector<featureBaseCPtr>&    featureBasesPtrs,
       const Parameters&                 bgaParams,
       const std::string&                dirForOutput);

 public: // пятёрка

  Task(const Task&) = default;
  Task(Task&&) = delete;
  Task& operator=(Task&&) = default;
  Task& operator=(const Task&) = default;
  ~Task() = default;

  template<typename STBase, class coefType, class varType>
  static Task<DockedStraightTaskType> 
  construct(STBase&                           st_base,
            const vector<featureBaseCPtr>&    control_constants,
            const vector<controlVarBaseCPtr>& control_variables,
            const Parameters&                 bgaParams,
            const std::string&                dirForOutput);
  

 void ShowFforDefault();

 public: // солверы
  
  /** \brief запуск поиска наилучшего решения по состоянию parameters */
  void SolveForOutput();
  
  /** \brief многочисленный запуск поиска наилучших решений со сбором статистики по ним */
  void SolveForStatistics(const uint16_t amount_of_shots);

  void OutputBestSolution();

 private:  // утилиты для реализации солверов тесно повязанные на this
  
  /** \brief метод, запускающий решение прямой задачи в заданном кол-ве потоков */
  void engage(size_t first_indiv_idx,
              size_t last_indiv_idx);

  /** \brief инициализатор списка параметров, по которым будет
   *  осуществлятся варьирование в процессе эволюции */
  size_t SetCVlist();

  /** \brief метод сортировки популяции в отборе*/
  bool sort_adapted_fraction(bool is_time_to_print);

  void SwapInVect(size_t loser, size_t winner) noexcept;

  void Recombine(Individ& Ind);
  void Mutate(Individ& Ind);

};  // class Task

}  // namespace BGA

#endif  // BGA_TASK_H_

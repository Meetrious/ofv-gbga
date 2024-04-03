#ifndef BGA_TASK_H_
#define BGA_TASK_H_

#include <ofv_bga/evo_pipe.h>
#include <ofv_bga/individ.h>
#include <ofv_bga/st_solver_for_BGA.h>


/* size of the std::array object that contains pointers to parameters that
 * are to variate during BGA iterations */
#define CTV_SIZE 30

namespace BGA {

using std::array;
using std::vector;

template <typename T>
using matrix = vector<vector<T>>;

template <typename StraightTask_t>
class Task final {

  class IOs;
  class Parameters;

 private: // составляющие члены

  /** \brief параметры BGA */
  Parameters m_params;

  /** \brief массив из решателей, которых ровно MAX_AMOUNT_OF_THREADS-штук */
  array<StraightTaskSolverThread<StraightTask_t>, MAX_AMOUNT_OF_THREADS> crew;

  /** \brief индивид, инициализарованный коэф-тами, прописанными по умолчанию */
  Individ default_indiv;

  /** \brief массив из членов популяции в BGA */
  vector<Individ> population;

  /** \brief интерфейс ввода/вывода данных */
  IOs ios;

 public: // пятёрка

  Task(const Task&) = default;
  Task(Task&&) = delete;
  Task& operator=(Task&&) = default;
  Task& operator=(const Task&) = default;
  ~Task() = default;

  // единственный конструктор
  Task(const StraightTask_t&                   stRef,
       const vector<feature_t::base::CnstPtr>& featureBasesPtrs,
       const Parameters&                       bgaParams);

 void ShowFforDefault();

 public: // солверы
  
  /** \brief запуск поиска наилучшего решения по состоянию parameters */
  void SolveForOutput();
  
  /** \brief многочисленный запуск поиска наилучших решений со сбором статистики по ним */
  void SolveForStatistics(const uint16_t amount_of_shots);

  void OutputBestSolution();

 private:  // утилиты для реализации солверов тесно повязанные на this
  
  /** \brief метод, запускающий решение прямой задачи в заданном кол-ве потоков */
  void engage(const size_t first_indiv_idx,
              const size_t last_indiv_idx);

  /** \brief инициализатор списка параметров, по которым будет
   *  осуществлятся варьирование в процессе эволюции */
  size_t SetCVlist();

  /** \brief метод сортировки популяции в отборе*/
  bool Sort1stFrac(bool is_time_to_print);

  void SwapInVect(size_t loser, size_t winner) noexcept;

  void Recombine(Individ& Ind);
  void Mutate(Individ& Ind);
  // void RecombineCarefully(Species& Ind);
  // void MutateCarefully(Species& Ind);
};  // class Task

// ==========================================================================================================

template <typename StraightTask_t>
class Task<StraightTask_t>::Parameters {
  
  template<typename T>
  using matrix = std::vector<std::vector<double>>;
 
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

  /** \brief процент выживания от регулярной численности
   *  после отбора \in (0, 100) */
  unsigned int sorted_fraction;

  /** \brief процент популяции, порождаемой случайным образом
   *  после отбора от исходных границ \in (0, 100) */
  unsigned int newly_recreated_fraction;

  /** \brief кол-во генераций поколений в одном запуске BGA */
  size_t amount_of_iterations;

  /** \brief параметр рекомбинации */
  double rc;

  /** \brief мутационный параметр */
  double mu;

  unsigned int amount_of_threads;

  void display() noexcept;

 private:

  inline unsigned int GetFraction(unsigned int general) noexcept {
    return SortedFraction * (general / 100);
  }
  
};  // class Task::Parameters

// ==========================================================================================================

template <typename StraightTask_t>
class Task<StraightTask_t>::IOs final {
  /* up to this point Individ class should be defined */

 public:
  std::ofstream CSout, Fout, Statout;
  std::ifstream Cin;

  void WriteBest(const Individ& Ind);

  void WriteResult(Individ& Ind);

  void RestartCollector(const Parameters & RT_P,
                        const vector<feature_t> & features);

  void WriteCoefDefBorders(const vector<feature_t> & features);

  void WriteOptimisedCoefs(const vector<feature_t> & features,
                           size_t AOA);

  void WriteStatData(Individ& best_Ind, const char* name);

  void ReadBest(Individ& Ind);

  void ConstructMultiplotScript();

  void ConstructCoefEvoPlotScript(matrix<double>& bounds,
                                  const vector<feature_t> & features);

  void ConstructAberEvoPlotScript();

};  // class Task::IOs

}  // namespace BGA

#endif  // BGA_TASK_H_

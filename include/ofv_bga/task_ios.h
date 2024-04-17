#ifndef BGA_TASK_IOS_H_
#define BGA_TASK_IOS_H_

#include <ofv_bga/individ.h>
#include <ofv_bga/task_parameters.h>

#include <vector>
#include <fstream>

namespace BGA {

class IOs final {
  
  template<typename T>
  using matrix = std::vector<std::vector<T>>;

 public:

  explicit IOs(const std::string& dirForOutput)
    : m_dirForOutput(dirForOutput) {}

  const std::string m_dirForOutput;

  std::ofstream CSout, Fout, Statout;
  std::ifstream Cin;

  /** \brief */
  void WriteBest(const Individ& indiv);

  /** \brief */
  void WriteResult(const Individ& indiv);

  /** \brief */
  void RestartCollector(const Parameters& RT_P, 
                        const std::vector<feature_t>& features);

  /** \brief */
  void WriteCoefDefBorders(const std::vector<feature_t>& features);

  /** \brief */
  void WriteOptimisedCoefs(const std::vector<feature_t>& features, size_t n_attributes);

  /** \brief */
  void WriteStatData(Individ& best_Ind, const char* name);

  /** \brief */
  void ReadBest(Individ& indiv);

  /** \brief */
  void ConstructMultiplotScript();

  /** \brief */
  void ConstructCoefEvoPlotScript(const std::vector<feature_t>& features);

  /** \brief */
  void ConstructAberEvoPlotScript();


};  // class Task::IOs

}  // namespace BGA

#endif  // BGA_TASK_IOS_H_

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

  /** \brief печать списка значений характеристик для данного индивидуума */
  void write_features_out(const Individ& indiv);

  /** \brief печать списка значений характеристик в файл с историей лидеров вдоль поколений */
  void write_features_for_current_generation_out(const Individ& indiv);

  /** \brief печать преамбулы в файлы истории предстоящих поколений,
   *  проверка возможности создания этих файлов. */
  void restart_chronicler(const Parameters& RT_P, 
                        const std::vector<feature_t>& features);

  /** \brief печать списка численных границ характеристик в текущем инстансе BGA-задачи */
  void write_features_bounds_out(const std::vector<feature_t>& features);

  /** \brief печать списка имён характеристик, варьируемых в текущем инстансе BGA-задачи */
  void write_optimized_features_out(const std::vector<feature_t>& features, size_t n_attributes);

  /** \brief печать списка значений характеристик и dfi в файл, аккумулирующий выборку */
  void write_full_result_for_current_generation_out(const Individ& best_indiv, const char* name);

  /** \brief считывание данных индивидуума из файла best.txt в надежде,
   *  что кол-во характеристик в indiv окажется не меньше, 
   * чем кол-во, предоставленное в файле */
  void read_individ_instance_into(Individ& indiv);

  /** \brief построение скрипта для открытия wxt-терминала gnuplot,
   * в котором отобразится график эволюции характеристик с увеличением индекса поколения */
  void construct_feat_evo_plot_script(const std::vector<feature_t>& features);

  /** \brief построение скрипта для открытия wxt-терминала gnuplot,
   *  в котором отобразится график эволюции dif с увеличением индекса поколения */
  void construct_dfi_evo_plot_script();

  /** \brief построение скрипта для открытия wxt-терминала gnuplot,
   * в котором отображатся результаты dfi_evo_plot_script и feat_evo_plot_script */
  void construct_multiplot_script();

};  // class Task::IOs

}  // namespace BGA

#endif  // BGA_TASK_IOS_H_

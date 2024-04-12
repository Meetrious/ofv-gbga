#include <ofv_bga/task_ios.h>
#include <exceptions.h>

#include <iostream>

#define RT_IOS_TEMPLATE(RETURN_TYPE) \
  template <typename StraightTask_t> RETURN_TYPE Task<StraightTask_t>::IOs

namespace BGA {

using std::vector;

void IOs::WriteBest(const Individ& Ind) {
  CSout.open(m_dirForOutput + "/RT/current/best.txt", std::ios_base::out);
  for (auto const& cur : Ind.m_features) CSout << cur.m_value << std::endl;
  CSout.close();
}

void IOs::WriteResult(Individ& Ind) {
  CSout.open(m_dirForOutput + "/RT/current/leaders_feats.txt",
             std::ios_base::out | std::ios_base::app);
  for (auto const& cur : Ind.m_features) CSout << cur.m_value << "\t\t\t";
  CSout << std::endl;
  CSout.close();

  Fout.open(m_dirForOutput + "/RT/current/leaders_dfis.txt",
            std::ios_base::out | std::ios_base::app);
  Fout << Ind.m_dfi_value << std::endl;
  Fout.close();
}

void IOs::RestartCollector(const Parameters& bgaParams,
                           const vector<feature_t> & features) {
  CSout.open(m_dirForOutput + "RT/current/leaders_feats.txt", std::ios_base::out);
  if (!CSout) {
    std::cerr << "\n !!!stream for coeficients was not allocated for some reason.\n Care "
                 "to attend!";
    std::cin.get();
    return;
  }

  CSout << '#' << "mutation_parameter = " << bgaParams.mutation_val << "\t"
        << "recombination_parameter = " << bgaParams.recombination_val << "\t"
        << "amount of generations in one BGA launch" << bgaParams.amount_of_iterations << "\t"
        << "population : " << bgaParams.initial_p << " -> " << bgaParams.regular_p << std::endl;

  CSout << "#";
  for (const auto & feat:features) CSout << feat.get_name();

  CSout << std::endl;

  CSout.close();

  Fout.open(m_dirForOutput + "RT/current/leaders_dfis.txt", std::ios_base::out);
  if (!Fout) {
    std::cerr << "\n !!!stream for dfi-values" 
              << " was not allocated for some reason.\n"
              << "Throwing exception!";
    throw external_file_allocation_error();
  }
  Fout.close();
}

void IOs::WriteCoefDefBorders(const vector<feature_t> & features) {
  CSout.open(m_dirForOutput + "/RT/accumul/CoefBorders.txt");
  if (!CSout) {
    std::cerr << "\n For some reason " 
              << "CoefBorders.txt was not created file \n"
              << "Throwing exception.";
    throw external_file_allocation_error();
  }
  for (size_t i = 0; i < features.size(); i++) {
    CSout << features[i].get_left_boundary() << " \t" 
          << features[i].get_right_boundary() << std::endl;
  }
  CSout.close();
}

void IOs::WriteOptimisedCoefs(
   const vector<feature_t> & features, size_t AOA) {
  CSout.open(m_dirForOutput + "/RT/accumul/OptCoefNames.txt", std::ios_base::out);
  if (!CSout) {
    std::cout << "\n For some reason I couldn't create OptCoefNames.txt file"
              << std::endl;
    std::cin.get();
    return;
  }
  for (size_t i = 0; i <= AOA; i++) {
    CSout << features[i].get_name() << "\n";
  }
  CSout.close();
}

void IOs::WriteStatData(Individ& best_Ind, const char* name) {
  Statout.open(m_dirForOutput + "/RT/accumul/" + name + ".txt",
               std::ios_base::out | std::ios_base::app);
  for (auto const& cur : best_Ind.m_features) Statout << cur.m_value << "\t\t\t";
  Statout << best_Ind.m_dfi_value << std::endl;
  Statout.close();
}

void IOs::ReadBest(Individ& Ind) {
  // Ind.m_features.clear();  /// \note: очищать нельзя, ведь не существует конструктора feature_t по значению
  Cin.open(m_dirForOutput + "RT/current/best.txt");
  if (!Cin) {
    std::cerr << "\n stop this maddness right now, "
              << "and give me proper direction for best "
              << "RT-solution data!\n";
    return;
  }
  const size_t amount_of_feats = Ind.m_features.size();
  size_t i = 0u;
  while (!Cin.eof()) {
    if (i < amount_of_feats) {
      double inputed_value;
      Cin >> inputed_value;
      Ind.m_features[i++].m_value = inputed_value;
    } else break;
  }
  Cin.close();
}

void IOs::ConstructMultiplotScript() {
  CSout.open(m_dirForOutput + "RT/plot_scripts/leaders_evo.plt", std::ios_base::out);
  if (!CSout) {
    std::cout << " \n Failed to create the leaders_evo.plt script in the given folder; "
              << " \n external_file_allocation_error is thrown";

    throw external_file_allocation_error();
  }
  CSout << "set terminal wxt size 1900, 1000" << std::endl
        << "set multiplot " << std::endl
        << "set size 0.75, 0.96" << std::endl
        << "set origin 0, 0 " << std::endl
        << "load \'C_evo.plt\' " << std::endl

        << "set size 0.27, 0.986" << std::endl
        << "set origin 0.73, 0 " << std::endl
        << "#set x2tics(sprintf(\"min(F) = %3.8f\", fmin)fmin)" << std::endl
        << "set xrange[0.2:0.6] " << std::endl
        << "load \'F_evo.plt\' " << std::endl

        << "unset multiplot" << std::flush;
  CSout.close();
}

void IOs::ConstructCoefEvoPlotScript(const vector<feature_t> & features) {
  double max_right_boundary = 0;

  // looking for the biggest Right bound
  for (auto const& feat : features) {
    const double cur_right_boundary = feat.get_right_boundary();
    if (cur_right_boundary > max_right_boundary) {
      max_right_boundary = cur_right_boundary;
    }
  }

  CSout.open(m_dirForOutput + "RT/plot_scripts/C_evo.plt", std::ios_base::out);
  if (!CSout.is_open()) {
    std::cerr << " \n Failed to create the C_evo.plt script in the given folder; "
              << " \n external_file_allocation_error is thrown";
    throw external_file_allocation_error();
  }

  CSout << "set border 9" << std::endl
        << "set xtics nomirror \n"
        << std::endl
        << "set nox2tics" << std::endl
        << "set xrange[-0.5:" << max_right_boundary << "]\n"
        << "set grid xtics ytics \n"
        << std::endl

        << "plot \"" << m_dirForOutput << "RT/current/leaders_feats.txt\" \\\n"
        << "u 1:0 with points ls 1 title \"" << features[0].get_name() << "\","
        << " \"\" u 1:0 with lines ls 1 title \"" << features[0].get_name() << "\",\\"
        << std::endl;
  for (size_t i = 2; i < features.size() + 1; ++i) {
    CSout << "\"\" u " << i << ":0 with points ls " << i << " title \""
          << features[i - 1].get_name() << "\", "
          << "\"\" u " << i << ":0 with lines ls " << i << " title \""
          << features[i - 1].get_name() << "\",\\" << std::endl;
  }
  CSout.close();
}

void IOs::ConstructAberEvoPlotScript() {
  CSout.open(m_dirForOutput + "RT/plot_scripts/F_evo.plt", std::ios_base::out);
  if (!CSout) {
    std::cout << " \n Failed to create the F_evo.plt script in the given folder; "
              << " \n external_file_allocation_error is thrown";
    external_file_allocation_error exception;
    throw(exception);
  }
  CSout
      << "set x2tics scale 0" << std::endl

      << "set border 3" << std::endl
      << "set ytics nomirror" << std::endl
      << "set grid xtics ytics" << std::endl

      << "plot \"" << m_dirForOutput << "RT/current/leaders_dfis.txt\" \\\n"
      << "u 1:0 with points lt 8 pt 7 ps 1 t \"deviation_from_ideal\", "
      << "\"\" u 1 : 0 with lines lt 1 notitle"
      << std::endl

      << "unset grid" << std::flush;
  CSout.close();
}

}  // namespace BGA

#undef RT_IOS_TEMPLATE

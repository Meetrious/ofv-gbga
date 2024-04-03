#include <ofv_bga/task.h>

namespace BGA {

#define RT_IOS_TEMPLATE(RETURN_TYPE) \
  template <typename StraightTask_t> RETURN_TYPE Task<StraightTask_t>::IOs

RT_IOS_TEMPLATE(void)::WriteBest(const Individ& Ind) {
  CSout.open(output_dir + "/RT/current/best.txt", std::ios_base::out);
  for (auto const& cur : Ind.m_features) CSout << cur.m_value << std::endl;
  CSout.close();
}

RT_IOS_TEMPLATE(void)::WriteResult(Individ& Ind) {
  CSout.open(output_dir + "/RT/current/leaders_C.txt",
             std::ios_base::out | std::ios_base::app);
  for (auto const& cur : Ind.m_features) CSout << cur << "\t\t\t";
  CSout << std::endl;
  CSout.close();

  Fout.open(output_dir + "/RT/current/leaders_F.txt",
            std::ios_base::out | std::ios_base::app);
  Fout << Ind.F_value << std::endl;
  Fout.close();
}

RT_IOS_TEMPLATE(void)::RestartCollector(const Parameters& bgaParams,
                                        const vector<feature_t> & features) {
  CSout.open(output_dir + "RT/current/leaders_C.txt", std::ios_base::out);
  if (!CSout) {
    std::cout << "\n !!!stream for coeficients was not allocated for some reason.\n Care "
                 "to attend!";
    std::cin.get();
    return;
  }

  CSout << '#' << "mu = " << bgaParams.mu << "\t"
        << "d = " << bgaParams.rc << "\t"
        << "N_gen" << bgaParams.amount_of_iterations << "\t"
        << "population : " << bgaParams.p_0 << " -> " << bgaParams.p << std::endl;

  CSout << "#";
  for (size_t i = 0; i <= bgaParams.amount_of_attributes; ++i)
    // CSout << CoefsToVariate[i]->name << "\t";
    CSout << features[i].get_name();

  CSout << std::endl;

  CSout.close();

  Fout.open(output_dir + "RT/current/leaders_F.txt", std::ios_base::out);
  if (!Fout) {
    std::cout << "\n !!!stream for F-values was not allocated for some reason.\n Care to "
                 "attend!";
    std::cin.get();
    return;
  }
  Fout.close();
}

RT_IOS_TEMPLATE(void)::WriteCoefDefBorders(const vector<feature_t> & features) {
  CSout.open(output_dir + "/RT/accumul/CoefBorders.txt");
  if (!CSout) {
    std::cout << "\n For some reason I couldn't create CoefBorders.txt file" << std::endl;
    std::cin.get();
    return;
  }
  for (size_t i = 0; i < bounds[0].size(); i++) {
    CSout << features[i].get_left_boundary() << " \t" << features[i].get_right_boundary() << std::endl;
  }
  CSout.close();
}

RT_IOS_TEMPLATE(void)::WriteOptimisedCoefs(
   const vector<feature_t> & features, size_t AOA) {
  CSout.open(output_dir + "/RT/accumul/OptCoefNames.txt", std::ios_base::out);
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

RT_IOS_TEMPLATE(void)::WriteStatData(Individ& best_Ind, const char* name) {
  Statout.open(output_dir + "/RT/accumul/" + name + ".txt",
               std::ios_base::out | std::ios_base::app);
  for (auto const& cur : best_Ind.m_features) Statout << cur.m_value << "\t\t\t";
  Statout << best_Ind.m_dfi_value << std::endl;
  Statout.close();
}

RT_IOS_TEMPLATE(void)::ReadBest(Individ& Ind) {
  Ind.coefs_values.clear();
  Cin.open(output_dir + "RT/current/best.txt");
  if (!Cin) {
    std::cout << "\n stop this maddness right now, and give me proper direction for best "
                 "RT-solution data!\n";
    return;
  }
  while (!Cin.eof()) {
    double tmp;
    Cin >> tmp;
    Ind.coefs_values.emplace_back(tmp);
  }
  Cin.close();
}

RT_IOS_TEMPLATE(void)::ConstructMultiplotScript() {
  CSout.open(output_dir + "RT/plot_scripts/leaders_evo.plt", std::ios_base::out);
  if (!CSout) {
    std::cout << " \n Failed to create the leaders_evo.plt script in the given folder; "
              << " \n external_file_allocation_error is thrown";
    external_file_allocation_error exception;
    throw(exception);
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

RT_IOS_TEMPLATE(void)::ConstructCoefEvoPlotScript(
    matrix<double>& bounds, const vector<feature_t> & features) {
  double R = 0;

  // looking for the biggest Right bound
  for (auto const& cur : bounds[1]) {
    if (cur > R) R = cur;
  }

  CSout.open(output_dir + "RT/plot_scripts/C_evo.plt", std::ios_base::out);
  if (!CSout.is_open()) {
    std::cout << " \n Failed to create the C_evo.plt script in the given folder; "
              << " \n external_file_allocation_error is thrown";
    external_file_allocation_error exception;
    throw(exception);
  }

  CSout << "set border 9" << std::endl
        << "set xtics nomirror \n"
        << std::endl
        << "set nox2tics" << std::endl
        << "set xrange[-0.5:" << R << "]" << std::endl
        << "set grid xtics ytics \n"
        << std::endl

        << "plot \"" << output_dir << "RT/current/leaders_C.txt\" \\" << std::endl
        << "u 1:0 with points ls 1 title \"" << CoefsToVariate[0]->name << "\","
        << " \"\" u 1:0 with lines ls 1 title \"" << CoefsToVariate[0]->name << "\",\\"
        << std::endl;
  for (size_t i = 2; i < bounds[0].size() + 1; ++i) {
    CSout << "\"\" u " << i << ":0 with points ls " << i << " title \""
          << CoefsToVariate[i - 1]->name << "\", "
          << "\"\" u " << i << ":0 with lines ls " << i << " title \""
          << CoefsToVariate[i - 1]->name << "\",\\" << std::endl;
  }
  CSout.close();
}

RT_IOS_TEMPLATE(void)::ConstructAberEvoPlotScript() {
  CSout.open(output_dir + "RT/plot_scripts/F_evo.plt", std::ios_base::out);
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

      << "plot \"" << output_dir << "RT/current/leaders_F.txt\" \\" << std::endl
      << "u 1:0 with points lt 8 pt 7 ps 1 t \"F\", \"\" u 1 : 0 with lines lt 1 notitle"
      << std::endl

      << "unset grid" << std::flush;
  CSout.close();
}

}  // namespace BGA
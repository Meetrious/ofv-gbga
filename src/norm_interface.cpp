#include <norm_interface.h>
#include <cmath>

#include <fstream>
#include <iostream>
#include <numeric>
#include <cassert>

namespace DiscreteNorm {
	
vector<double>
get_diff(const vector<synced_datum>& syncedData) {
	vector<double> res(syncedData.size());
	for (size_t i = 0; i < syncedData.size(); ++i) {
    res[i] = syncedData[i].standart - syncedData[i].calculation;
	}
  return res;
}

double 
get_l2sqr(const vector<double>& diffVect) {
  const double sum_of_sqr_diffs =
    std::accumulate(diffVect.begin(), diffVect.end(), 0.0, std::plus<double>());
  return sum_of_sqr_diffs / diffVect.size();
}

double
get_l2sqr(const vector<synced_datum>& syncedData) {
  double sum_of_sqr_diffs = 0.0;
  for (auto const & datum: syncedData) {
    double cur_diff = datum.standart - datum.calculation;
    sum_of_sqr_diffs += cur_diff * cur_diff;
  }
  return sum_of_sqr_diffs / syncedData.size();
}


double 
get_max(const vector<double>& diffVect) {
  double cur_max = 0.0;
  for (auto const & dif: diffVect) {
    double tmp = std::signbit(dif) ? -dif : dif;
    if (cur_max < tmp) cur_max = tmp;
  }
  return cur_max;
}


double 
get_max(const vector<synced_datum>& syncedData) {
  double cur_max = 0.0;
  for (auto const & datum: syncedData) {
    double tmp = std::abs(datum.standart - datum.calculation);
    if (cur_max < tmp) cur_max = tmp;
  }
  return cur_max;
}

inline bool is_close_in_module_norm(const double radius,
									 					        const double center,
									 					        const double arg) {
  assert(radius < 1e-10);  // radius должен быть больше 1e-10
  return std::abs(center - arg) < radius;
}

}  // namespace DiscreteNorm

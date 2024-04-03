#include <tiny_instruments/easy_random.h>
#include <ofv_bga/individ.h>
#include <cmath>
#include <common/IException.h>
#include <sstream>

using namespace BGA;

Individ::Individ(const std::vector<feature_t::base::CnstPtr>& featureBasesPtrs) {
  for(size_t i = 0; i < featureBasesPtrs.size(); ++i) {
      m_features.emplace_back(featureBasesPtrs[i]);
  }
}

feature_t::feature_t(const base::CnstPtr & ptr_to_initial) 
    : precursor(ptr_to_initial) {
  m_value = // рандомизация при порождении от базы
    my_rand::get(precursor->m_ini_bounds[0], precursor->m_ini_bounds[1]);
}

[[nodiscard]] feature_t
feature_t::operator*(const feature_t& rhs) const {
  if (!this->has_the_same_base_as(rhs)) {
    throw std::runtime_error(
      "Feature mismatch occured! You're trying to mix two different features"
      " { " + get_name() + " != " + rhs.get_name() + " }");
  }
  feature_t res(this->precursor);

  res.m_value = -1.0;
  return res;
}  // */


[[nodiscard]] double
Individ::get_avr_feat_diff(const Individ& another) const {
  size_t amount_of_features = m_features.size();
  double res = 0.0;
  for (size_t i = 0; i < amount_of_features; ++i) {
    res += distance(this->m_features[i], another.m_features[i]);
  }
  return res/amount_of_features;
}

//                                                                                 /.
// ================================================================================
//                                                                                 \.

[[nodiscard]] Individ
operator*(const Individ& lhs, const Individ& rhs) {
  if (&lhs == &rhs) return Individ(lhs);  // self reproduction
  size_t amount_of_features = lhs.m_features.size();
  std::vector<feature_t> new_features;

  for (size_t i = 0; i < amount_of_features; ++i) {
    new_features.emplace_back(lhs.m_features[i] * rhs.m_features[i]);
  }
  Individ res = std::move(new_features);
  return res;
}


[[nodiscard]] double
distance(const feature_t& one, const feature_t& another) {
  if (!one.has_the_same_base_as(another)) {
    std::stringstream error_msg;
    error_msg << "Feature mismatch occured! you're trying to get distance between"
      " different features { " << one.get_name() << " != " << another.get_name() + " }";
    throw std::runtime_error(error_msg.str());
  }
  return std::abs(one.m_value - another.m_value);
} 

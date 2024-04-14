#include <ofv_bga/individ.h>
#include <cmath>
#include <common/IException.h>
#include <sstream>
#include <cassert>

// Use (void) to silence unused warnings.
#define assertm(exp, msg) assert(((void)msg, exp))

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

[[nodiscard]] double
Individ::get_avr_feat_diff(const Individ& another) const {
  size_t amount_of_features = m_features.size();
  double res = 0.0;
  for (size_t i = 0; i < amount_of_features; ++i) {
    res += distance(this->m_features[i], another.m_features[i]);
  }
  return res/amount_of_features;
}

double
feature_t::replace_with_combination_of(const feature_t& l_feature,
                                       const feature_t& r_feature,
                                       const double position) {

  assertm(l_feature.has_the_same_base_as(r_feature),
          "Different features are being recombined!");

  m_value = l_feature + position * (r_feature - l_feature);
  return m_value; 
}

void
Individ::replace_with_combination_of(const Individ& lhs,
                                     const Individ& rhs,
                                     const double recomb_param) {
  // если this участвует в рекомбинации, то this не должен быть продуктом рекомбинации
  assertm((this == &lhs) or (this == &rhs),
          "individ is used for recombination yet participates in it");

  if (&lhs == &rhs) *this = lhs.m_features;  // self reproduction

  size_t amount_of_features = lhs.m_features.size();

  for (size_t i = 0; i < amount_of_features; ++i) {
    const double pos = my_rand::get(-recomb_param, 1.0 + recomb_param);
    m_features[i].replace_with_combination_of(lhs.m_features[i], rhs.m_features[i], pos);
  }
  ++generation_count;
}

// ================================================================================

[[nodiscard]] feature_t
operator*(const feature_t& lhs, const feature_t& rhs) {
  if (&lhs == &rhs) return lhs;

  if (!lhs.has_the_same_base_as(rhs)) {
    throw std::runtime_error(
      "Feature mismatch occured! You're trying to mix two different features"
      " { " + lhs.get_name() + " != " + rhs.get_name() + " }");
  }
  
  feature_t res(lhs);
  res.m_value = my_rand::get(lhs, rhs);
  return res;
}

[[nodiscard]] Individ
operator*(const Individ& lhs, const Individ& rhs) {
  if (&lhs == &rhs) return Individ(lhs);  // self reproduction
  size_t amount_of_features = lhs.m_features.size();
  std::vector<feature_t> new_features;

  for (size_t i = 0; i < amount_of_features; ++i) {
    new_features.emplace_back(BGA::operator*(lhs.m_features[i], rhs.m_features[i]));
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

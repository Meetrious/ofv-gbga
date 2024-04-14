#ifndef BGA_SPECIES_H_
#define BGA_SPECIES_H_

#include <common/shd_ptr_mod.h>
#include <tiny_instruments/easy_random.h>

#include <vector>
#include <array>
#include <string>

namespace BGA {

class feature_t final {
 public:
  class base final: public Tools::SharedPointerMod<base> {

   public:  /* Поля \_______________
    +                                \*/
    std::string m_base_name;
    std::array<double, 2> m_ini_bounds;

    /** \brief конструктор для полного заполнения */
    base(const std::string & name, double left, double right)
    : m_base_name(name) {
      if (left < right) {
        m_ini_bounds[0] = left;
        m_ini_bounds[1] = right;
      } else {
        m_ini_bounds[0] = right;
        m_ini_bounds[1] = left;
      }
    }

   public:  /* BIG_5 \_______________________
    +                                         \*/
    base(const base &) = delete;
    base(base &&) = delete;
    base& operator=(const base &) = delete;
    base& operator=(base &&) = delete;
    ~base() = default;
  };

 public:  /* Поля \______________________
  +                                       \*/
  double m_value;
  operator double() const { return m_value; }
  
  /** \brief значимость значения в системе */
  [[maybe_unused]]double m_relevance;  // \in[0,1]

 public:  /* BIG_5 \_______________________
  +                                         \*/
  feature_t(const feature_t&) = default;
  feature_t(feature_t&&) = default;
  feature_t& operator=(const feature_t&) = default;
  feature_t& operator=(feature_t&&) = default;
  ~feature_t() = default;
  
 public:  /* способы порождения \_________
  +                                        \*/
  /** \brief порождение существующей базой */
  feature_t(const base::CnstPtr & ptr_to_initial);

 public:  /* геттеры \______________________________________________________
  +                                                                          \*/
  /** \brief ссылка на std::string с именем базы */
  inline const std::string& 
  get_name() const & {return precursor->m_base_name; }

  /** \brief явный оператор приведения feature_t в си-строку.
   * \returns строка с именем базы */
  explicit operator const char*() const {return precursor->m_base_name.c_str();}

  /** \brief метод возвращает левую границу при генерации значения m_value
   *  в нулевом поколении популяции */
  inline double get_left_boundary() const noexcept { return precursor->m_ini_bounds[0]; }

   /** \brief метод возвращает правую границу при генерации значения m_value
    *  в нулевом поколении популяции */
  inline double get_right_boundary() const noexcept { return precursor->m_ini_bounds[1]; }

  double replace_with_combination_of(const feature_t& l_feature,
                                     const feature_t& r_feature,
                                     double position);

  inline double randomize_in_base_bounds() {
    m_value = my_rand::get(precursor->m_ini_bounds[0], precursor->m_ini_bounds[1]);
    return m_value;
  }

  /** \brief предикат общей базы */
  inline bool
  has_the_same_base_as(const feature_t & another) const noexcept {
     return (precursor.get() == another.precursor.get());
  }

 private:
 
  /** \brief const shared_ptr на базу 
   * с границами порождения для m_value и именем характеристики */
  base::CnstPtr precursor;
};

/** \brief порождение перемножением */
[[nodiscard]]feature_t
operator*(const feature_t& lhs, const feature_t& rhs);


struct Individ final {

  // attributes of current individual
  std::vector<feature_t> m_features;

  /** \brief deviation_from_ideal; 
   * значение функционала невязки на решении
   * от текущего индивидуума */ 
  double m_dfi_value = -1.0;

  /** \brief move-конструктор от набора характеристик */
  Individ(std::vector<feature_t>&& new_features) :
    m_features(std::move(new_features)) {}
    
  /** \brief copy-конструктор от набора характеристик */
  Individ(const std::vector<feature_t>& new_features) :
    m_features() {}

  /** \brief copy-assignment для переопределение вектором характеристик */
  inline Individ& operator=(const std::vector<feature_t>& another_features) {
    m_features = another_features; return *this;
  }

  /** \brief move-assignment для переопределение вектором характеристик */
  inline Individ& operator=(std::vector<feature_t>&& another_features) {
      m_features = std::move(another_features); return *this;
  }
  
  Individ(const Individ&) = default;
  Individ(Individ&&) = default;
  Individ& operator=(const Individ&) = default;
  Individ& operator=(Individ&&) = default;
  ~Individ() = default;

   /** \brief конструктор от вектора указателей на базовые фитчи */
  Individ(const std::vector<feature_t::base::CnstPtr>& feature_bases);

  inline bool 
  operator<(const double value) const noexcept 
  { return m_dfi_value < value; }

  inline bool 
  operator<(const Individ& another) const noexcept
  { return m_dfi_value < another.m_dfi_value; }
  
  /** \brief расчёт средней разницы между между id-атрибутов*/
  double get_avr_feat_diff(const Individ& other) const;

  void replace_with_combination_of(const Individ& lhs,
                                   const Individ& rhs,
                                   double recomb_param);

  inline size_t get_amount_of_features() const noexcept { return m_features.size(); }

  void randomize_in_base_bounds() {
    for(auto & feature: m_features) feature.randomize_in_base_bounds();
  }
 
 private:

  /** \brief порядковый номер текущего поколения */
  unsigned long generation_count;

  // /** \brief hash-по именам */
  // unsigned char hash[SHA_DIGEST_LENGTH];  
};

/** \brief перемножение индивидуумов */
[[nodiscard]] Individ 
operator*(const Individ& lhs, const Individ& rhs);

[[nodiscard]]double 
distance(const feature_t& one, const feature_t& another);

}  // namespace BGA

#endif  // BGA_SPECIES_H_

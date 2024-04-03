#ifndef NORM_INTERFACE_H_
#define NORM_INTERFACE_H_

#include <vector>

namespace common {
	/** \brief структура с данными эталона и численного решения,
	 * синхронизированными по аргументу.
	 * \param standart - принятое эталонное значение.
	 * \param calculation - вычисленное решение; данные,
	 *  которые требуется притянуть к стандарту.
	 * \param arg независимый аргумент, по которому синхронизируются данные (время, х). */
	struct synced_datum { 
		double arg = -1.0;
		double calculation = -1.0;
		double standart = -1.0;
	};
}

namespace DiscreteNorm {
	
	using std::vector;
	using common::synced_datum;
	
	/** \brief расчёт отличия эталона от результата вычислений.
	 * \param syncedData набор синхронизированных данных.
	 * \returns вектор разницы эталона и численного решения
	 *  вдоль синхронизированных аргументов. */
	vector<double> get_diff(const vector<synced_datum>& syncedData);

	/** \brief расчёт относительной погрешности в l2-смысле 
	 * по вектору разницы.
	 * \param diffVect вектор разницы синхронизированных данных.
	 * \returns относительная погрешность в l2-смысле. */
	double get_l2sqr(const vector<double>& diffVect);

	/** \brief расчёт относительной погрешности в l2-смысле
	 * по набору синхронизированных данных.
	 * \param syncedData набор синхронизированных данных.
	 * \returns относительная погрешность в l2-смысле. */
	double get_l2sqr(const vector<synced_datum>& syncedData);

	/** \brief расчёт относительной погрешности в смысле max-нормы
	 * по вектору разницы
	 * \param diffVect вектор разницы синхронизированных данных.
	 * \returns относительная погрешность в смысле max-нормы. */
	double get_max(const vector<double>& diffVect);

	/** \brief расчёт относительной погрешности в смысле max-нормы 
	 * по набору синхронизированных данных.
	 * \param syncedData набор синхронизированных данных.
	 * \returns относительная погрешность в смысле max-нормы. */
	double get_max(const vector<synced_datum>& syncedData);

	/** \brief расчёт принадлежности числа arg 
	 * к radius-окресности точки center. */
	bool is_close_in_module_norm(const double radius,
									 						 const double center,
									 						 const double arg);

}

#endif  // NORM_INTERFACE_H_

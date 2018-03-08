#pragma once

#include <QString>
#include <stdint.h>

/*!
 * Шаг, с которым соденяется вершина графа.
 */
struct vertex_connect_struct {
	QString			connect_step;		/// Имя шага.
	uint32_t		number;				/// При каком возвращаемом значении
										/// метода вершины графа происходит
										/// переход на этот шаг.

	/// Оператор сравнения для сортировки вектора по возрастанию
	/// number, чтобы потом можно было, отсортировав вектор
	/// vertex_connect_struct-ов найти пробелы.
	bool operator < ( const vertex_connect_struct& s ) const {
			return ( number < s.number );
	}
};

/*!
 * Описание вершины графа.
 */
struct vertex_struct {
	QString								step_name;			/// Имя шага.
	QString								func_name;			/// Имя функции, вызываемой в вершине.
	QVector< vertex_connect_struct >*	vertex_connect;		/// С какими вершинами соединен.
};


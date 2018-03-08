#pragma once

#include <QFile>
#include <QString>
#include <QVector>
#include <QRegularExpression>

#include "def.h"

/*!
 * Черная магия, эльфийский.
 */
#define REG_EXP_TEMPLATE_ENTRY_POINT					"^\\[\\*]\\s*-->\\s*\\w*\\s*\n$"
#define REG_EXP_TEMPLATE_ENTRY_POINT_REMOVE_LEFT		"^\\[\\*]\\s*-->\\s*"
#define REG_EXP_TEMPLATE_ENTRY_POINT_REMOVE_RIGHT							   "\\s*\n$"


#define REG_EXP_TEMPLATE_SEARCH_FUNC_NAME				"^state\\s*\"\\w*\"\\s*as\\s*\\w*\\s*{\\s*\n$"

#define REG_EXP_TEMPLATE_STEP_NAME_GET_LEFT				"^state\\s*\"\\w*\"\\s*as\\s*"
#define REG_EXP_TEMPLATE_STEP_NAME_GET_RIGHT											"\\s*{\\s*\n$"

#define REG_EXP_TEMPLATE_FUNC_NAME_GET_LEFT				"^state\\s*\""
#define REG_EXP_TEMPLATE_FUNC_NAME_GET_RIGHT							"\"\\s*as\\s*\\w*\\s*{\\s*\n$"

#define REG_EXP_TEMPLATE_SEARCH_CONNECT					"^\\w*\\s*-->\\s*\\w*\\s*:\\s*\\d*\\s*\n$"

#define REG_EXP_TEMPLATE_STEPIN_NAME_GET_RIGHT				 "\\s*-->\\s*\\w*\\s*:\\s*\\d*\\s*\n$"

#define REG_EXP_TEMPLATE_STEPOUT_NAME_GET_LEFT			"^\\w*\\s*-->\\s*"
#define REG_EXP_TEMPLATE_STEPOUT_NAME_GET_RIGHT								"\\s*:\\s*\\d*\\s*\n$"

#define REG_EXP_TEMPLATE_NUMBER_GET_LEFT				"^\\w*\\s*-->\\s*\\w*\\s*:\\s*"
#define REG_EXP_TEMPLATE_NUMBER_GET_RIGHT												 "\\s*\n$"

/*!
 * Метод производит поиск строки в файле, удовлетворяющей
 * требованиям регулярного выражения REG_EXP_TEMPLATE_ENTRY_POINT,
 * читаемого как:
 * Строка должна начинаться с подстроки <<[*]>> после чего
 * может идти любой символ отступа или не быть его вовсе,
 * далее должна идти подстрока "-->", за ней символ отступа
 * или ничего, после имя шага латинскими символами с
 * допуском нижнего подчеркивания
 * далее отступ или ничего и 2 символа переноса строки
 * (в QString символ переноса строки не 1, а их 2).
 * \param[in]	file_path - путь к файлу с plantuml.
 * \return		Строка с именем шага, если таковой
 *				был найден или пустая строка.
 */
QString search_entry_point_step_name ( char* file_path );


/*!
 * Метод производит поиск имени функции в файле,
 * соответствующую имени шага.
 * \param[in]	file_path - путь к файлу с plantuml.
 * \param[in]	step_name - имя шага.
 *  \return		Строка с именем функции, соответствующей
 *				имени шага или пустая строка, если таковая не была
 *				найдена.
 */
int search_func_name_and_step_name ( char* file_path, QVector< vertex_struct >* tree );

/*!
 * Метод заполняет вектор vertex_connect_struct у каждой вершины графа.
 * \param[in]	file_path - путь к файлу с plantuml.
 * \param[in]	tree - составленное дерево вершин.
 * \return		Успешность соединения вершин.
 */
int search_connect_step ( char* file_path, QVector< vertex_struct >* tree );

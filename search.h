#pragma once

#include <QFile>
#include <QString>
#include <QVector>
#include <QRegularExpression>

#include "def.h"

/*!
 * ССогласно этой структуре будет формироваться вывод дерева.
 */
struct treePrintFormat {
	/// Путь и имя выходного файла с расширением.
	char*							fileOutPath;

	/// Имя класса, в который будут входить методы fsm.
	QString*						className;

	/// Разобранный граф.
	QVector< vertex_struct >*		tree;

	/// Имя include файла, в котором хранится описание используемого класса.
	char*							handlerClassName;

	/*!
	 * false:
	 * Входные параметры:
	 * Имя класса:	ayplayer
	 * Шаг:			base_object_init
	 *
	 * Имя структуры-старта:
	 * ayplayer_class_base_object_init_fsm_step
	 *
	 * Имя шага в объекте.
	 * fsm_step_base_object_init
	 *
	 *
	 * true:
	 * Входные параметры:
	 * Имя класса:	AyPlayer
	 * Шаг:			baseObjectInit
	 *
	 * Имя структуры-старта:
	 * ayplayerClassBaseObjectInitFsmStep
	 *
	 * Имя шага в объекте.
	 * fsmStepBaseObjectInit
	 */
	bool							flagCamelcase;
};

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
 * Вычленяет параметры вывода.
 */
/// Пример: CLASS_NAME						=		AyPlayer
#define	REG_EXP_TEMPLATE_PARAM_AND_ARG					"^\\s*\\S*\\s*=\\s*\\S*\\s*\n$"

/// Вычленяет: CLASS_NAME
#define	REG_EXP_TEMPLATE_PARAM_REMOVE_LEFT				"^\\s*"
#define	REG_EXP_TEMPLATE_PARAM_REMOVE_RIGHT						 "\\s*=\\s*\\S*\\s*\n$"

/// Вычленяет: AyPlayer
#define	REG_EXP_TEMPLATE_ARG_REMOVE_LEFT				"^\\s*\\S*\\s*=\\s*"
#define	REG_EXP_TEMPLATE_ARG_REMOVE_RIGHT									  "\\s*\n$"

/*!
 * Ищет в файле строку типа:
 * ПАРАМЕТР			=		АРГУМЕНТ
 * Возвращает указатель на строку с аргументом.
 */
char* getFsmClassArg ( const char* const pathInputFile, QString parameter );

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
QString searchEntryPointStepName ( char* file_path );


/*!
 * Метод производит поиск имени функции в файле,
 * соответствующую имени шага.
 * \param[in]	file_path - путь к файлу с plantuml.
 * \param[in]	step_name - имя шага.
 *  \return		Строка с именем функции, соответствующей
 *				имени шага или пустая строка, если таковая не была
 *				найдена.
 */
int searchFuncNameAndStepName ( char* file_path, QVector< vertex_struct >* tree );

/*!
 * Метод заполняет вектор vertex_connect_struct у каждой вершины графа.
 * \param[in]	file_path - путь к файлу с plantuml.
 * \param[in]	tree - составленное дерево вершин.
 * \return		Успешность соединения вершин.
 */
int searchConnectStep ( char* file_path, QVector< vertex_struct >* tree );

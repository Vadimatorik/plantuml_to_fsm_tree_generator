#pragma once

#include <QFile>
#include <QString>
#include <QVector>
#include <QRegularExpression>
#include "def.h"

/*!
 * Метод создает выходной файл на основе заранее разобранного графа.
 * \param[in]	file_path - путь и имя выходного файла с расширением.
 * \param[in]	obj_class - имя класса, в который будут входить методы fsm.
 * \param[in]	tree - разобранный граф.
 */
int create_output_file ( char* file_path, QString obj_class, QVector< vertex_struct >* tree );

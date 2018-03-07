#pragma once

#include <QFile>
#include <QString>
#include <QRegularExpression>

/*!
 * Метод ищет в planuml файле функцию точки входа.
 * \param[in]	file_path - путь к plantuml файлу.
 * \return		Возвращает имя функции, с которой начинается выполнение графа.
 *				В случае отсутствия точки входа возвращает пустую строку.
 */
QString search_entry_point ( char* file_path );

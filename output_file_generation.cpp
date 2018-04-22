#include "output_file_generation.h"
#include <iostream>

using namespace std;


static QString get_name_func_from_name_step ( QString name_step, QVector< vertex_struct >* tree ) {
	for ( int l = 0; l < tree->size(); l++ ) {
		if ( tree->at( l ).stepName == name_step ) {
			return tree->at( l ).funcName;
		}
	}
	return "";
}

static void abort_parsing ( QString& message, uint32_t code ) {
	QByteArray b_byte_string = message.toUtf8();
	char* b_write = b_byte_string.data();
	cout << b_write << endl;
	exit( code );
}

static void writeToFile ( QFile& f, QString& s ) {
	QByteArray b_byte_string = s.toUtf8();
	char* b_write = b_byte_string.data();
	f.write( b_write );
}

/*!
 * Если выставлен флаг Camelcase, но первая буква становится заглавной.
 */
static QString upOneCharIfCamelcaseUp ( const QString inputString, const treePrintFormat& cfg ) {
	if ( cfg.flagCamelcase ) {
		QString bufSt = inputString;
		bufSt.replace( 0, 1, QChar( bufSt.at(0).toUpper() ) );
		return bufSt;
	} else {
		return inputString;
	}
}

/*!
 * Делаем первый символ маленьиким.
 */
static QString oneCharDown ( const QString inputString ) {
	QString bufSt = inputString;
	bufSt.replace( 0, 1, QChar( bufSt.at(0).toLower() ) );
	return bufSt;
}

/*!
 * Если Camelcase не выставлен, то возвращает нижнее подчеркивание.
 * Иначе пустая строка.
 */
static QString setBottomUnderline ( const treePrintFormat& cfg ) {
	if ( cfg.flagCamelcase ) {
		return QString("");
	} else {
		return QString("_");
	}
}

int createOutputFile ( treePrintFormat& cfg ) {
	QFile f( cfg.fileOutPath );
	bool b_r;

	b_r = f.open( QIODevice::WriteOnly | QIODevice::Text );
	if ( !b_r ) return ENOENT;

	f.write( "#include \"fsm.h\"\n" );

	QString s_handler = "#include \"" + QString( cfg.handlerClassName ) + "\"\n\n";
	writeToFile( f, s_handler );

	/// Перечень шагов.
	for ( int l = 0; l < cfg.tree->size(); l++ ) {
		QString	extern_func_fsm;
		extern_func_fsm = "extern const fsmStep< ";
		extern_func_fsm += cfg.className;
		extern_func_fsm += " > ";
		extern_func_fsm += oneCharDown( *cfg.className );
		extern_func_fsm += setBottomUnderline( cfg );
		extern_func_fsm += upOneCharIfCamelcaseUp( cfg.tree->at( l ).funcName, cfg );

		if ( cfg.flagCamelcase ) {
			extern_func_fsm += "FsmStep;\n";
		} else {
			extern_func_fsm += "_fsmStep;\n";
		}


		if ( l == cfg.tree->size() - 1 ) {
			extern_func_fsm += "\n";
		}

		writeToFile( f, extern_func_fsm );
	}

	/// Начинаем описывать сами шаги.
	for ( int l = 0; l < cfg.tree->size(); l++ ) {
		// Если вершина хоть с кем-то связана.
		int vertex_count = cfg.tree->at( l ).vertexConnect->size();
		QString	extern_func_fsm_array;

		uint32_t count_intm_array = 0;			/// Реальное количество элементов в массиве. Включая nullptr.
		if ( vertex_count != 0 ) {
			/// Сортируем вектор переходов, чтобы потом найти в нем пробелы.
			std::sort( cfg.tree->at( l ).vertexConnect->begin(), cfg.tree->at( l ).vertexConnect->end() );
			/// Мы берем не колличество связок, а самый старший номер связки. Потому что может быть одна связка с номером
			/// 12, а 12 (0..11) до нее мы заполним nullptr.
			/// ( vertex_count - 1 ) - потому что у нас колличество элементов (счет с 1). А в векторе считаются с 0.
			/// .number + 1, потому что в переменной хранится номер перехода в логике вектора. Типа 0-й, 1-й.
			/// А нам надо колличество (счет с 1).
			count_intm_array = cfg.tree->at( l ).vertexConnect->at( vertex_count - 1 ).number + 1;

			extern_func_fsm_array = "const fsmStep< ";
			extern_func_fsm_array += cfg.className;
			extern_func_fsm_array += " >* ";
			extern_func_fsm_array += oneCharDown( *cfg.className );
			extern_func_fsm_array += setBottomUnderline( cfg );


			extern_func_fsm_array += upOneCharIfCamelcaseUp( cfg.tree->at( l ).funcName, cfg );

			if ( cfg.flagCamelcase ) {
				extern_func_fsm_array += "FsmStepArray[ ";
			} else {
				extern_func_fsm_array += "_fsm_step_array[ ";
			}


			extern_func_fsm_array += QString::number( count_intm_array );
			extern_func_fsm_array += " ] = {\n";

			/// Описываем все соединения.

			/// Защита на случай, что будет пробел в переходах.
			/// Пример, функция возвращает 0, 2, 3. А 1 - нет. Там должно быть nullptr.
			long int counting_control = -1;

			for ( int i = 0; i < vertex_count; ) {
				counting_control++;
				/// Пробел в векторе переходов.
				if ( counting_control !=  cfg.tree->at( l ).vertexConnect->at( i ).number ) {
					extern_func_fsm_array += "\tnullptr,\n";
					continue;
				}

				extern_func_fsm_array += "\t&";
				extern_func_fsm_array += oneCharDown( *cfg.className );
				extern_func_fsm_array += setBottomUnderline( cfg );
				/// Ищем соответствующему имени шага имя функции.
				QString nm = get_name_func_from_name_step( cfg.tree->at( l ).vertexConnect->at( i ).connect_step, cfg.tree );
				nm = upOneCharIfCamelcaseUp( nm, cfg );

				/// Если вдруг нет такого шага.
				if ( nm.isEmpty() ) {
					QString b_s = "Name func for step <<" + cfg.tree->at( l ).vertexConnect->at( i ).connect_step + ">> does not exist!";
					abort_parsing( b_s, -1 );
				}

				extern_func_fsm_array += nm;

				if ( cfg.flagCamelcase ) {
					extern_func_fsm_array += "FsmStep";
				} else {
					extern_func_fsm_array += "_fsm_step";
				}


				if ( i != vertex_count - 1 ) {
					extern_func_fsm_array += ",\n";
				} else {
					extern_func_fsm_array += "\n";
				}
				i++;
			}

			extern_func_fsm_array += "};\n\n";

			writeToFile( f, extern_func_fsm_array );
		}

		/// Сам элемент графа.
		QString	b_s_vertex;
		b_s_vertex = "const fsmStep< ";
		b_s_vertex += cfg.className;
		b_s_vertex += " > ";
		b_s_vertex += cfg.className;
		b_s_vertex += setBottomUnderline( cfg );
		b_s_vertex += cfg.tree->at( l ).funcName;

		if ( cfg.flagCamelcase ) {
			b_s_vertex += "FsmStep = {\n";
		} else {
			b_s_vertex += "_fsm_step = {\n";
		}

		b_s_vertex += "\t.funcStep\t\t\t\t= ";
		b_s_vertex += oneCharDown( *cfg.className );

		b_s_vertex += "::fsmStepFunc";
		b_s_vertex += setBottomUnderline( cfg );

		b_s_vertex += upOneCharIfCamelcaseUp( cfg.tree->at( l ).funcName, cfg );
		b_s_vertex += ",\n";
		b_s_vertex += "\t.nextStepArray\t\t\t= ";

		if ( vertex_count != 0 ) {
			b_s_vertex += oneCharDown( *cfg.className );
			b_s_vertex += setBottomUnderline( cfg );
			b_s_vertex += upOneCharIfCamelcaseUp( cfg.tree->at( l ).funcName, cfg );

			if ( cfg.flagCamelcase ) {
				b_s_vertex += "FsmStepArray";
			} else {
				b_s_vertex += "_fsm_step_array";
			}

		} else {
			b_s_vertex += "nullptr";
		}

		b_s_vertex += ",\n";
		b_s_vertex += "\t.numberArray\t\t\t= ";
		b_s_vertex += QString::number( count_intm_array );
		b_s_vertex += "\n};\n\n";

		writeToFile( f, b_s_vertex );
	}

	f.close();
	return 0;
}

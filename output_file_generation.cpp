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

static void abortParsing ( QString& message, uint32_t code ) {
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
static QString oneCharUp ( const QString inputString ) {
	QString bufSt = inputString;
	bufSt.replace( 0, 1, QChar( bufSt.at(0).toUpper() ) );
	return bufSt;
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
 * Записывает следующие include:
 * fsm.h
 * Файл с описанием класса.
 */
static void writeHandlers ( QFile& f, const treePrintFormat& cfg ) {
	f.write( "#include \"fsm.h\"\n" );

	QString s_handler = "#include \"" + QString( cfg.handlerClassName ) + "\"\n\n";
	writeToFile( f, s_handler );
}

/*!
 * Записывает перечень шагов.
 * Пример:
 * extern const fsmStep< AyPlayer > ayPlayerSpiFailureHandlerFsmStep;
 */
static void writeExternSteps ( QFile& f, const treePrintFormat& cfg ) {
	for ( int l = 0; l < cfg.tree->size(); l++ ) {
		QString	externFuncFsm;
		externFuncFsm				=	"extern const fsmStep< ";
		externFuncFsm				+=	cfg.className;
		externFuncFsm				+=	" > ";
		externFuncFsm				+=	oneCharDown( *cfg.className );
		externFuncFsm				+=	oneCharUp( cfg.tree->at( l ).funcName );
		externFuncFsm				+=	"FsmStep;\n";

		writeToFile( f, externFuncFsm );
	}

	QString bufN( "\n" );
	writeToFile( f, bufN );
}

/*!
 * Описываем массив указателей переходов.
 * Пример:
 * const fsmStep< AyPlayer >* ayPlayerGuiInitFsmStepArray[ 2 ] = {
 *	&ayPlayerDpInitFsmStep,
 *	&ayPlayerSpiFailureHandlerFsmStep
 * };
 */
static void writeArrayPoints ( QFile& f, const treePrintFormat& cfg, const int step, const int vertexCount, uint32_t& countItemArray ) {
	/// Сортируем вектор переходов, чтобы потом найти в нем пробелы.
	std::sort( cfg.tree->at( step ).vertexConnect->begin(), cfg.tree->at( step ).vertexConnect->end() );
	/// Мы берем не колличество связок, а самый старший номер связки. Потому что может быть одна связка с номером
	/// 12, а 12 (0..11) до нее мы заполним nullptr.
	/// ( vertexCount - 1 ) - потому что у нас колличество элементов (счет с 1). А в векторе считаются с 0.
	/// .number + 1, потому что в переменной хранится номер перехода в логике вектора. Типа 0-й, 1-й.
	/// А нам надо колличество (счет с 1).
	countItemArray = cfg.tree->at( step ).vertexConnect->at( vertexCount - 1 ).number + 1;

	QString	externFuncFsmArray;
	externFuncFsmArray = "const fsmStep< ";
	externFuncFsmArray += cfg.className;
	externFuncFsmArray += " >* ";
	externFuncFsmArray += oneCharDown( *cfg.className );
	externFuncFsmArray += oneCharUp( cfg.tree->at( step ).funcName );
	externFuncFsmArray += "FsmStepArray[ ";
	externFuncFsmArray += QString::number( countItemArray );
	externFuncFsmArray += " ] = {\n";

	/// Описываем все соединения.

	/// Защита на случай, что будет пробел в переходах.
	/// Пример, функция возвращает 0, 2, 3. А 1 - нет. Там должно быть nullptr.
	long int countingControl = -1;

	for ( int i = 0; i < vertexCount; ) {
		countingControl++;
		/// Пробел в векторе переходов.
		if ( countingControl !=  cfg.tree->at( step ).vertexConnect->at( i ).number ) {
			externFuncFsmArray += "\tnullptr,\n";
			continue;
		}

		externFuncFsmArray += "\t&";
		externFuncFsmArray += oneCharDown( *cfg.className );

		/// Ищем соответствующему имени шага имя функции.
		QString nm = get_name_func_from_name_step( cfg.tree->at( step ).vertexConnect->at( i ).connect_step, cfg.tree );
		nm = oneCharUp( nm );

		/// Если вдруг нет такого шага.
		if ( nm.isEmpty() ) {
			QString b_s = "Name func for step <<" + cfg.tree->at( step ).vertexConnect->at( i ).connect_step + ">> does not exist!";
			abortParsing( b_s, -1 );
		}

		externFuncFsmArray += nm;
		externFuncFsmArray += "FsmStep";


		if ( i != vertexCount - 1 ) {
			externFuncFsmArray += ",\n";
		} else {
			externFuncFsmArray += "\n";
		}
		i++;
	}

	externFuncFsmArray += "};\n\n";

	writeToFile( f, externFuncFsmArray );
}

/*!
 * Записываем сами шаги.
 * Пример:
 * const fsmStep< AyPlayer > AyPlayerspiFailureHandlerFsmStep = {
 *	.funcStep				= ayPlayer::fsmStepFuncSpiFailureHandler,
 *	.nextStepArray			= nullptr,
 *	.numberArray			= 0
 * };
 */
static void writeStep ( QFile& f, const treePrintFormat& cfg, const int& stepNumber, const int& vertexCount, const int& countArray ) {
	/// Сам элемент графа.
	QString	vertex;
	vertex = "const fsmStep< ";
	vertex += cfg.className;
	vertex += " > ";
	vertex += oneCharDown( *cfg.className );
	vertex += oneCharUp( cfg.tree->at( stepNumber ).funcName );
	vertex += "FsmStep = {\n";

	vertex += "\t.funcStep\t\t\t\t= ";
	vertex += *cfg.className;
	vertex += "::fsmStepFunc";
	vertex += oneCharUp( cfg.tree->at( stepNumber ).funcName );
	vertex += ",\n";

	vertex += "\t.nextStepArray\t\t\t= ";
	if ( vertexCount != 0 ) {
		vertex += oneCharDown( *cfg.className );
		vertex += oneCharUp( cfg.tree->at( stepNumber ).funcName );
		vertex += "FsmStepArray";
	} else {
		vertex += "nullptr";
	}

	vertex += ",\n";
	vertex += "\t.numberArray\t\t\t= ";
	vertex += QString::number( countArray );
	vertex += "\n};\n\n";

	writeToFile( f, vertex );
}

int createOutputFile ( treePrintFormat& cfg ) {
	QFile f( cfg.fileOutPath );
	bool r;

	r = f.open( QIODevice::WriteOnly | QIODevice::Text );
	if ( !r )
		return ENOENT;

	/// Include файлы.
	writeHandlers( f, cfg );

	/// Перечень extern шагов.
	writeExternSteps( f, cfg );

	/// Начинаем описывать сами шаги.
	for ( int stepNumber = 0; stepNumber < cfg.tree->size(); stepNumber++ ) {
		// Если вершина хоть с кем-то связана, то надо вывести массив соединений.
		int vertexCount = cfg.tree->at( stepNumber ).vertexConnect->size();
		uint32_t countItemArray = 0;			/// Реальное количество элементов в массиве. Включая nullptr.
		if ( vertexCount != 0 ) {
			writeArrayPoints( f, cfg, stepNumber, vertexCount, countItemArray );
		}

		writeStep( f, cfg, stepNumber, vertexCount, countItemArray );
	}

	f.close();
	return 0;
}

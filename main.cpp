#include "main.h"
#include <iostream>

using namespace std;

/*!
 * Требуется, чтобы был указан входящий файл и исходящий.
 * Все остальные параметры берутся из файла.
 *
 * \param[in]	countArg	-	количество переданных в функцию параметров.
 *
 * \return		{	true	-	количество пришедших аргументов соответствует ожиданиям.
 *					false	-	в противном случае.	}
 */
bool checkCountInputArgument ( const int& countArg ) {
	if ( countArg != 3 ) {
		cout << "Wrong number of arguments!"		<< endl;
		cout << "It takes 2 arguments:"				<< endl;
		cout << "1. Input file (path/name.pu)."		<< endl;
		cout << "2. Output file (path/name.cpp)."	<< endl;
		return false;
	}
	return true;
}

int main ( int argc, char* argv[] ) {
	/// Проверяем количество пришедших аргументов.
	if ( checkCountInputArgument( argc ) == false )
		return 0;

	/// Формат вывода дерева. Заполняется согласно конфига в файле.
	treePrintFormat					printCfg;

	/// Полные пути до входного и выходного файла с расширениями.
	char* pathFileIn				=	argv[ 1 ];
	printCfg.fileOutPath			=	argv[ 2 ];

	/// Имя класса, к которому будет относится FSM
	/// дерево должно находится в заголовке файла.

	/// Имя класса, к которому будет относится FSM дерево.
	char* fsmClassName;
	fsmClassName = getFsmClassArg( pathFileIn, "CLASS_NAME" );
	if ( fsmClassName == nullptr ) {
		cout << "Parametr <<CLASS_NAME>> not finded!"		<< endl;
		return 0;
	}
	printCfg.className = new QString( fsmClassName );
	delete[] fsmClassName;


	/// .h файл с описанием класса.
	char*	handlerClass;
	handlerClass = getFsmClassArg( pathFileIn, "FILE_WITH_CLASS_DESCRIPTION" );
	if ( handlerClass == nullptr ) {
		cout << "Parametr <<FILE_WITH_CLASS_DESCRIPTION>> not finded!"		<< endl;
		return 0;
	}
	printCfg.handlerClassName = handlerClass;

	QString enter_step_name;
	enter_step_name = searchEntryPointStepName( pathFileIn );

	printCfg.tree = new QVector< vertex_struct >;

	searchFuncNameAndStepName( pathFileIn, printCfg.tree );
	searchConnectStep( pathFileIn, printCfg.tree );
	createOutputFile( printCfg );
}

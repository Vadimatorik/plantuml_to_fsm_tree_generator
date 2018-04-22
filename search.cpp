#include "search.h"

char* getFsmClassArg ( const char* const pathInputFile, QString parameter ) {
	char*	returnArg = nullptr;

	QFile f( pathInputFile );
	bool r;

	r = f.open( QIODevice::ReadOnly | QIODevice::Text );
	if ( !r ) return nullptr;

	QString	b;
	QRegularExpression reIn( REG_EXP_TEMPLATE_PARAM_AND_ARG );
	QRegularExpression inParamRemoveLeft( REG_EXP_TEMPLATE_PARAM_REMOVE_LEFT );
	QRegularExpression inParamRemoveRight( REG_EXP_TEMPLATE_PARAM_REMOVE_RIGHT );
	QRegularExpression inArgRemoveLeft( REG_EXP_TEMPLATE_ARG_REMOVE_LEFT );
	QRegularExpression inArgRemoveRight( REG_EXP_TEMPLATE_ARG_REMOVE_RIGHT );

	/// Пытаемся найти строку под шаблон.
	while ( !f.atEnd() ) {
		b = f.readLine();
		r = b.contains( reIn );

		if ( !r )
			continue;

		/// Смотрим на параметр.
		QString stringFileParam = b;
		stringFileParam = stringFileParam.remove( inParamRemoveLeft );
		stringFileParam = stringFileParam.remove( inParamRemoveRight );

		if ( stringFileParam != parameter )
			continue;

		QString arg = b;
		arg = arg.remove( inArgRemoveLeft );
		arg = arg.remove( inArgRemoveRight );

		uint32_t countChar = arg.length();

		/// +1 - нуль-терминатор.
		returnArg = new char[ countChar + 1 ];
		qstrncpy( returnArg, qPrintable( arg ), countChar + 1 );
		break;
	}

	f.close();
	return returnArg;
}


QString searchEntryPointStepName ( char* file_path ) {
	QString entry_point_step_name;
	QFile f( file_path );
	bool b_r;

	b_r = f.open(QIODevice::ReadOnly | QIODevice::Text);
	if ( !b_r ) return entry_point_step_name;

	QString	b;												/// Строка-буффер.
	QRegularExpression re_in( REG_EXP_TEMPLATE_ENTRY_POINT );
	QRegularExpression re_in_remove_left( REG_EXP_TEMPLATE_ENTRY_POINT_REMOVE_LEFT );
	QRegularExpression re_in_remove_right( REG_EXP_TEMPLATE_ENTRY_POINT_REMOVE_RIGHT );

	/// Находим имя шага.
	while ( !f.atEnd() ) {
		b = f.readLine();
		b_r = b.contains( re_in );
		if ( !b_r ) continue;
		b = b.remove( re_in_remove_left );
		b = b.remove( re_in_remove_right );
		entry_point_step_name = b;
		break;
	}

	f.close();
	return entry_point_step_name;
}

int searchFuncNameAndStepName ( char* file_path, QVector< vertex_struct >* tree ) {
	QString func_name;
	QFile f( file_path );
	bool b_r;

	b_r = f.open(QIODevice::ReadOnly | QIODevice::Text);
	if ( !b_r ) return ENOENT;

	QString	b;												/// Строка-буффер.
	QRegularExpression re( REG_EXP_TEMPLATE_SEARCH_FUNC_NAME );

	QRegularExpression re_name_step_left( REG_EXP_TEMPLATE_STEP_NAME_GET_LEFT  );
	QRegularExpression re_name_step_right( REG_EXP_TEMPLATE_STEP_NAME_GET_RIGHT );

	QRegularExpression re_name_f_left( REG_EXP_TEMPLATE_FUNC_NAME_GET_LEFT );
	QRegularExpression re_name_f_right( REG_EXP_TEMPLATE_FUNC_NAME_GET_RIGHT );

	/// Находим имя шага.
	while ( !f.atEnd() ) {
		b = f.readLine();
		b_r = b.contains( re );
		if ( !b_r ) continue;
		// Мы нашли подходящую под шаблон строку, теперь надо выдернуть название шага и имя функции.
		QString b_f_name = b;
		QString b_step_name = b;
		b_f_name.remove( re_name_f_left );
		b_f_name.remove( re_name_f_right );

		b_step_name.remove( re_name_step_left );
		b_step_name.remove( re_name_step_right );

		// Сохраняем выделенное в дерево.
		vertex_struct		b_vs;
		b_vs.funcName		= b_f_name;
		b_vs.stepName		= b_step_name;
		b_vs.vertexConnect	= new QVector< vertex_connect_struct >;

		tree->append( b_vs );
	}

	f.close();
	return 0;
}

int searchConnectStep ( char* file_path, QVector< vertex_struct >* tree ) {
	QString func_name;
	QFile f( file_path );
	bool b_r;

	b_r = f.open(QIODevice::ReadOnly | QIODevice::Text);
	if ( !b_r ) return ENOENT;

	QString	b;												/// Строка-буффер.
	QRegularExpression re( REG_EXP_TEMPLATE_SEARCH_CONNECT );

	QRegularExpression re_step_in( REG_EXP_TEMPLATE_STEPIN_NAME_GET_RIGHT  );

	QRegularExpression re_step_out_left( REG_EXP_TEMPLATE_STEPOUT_NAME_GET_LEFT );
	QRegularExpression re_step_out_right( REG_EXP_TEMPLATE_STEPOUT_NAME_GET_RIGHT );

	QRegularExpression re_number_left( REG_EXP_TEMPLATE_NUMBER_GET_LEFT );
	QRegularExpression re_number_right( REG_EXP_TEMPLATE_NUMBER_GET_RIGHT );

	/// Находим имя шага.
	while ( !f.atEnd() ) {
		b = f.readLine();
		b_r = b.contains( re );
		if ( !b_r ) continue;

		// Мы нашли подходящую под шаблон строку, теперь надо выдернуть:
		// Имя шага входа.
		// Имя к кому конектиться.
		// Номер коннекта.
		QString b_in = b;
		QString b_out = b;
		QString b_number = b;

		b_in.remove( re_step_in );

		b_out.remove( re_step_out_left );
		b_out.remove( re_step_out_right );

		b_number.remove( re_number_left );
		b_number.remove( re_number_right );

		// Ищем, какой вершине пренадлежит этот коннект.
		for ( int l = 0; l < tree->size(); l++ ) {
			if ( tree->at( l ).stepName != b_in ) continue;		// Ищем вершину, которую будем к чему-то подключать.

			int b_connect_number = b_number.toInt();

			vertex_connect_struct	b_vcs;
			b_vcs.connect_step		= b_out;
			b_vcs.number			= b_connect_number;

			tree->at( l ).vertexConnect->append( b_vcs );
			break;
		}
	}

	f.close();
	return 0;
}

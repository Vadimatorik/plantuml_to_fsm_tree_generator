#include "search.h"

QString search_entry_point_step_name ( char* file_path ) {
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

int search_func_name_and_step_name ( char* file_path, QVector< vertex_struct >* tree ) {
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
		b_vs.func_name		= b_f_name;
		b_vs.step_name		= b_step_name;
		b_vs.vertex_connect	= new QVector< vertex_connect_struct >;

		tree->append( b_vs );
	}

	f.close();
	return 0;
}

int search_connect_step ( char* file_path, QVector< vertex_struct >* tree ) {
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
			if ( tree->at( l ).step_name != b_in ) continue;		// Ищем вершину, которую будем к чему-то подключать.

			int b_connect_number = b_number.toInt();

			vertex_connect_struct	b_vcs;
			b_vcs.connect_step		= b_out;
			b_vcs.number			= b_connect_number;

			tree->at( l ).vertex_connect->append( b_vcs );
			break;
		}
	}

	f.close();
	return 0;
}

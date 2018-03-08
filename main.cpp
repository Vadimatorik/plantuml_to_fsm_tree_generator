#include "main.h"

/*!
 * Ищем строку
 */
#define REG_EXP_TEMPLATE_ENTRY_POINT							"^\\[\\*]\\s*-->\\s*\\w*\\s*\n$"
#define REG_EXP_TEMPLATE_ENTRY_POINT_REMOVE_LEFT				"^\\[\\*]\\s*-->\\s*"
#define REG_EXP_TEMPLATE_ENTRY_POINT_REMOVE_RIGHT									   "\\s*\n$"

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

/*!
 * Шаг, с которым соденяется вершина графа.
 */
struct vertex_connect_struct {
	QString			connect_step;						/// Имя шага.
	uint32_t		number;								/// При каком возвращаемом значении
														/// метода вершины графа происходит
														/// переход на этот шаг.
};

/*!
 * Описание вершины графа.
 */
struct vertex_struct {
	QString								step_name;			/// Имя шага.
	QString								func_name;			/// Имя функции, вызываемой в вершине.
	QVector< vertex_connect_struct >*	vertex_connect;		/// С какими вершинами соединен.
};

#define REG_EXP_TEMPLATE_SEARCH_FUNC_NAME					"^state\\s*\"\\w*\"\\s*as\\s*\\w*\\s*{\\s*\n$"

#define REG_EXP_TEMPLATE_STEP_NAME_GET_LEFT					"^state\\s*\"\\w*\"\\s*as\\s*"
#define REG_EXP_TEMPLATE_STEP_NAME_GET_RIGHT												"\\s*{\\s*\n$"

#define REG_EXP_TEMPLATE_FUNC_NAME_GET_LEFT					"^state\\s*\""
#define REG_EXP_TEMPLATE_FUNC_NAME_GET_RIGHT								"\"\\s*as\\s*\\w*\\s*{\\s*\n$"

/*!
 * Метод производит поиск имени функции в файле,
 * соответствующую имени шага.
 * \param[in]	file_path - путь к файлу с plantuml.
 * \param[in]	step_name - имя шага.
 *  \return		Строка с именем функции, соответствующей
 *				имени шага или пустая строка, если таковая не была
 *				найдена.
 */
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


#define REG_EXP_TEMPLATE_SEARCH_CONNECT					"^\\w*\\s*-->\\s*\\w*\\s*:\\s*\\d*\\s*\n$"

#define REG_EXP_TEMPLATE_STEPIN_NAME_GET_RIGHT				 "\\s*-->\\s*\\w*\\s*:\\s*\\d*\\s*\n$"

#define REG_EXP_TEMPLATE_STEPOUT_NAME_GET_LEFT			"^\\w*\\s*-->\\s*"
#define REG_EXP_TEMPLATE_STEPOUT_NAME_GET_RIGHT								"\\s*:\\s*\\d*\\s*\n$"

#define REG_EXP_TEMPLATE_NUMBER_GET_LEFT				"^\\w*\\s*-->\\s*\\w*\\s*:\\s*"
#define REG_EXP_TEMPLATE_NUMBER_GET_RIGHT												 "\\s*\n$"

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


QString get_name_func_from_name_step ( QString name_step, QVector< vertex_struct >* tree ) {
	for ( int l = 0; l < tree->size(); l++ ) {
		if ( tree->at( l ).step_name == name_step ) {
			return tree->at( l ).func_name;
		}
	}
	return "";
}

int create_output_file ( char* file_path, QString obj_class, QVector< vertex_struct >* tree ) {
	QFile f( file_path );
	bool b_r;

	b_r = f.open( QIODevice::WriteOnly | QIODevice::Text );
	if ( !b_r ) return ENOENT;

	f.write( "#include \"fsm.h\"\n\n" );

	/// Перечень шагов.
	for ( int l = 0; l < tree->size(); l++ ) {
		QString	extern_func_fsm;
		extern_func_fsm = "extern const fsm_step< ";
		extern_func_fsm += obj_class;
		extern_func_fsm += " > ";
		extern_func_fsm += obj_class;
		extern_func_fsm += "_";
		extern_func_fsm += tree->at( l ).func_name;
		extern_func_fsm += "_fsm_step;\n";

		if ( l == tree->size() - 1 ) {
			extern_func_fsm += "\n";
		}

		QByteArray b_byte_string = extern_func_fsm.toUtf8();
		char* b_write = b_byte_string.data();

		f.write( b_write );
	}

	/// Начинаем описывать сами шаги.
	for ( int l = 0; l < tree->size(); l++ ) {
		// Если вершина хоть с кем-то связана.
		int vertex_count = tree->at( l ).vertex_connect->size();
		QString	extern_func_fsm_array;

		if ( vertex_count != 0 ) {
			extern_func_fsm_array = "const fsm_step< ";
			extern_func_fsm_array += obj_class;
			extern_func_fsm_array += " >* ";
			extern_func_fsm_array += obj_class;
			extern_func_fsm_array += "_";
			extern_func_fsm_array += tree->at( l ).func_name;
			extern_func_fsm_array += "_fsm_step_array[ ";
			extern_func_fsm_array += QString::number( vertex_count );
			extern_func_fsm_array += " ] = {\n";

			/// Описываем все соединения.
			for ( int i = 0; i < vertex_count; i++ ) {
				extern_func_fsm_array += "\t&";

				extern_func_fsm_array += obj_class;
				extern_func_fsm_array += "_";
				/// Ищем соответствующему имени шага имя функции.
				extern_func_fsm_array += get_name_func_from_name_step( tree->at( l ).vertex_connect->at( i ).connect_step, tree );
				extern_func_fsm_array += "_fsm_step;";

				if ( i != vertex_count - 1 ) {
					extern_func_fsm_array += ",\n";
				} else {
					extern_func_fsm_array += "\n";
				}
			}

			extern_func_fsm_array += "};\n\n";

			QByteArray b_byte_string = extern_func_fsm_array.toUtf8();
			char* b_write = b_byte_string.data();

			f.write( b_write );
		}

		/// Сам элемент графа.
		QString	b_s_vertex;
		b_s_vertex = "const fsm_step< ";
		b_s_vertex += obj_class;
		b_s_vertex += " > ";
		b_s_vertex += obj_class;
		b_s_vertex += "_";
		b_s_vertex += tree->at( l ).func_name;
		b_s_vertex += "_fsm_step = {\n";

		b_s_vertex += "\t.func_step\t\t\t\t= ";
		b_s_vertex += obj_class;
		b_s_vertex += "::fsm_step_func_";
		b_s_vertex += tree->at( l ).func_name;
		b_s_vertex += ",\n";
		b_s_vertex += "\t.next_step_array\t\t= ";

		if ( vertex_count != 0 ) {
			b_s_vertex += obj_class;
			b_s_vertex += "_";
			b_s_vertex += tree->at( l ).func_name;
			b_s_vertex += "_fsm_step_array";
		} else {
			b_s_vertex += "nullptr";
		}

		b_s_vertex += ",\n";
		b_s_vertex += "\t.number_array\t\t\t= ";
		b_s_vertex += QString::number( vertex_count );
		b_s_vertex += "\n};\n\n";


		QByteArray b_byte_string = b_s_vertex.toUtf8();
		char* b_write = b_byte_string.data();
		f.write( b_write );
	}


	f.close();
	return 0;
}

int main ( int argc, char* argv[] ) {
	char* f_in;
	char* f_out;
	char* fsm_class_name;

	f_in					= argv[ 1 ];
	f_out					= argv[ 2 ];
	fsm_class_name			= argv[ 3 ];
	QString enter_step_name;
	enter_step_name = search_entry_point_step_name( f_in );

	QVector< vertex_struct >* tree;
	tree = new QVector< vertex_struct >;
	search_func_name_and_step_name( f_in, tree );
	search_connect_step( f_in, tree );
	create_output_file( f_out, fsm_class_name, tree );
}

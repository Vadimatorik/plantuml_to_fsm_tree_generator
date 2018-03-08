#include "output_file_generation.h"

static QString get_name_func_from_name_step ( QString name_step, QVector< vertex_struct >* tree ) {
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

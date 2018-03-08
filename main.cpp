#include "main.h"

int main ( int argc, char* argv[] ) {
	(void)argc;

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

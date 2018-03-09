#include "main.h"
#include <iostream>

using namespace std;

int main ( int argc, char* argv[] ) {
	(void)argc;

	char* f_in;
	char* f_out;
	char* fsm_class_name;
	char* handler_class;

	f_in					= argv[ 1 ];
	f_out					= argv[ 2 ];
	fsm_class_name			= argv[ 3 ];
	handler_class			= argv[ 4 ];

	cout << '\t' << "Input file: " << "\t\t" << f_in << endl;
	cout << '\t' << "Output file: " << "\t\t" << f_out << endl;
	cout << '\t' << "FSM class name: " << "\t" << fsm_class_name << endl;
	cout << '\t' << "Handler class name:" << "\t" << handler_class << endl;

	QString enter_step_name;
	enter_step_name = search_entry_point_step_name( f_in );

	QVector< vertex_struct >* tree;
	tree = new QVector< vertex_struct >;

	search_func_name_and_step_name( f_in, tree );
	search_connect_step( f_in, tree );
	create_output_file( f_out, fsm_class_name, tree, handler_class );
}

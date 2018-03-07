#include "main.h"

QString search_entry_point ( char* file_path ) {
	QString entry_point_func_name;
	QFile f( file_path );
	bool r;
	r = f.open(QIODevice::ReadOnly | QIODevice::Text);
	if ( !r ) return entry_point_func_name;					/// Точка входа отсутствует.
	QString b;												/// Строка-буффер.
	while ( !f.atEnd() ) {
		b = f.readLine();

	}
}

int main ( int argc, char* argv[] ) {
	std::cout << "Hello";
}

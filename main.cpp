#include "main.h"

/*!
 * Ищем строку
 */
#define REG_EXP_TEMPLATE_ENTRY_POINT				"^\\[\\*]\\s*-->\\s*\\w*\\s*\n$"

QString search_entry_point ( char* file_path ) {
	QString entry_point_func_name;
	QFile f( file_path );
	bool b_r;
	int i_r;

	b_r = f.open(QIODevice::ReadOnly | QIODevice::Text);
	if ( !b_r ) return entry_point_func_name;				/// Точка входа отсутствует.

	QString	b;												/// Строка-буффер.
	QRegularExpression re_in( REG_EXP_TEMPLATE_ENTRY_POINT );
	while ( !f.atEnd() ) {
		b = f.readLine();
		b_r = b.contains( re_in );
		if ( !b_r ) continue;
	}
}

int main ( int argc, char* argv[] ) {
	search_entry_point( argv[ 1 ] );
}

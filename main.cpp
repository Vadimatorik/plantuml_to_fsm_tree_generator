#include "main.h"

/*!
 * Ищем строку
 */
#define REG_EXP_TEMPLATE_ENTRY_POINT				"[[][*]]\\s-->\\s[^\\s]*"

QString search_entry_point ( char* file_path ) {
	QString entry_point_func_name;
	QFile f( file_path );
	bool b_r;
	int i_r;

	b_r = f.open(QIODevice::ReadOnly | QIODevice::Text);
	if ( !b_r ) return entry_point_func_name;				/// Точка входа отсутствует.

	QString	b;												/// Строка-буффер.
	QRegExp	re_in( REG_EXP_TEMPLATE_ENTRY_POINT );
	while ( !f.atEnd() ) {
		b = f.readLine();
		i_r = re_in.indexIn( b );
		if ( i_r == -1 ) continue;
	}
}

int main ( int argc, char* argv[] ) {
	search_entry_point( argv[ 1 ] );
}

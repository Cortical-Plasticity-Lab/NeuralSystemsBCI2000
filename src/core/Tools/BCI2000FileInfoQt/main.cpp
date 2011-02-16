#include <QtGui/QApplication>
#include "bci2000fileinfo.h"

#ifdef _WIN32
# include <Windows.h>

int main( int, char*[] );

int WINAPI
WinMain( HINSTANCE, HINSTANCE, LPSTR, int )
{
  return main( __argc, __argv );
}
#endif // _WIN32

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  a.setOrganizationName( "BCI2000" );
  a.setOrganizationDomain( "bci2000.org" );
  a.setApplicationName( "BCI2000FileInfo" );
  BCI2000FileInfo w;
  w.show();
  return a.exec();
}
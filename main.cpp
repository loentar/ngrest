#pragma hdrstop
#pragma argsused

#ifdef _WIN32
#include <tchar.h>
#else
  typedef char _TCHAR;
  #define _tmain main
#endif

#include <stdio.h>
#include <iostream>
#include <core/common/src/HttpException.h>

#include "pqConnect.hpp"

using namespace std;


 int _tmain(int argc, _TCHAR* argv[])
{
	//printf("<!DOCTYPE html>\n");
	printf("Content-Type: text/html; charset=utf-8\n\n") ;
	printf("<html>\n") ;
	printf("<head>\n") ;
	printf("<title>Conectando Postgress con cgi c++</title>\n") ;
	printf("</head> \n");
	pqConnect pq( "localhost", "5432", "apaaa","postgres", "1234" ) ;
	pq.Show() ;
	pq.Disconnect() ;
	printf("<body>\n") ;
	printf("</body>\n") ;
	printf("</html>\n") ;

	system("pause");
	return 0;
}

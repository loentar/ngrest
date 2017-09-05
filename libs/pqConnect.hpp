//
//  pqConnect.hpp
//  Practica Profecional
//
//  Created by Leonardo Casales on 26/8/17.
//  Copyright © 2017 Leo. All rights reserved.
//

#ifndef pqConnect_hpp
#define pqConnect_hpp

#include <stdio.h>
#include <libpq-fe.h>
//#include <libpq.lib>

class pqConnect {
private:
    char* host ;
    char* dataBase ;
    char* port ;
    char* user ;
    char* passwd ;
	PGconn *cnn ;
    PGresult *result ;

public:
    pqConnect () ;
    pqConnect ( char* host, char* port, char* dataBase, char* user, char* passwd ) ;
    bool Connect() ;
    void Disconnect() ;
    int Show() ;
};
#endif /* pqConnect_hpp */

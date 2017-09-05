//
//  pqConnect.cpp
//  Practica Profecional
//
//  Created by Leonardo Casales on 26/8/17.
//  Copyright © 2017 Leo. All rights reserved.
//

#include "pqConnect.hpp"

pqConnect::pqConnect() {
    PGconn *cnn = NULL;
    PGresult *result = NULL;
}

pqConnect::pqConnect ( char* Host, char* Port, char* DataBase, char* User, char* Passwd ) {
    PGconn *cnn = NULL;
    PGresult *result = NULL;
    host = Host ;
    dataBase = DataBase ;
    port = Port ;
    user = User ;
    passwd = Passwd ;
}

int pqConnect::Show() {

    int i;

    cnn = PQsetdbLogin(host,port,NULL,NULL,dataBase,user,passwd);
    if (PQstatus(cnn) != CONNECTION_BAD) {
        printf( "Estamos conectados a PostgreSQL!<br>\n" ) ;
        result = PQexec(cnn, "SELECT * FROM users");//result = PQexec(cnn, "SELECT * FROM test");

        if (result != NULL) {
            int tuplas = PQntuples(result);
            int campos = PQnfields(result);
            printf( "No. Filas: %i<br>\n", tuplas ) ;
            printf( "No. Campos:%i<br>\n", campos ) ;

            printf( "Los nombres de los campos son:<br>\n" ) ;

            for (i=0; i<campos; i++) {
                printf( "%s | ", PQfname(result,i) ) ;
            }

            printf( "Contenido de la tabla<br>\n" ) ;

            for (i=0; i<tuplas; i++) {
                for (int j=0; j<campos; j++) {
                    printf( "%s | ",PQgetvalue(result,i,j) );
                }
                printf ( "<br>\n" ) ;
            }
        }
        return true ;
        // Ahora nos toca liberar la memoria
        //PQclear(result);

    } else {
        printf( "Error de conexion\n" );
        PQfinish(cnn);
        return false ;
    }

}

void pqConnect::Disconnect() {
    PQclear(result);
    PQfinish(cnn);
}

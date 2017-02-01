/* Projekt 1: Klient na komunikaciu so serverom pre VUT FIT predmet IPK
 * Autor: Matej Marusak, xmarus06@fit.vutbr.cz
 * 2015/2016
 */


#include <iostream>
#include <regex>//Ou Jes
#include <string>
#include <cstdlib>
#include <cstring>
#include <netdb.h> //hostent, gethostbyname
#include <sys/types.h>//socket
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <fstream>
#include <unistd.h>
#include "RW.hh"

#include <stdio.h>

//vytlaci error na stderr a ukonci program s chybovym kodom
void error(std::string err_message, unsigned err_code){
    std::cerr<<err_message<<std::endl;
    exit(err_code);
}

// Structure of parsed arguments
struct WhatToDo{
    const char *host;
    int port;
    bool download;//true download, false upload
    const char *fileName;
};

void Comunicate(WhatToDo x){

    hostent *H = gethostbyname(x.host);
    if (!H)
        error("Neviem prelozit adresu",3);
    //vytvorime socket
    int socket_1;
    if ((socket_1 = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
        error("Nejde urobit socket!",6);
    sockaddr_in anotherEnd;
    anotherEnd.sin_family = AF_INET;
    anotherEnd.sin_port = htons(x.port);
    memcpy(&(anotherEnd.sin_addr), H->h_addr, H->h_length);

    //CONNECT
    if (connect( socket_1,( sockaddr *)&anotherEnd, sizeof(anotherEnd)) == -1)
        error("Neviem naviazat spojenie",7);

    int sent;
    std::string text;
    RW data;
    int recieved;
    char buf[256];

    //RECIEVE
    if (x.download){
        text = "GET\n"+std::string(x.fileName)+"\n\r";
        if ((sent = send(socket_1, text.c_str(), text.size() + 1, 0)) == -1)
            error("Neviem poslat udaje",8);
        while ((recieved = recv(socket_1, buf, 255, 0)) != 0)
            data.addNext(buf, recieved);
        std::string head = data.splitHead();
        if (head.compare("OK"))
            error("Server nevie poskytnut data", 5);
        if (data.writeToFile(x.fileName))
            error("Neviem ulozit",4);
    }

    //SEND
    else{
        text = "POST\n"+std::string(x.fileName)+"\n\r";
        if ((sent = send(socket_1, text.c_str(), text.size() + 1, 0)) == -1)
            error("Neviem poslat udaje",8);

        int l = 255;
        if (data.addInput(x.fileName))
            error("Neda sa otvorit subor", 4);
        std::string buff;
        int r = recv(socket_1, buf, 255, 0);
        if (r < 0 || strncmp(buf, "OK\n", 3))
            error("Server nechce nic", 5);
        r = l;
        while (r == l){
            r = data.readNext(l, buff);
            send(socket_1, buff.c_str(), r,0);
        }
        //t

    }
    close(socket_1);
}

char *getOption(char *argv[], int len, std::string opt){
    char **n = std::find(argv, argv+len, opt);
    if (n == argv+len || ++n == argv+len)
        return NULL;
    return *n;
}



int main(int argc, char *argv[]){
    if (argc != 7)
        error("Nespravny pocet argumentov",1);
    char *h = NULL;
    char *p = NULL;
    char *v = NULL;
    WhatToDo x;
    if (! (h = getOption(argv, argc, "-h")))
        error("Nezadane -h",1);
    if (! (p=getOption(argv, argc, "-p")))
        error("Nezadane -p",1);
    if (! (v=getOption(argv, argc, "-d")))
        if (!(v = getOption(argv, argc, "-u")))
            error("Spatna volba",1);
        else
            x = {h, std::atoi(p), false, v};
    else
        x = {h, std::atoi(p), true, v};

    Comunicate(x);

    return 0;
}

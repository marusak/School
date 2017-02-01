/* Projekt 1: Server na komunikaciu s klientom pre VUT FIT predmet IPK
 * Autor: Matej Marusak, xmarus06@fit.vutbr.cz
 * 2015/2016
 */

#include <iostream>
#include <string>
#include <cstring>
#include <algorithm>
#include <iterator>
#include <fstream>

#include "RW.hh"

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>


// VYtlaci chybovy kod a skonci program
void error(std::string msg, int errCode){
  std::cout<<msg<<std::endl<<std::flush;
  exit(errCode);
}

// Zabezpecuje komunikaciu s klientom (berie cislo socketu, na ktorom je pripravena komunikacia)
void letsComunicate(int sock){
    char buffer[256];
    int n;
    if ((n = recv(sock, buffer, 255, 0) < 0))
        error("Chyba pri citani zo socketu",9);
    //rozprasujeme si prvu spravu
    std::string message = buffer;
    size_t erste = message.find("\n");
    size_t second = message.find("\n\r");
    std::string what = message.substr(0, erste);
    std::string fileName = message.substr(erste+1, second-erste-1);
    std::string text;
    RW data;


    //RECIEVE DATA
    if (!what.compare("POST")){
        n = write(sock, "OK\n", 3);
        while ((n = recv(sock, buffer, 255, 0)) != 0)
            data.addNext(buffer, n);
        if (data.writeToFile(fileName))
            n = send(sock, "ERR\nCannot save file\n\r",22,0);
        else
            n = send(sock, "OK\n",3,0);

    }

    //SEND DATA
    else if (!what.compare("GET")){
        int l = 255;
        if (data.addInput(fileName)){
            n = write(sock, "FAIL\nCannot Open File\n\r", 23);
            return;
        }
        std::string buff;
        int r = data.readNext(l-3, buff);
        n = write(sock, ("OK\n"+buff).c_str(), r+3);
        r = l;
        while (r == l){
            r = data.readNext(l, buff);
            n = write(sock, buff.c_str(), r);
        }
    }

    //DO NOT UNDERSTAND
    else {
        n = write(sock, "ERR\nDont understand\n\r", 21);
    }
}

int main(int argc, char *argv[]){
  //parse arguments
  if (argc != 3)
    error("Nespravny pocet arguemntov", 44);

    if ((std::strcmp(argv[1], "-p")))
        error("Nespravna volba", 44);
    int port = std::stoi(argv[2]);

  int sock;
  int sock1;
  socklen_t len; //length of adress of the client
  struct sockaddr_in serv_addr, cli_addr;
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0)
    error("Nepodarilo sa otvorit socket",4);
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port);
  serv_addr.sin_addr.s_addr=INADDR_ANY;
  if (bind(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    error("Chyba pri bind",4);
  listen(sock, 5);
  len = sizeof(cli_addr);

  while(true){
    sock1 = accept(sock, (struct sockaddr *) &cli_addr, &len);
    if (sock < 0)
      error("chyba pri akcepte",4);
    int pid = fork();
    if (pid < 0)
      error("Chyba pri robeni vidlicky",4);
    if (pid == 0){
      close(sock);
      letsComunicate(sock1);
      exit(0);
      }
      else
        close(sock1);
    }
    close(sock);
    return 0;
  }

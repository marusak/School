#include <iostream>
#include <string>

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

void error(std::string msg, int errCode){
  std::cout<<msg<<std::endl;
  exit(errCode);
}

int main(int argc, char *argv[]){
  //TODO parse arguments
  int sock;
  int sock1;
  int port; //port number
  int len; //length of adress of the client
  int  n; //help variable for checking correct return values
  char buffer[256];
  struct sockaddr_in serv_addr, cli_addr;
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0)
    error("Nepodarilo sa otvorit socket",4);
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(5150);
  serv_addr.sin_addr.s_addr=INADDR_ANY;
  if (bind(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    error("Chyba pri bind",4);
  listen(sock, 5);
  len = sizeof(cli_addr);
  sock1 = accept(sock, (struct sockaddr *) &cli_addr, &len);
  if (sock < 0)
    error("chyba pri akcepte",4);
  n = read(sock1, buffer, 255);
  if (n < 0)
    error("chyba pri citani zo socketu",4);
  n = write(sock1, "Hello world", 12);

  return 0;
}

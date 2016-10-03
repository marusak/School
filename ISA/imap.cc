#include <iostream>
#include <string>

#include <string.h>
#include <stdlib.h>

#include <netdb.h> //hostent, gethostbyname
#include <sys/socket.h>
#include <sys/types.h>

#include "imap.hh"


/* Print error to stderr and exit. */
void IMAP::error(std::string err_msg, unsigned err_code){
    this->err_code = err_code;
    this->err_msg = err_msg;
    std::cerr<<err_msg;
    //exit (err_code);
}

/* Return true if error happened in the last action. */
bool IMAP::error_happened(){
    if (err_code != 0)
        return true;
    else
        return false;
}

void IMAP::clear_error(){
    err_code = 0;
    err_msg = "";
}

/*Constructor:
 * set error code to 0
 * establish connection to server
 */
IMAP::IMAP(std::string host, int port){
    err_code = 0;
    message_id = 0;

    hostent *H = gethostbyname(host.c_str());
    if (!H)
        error("Can not get host by name", 2);

    //Create socket
    int sock;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        error("Can not create socket", 3);

    //Create sockaddr_in (socket on server)
    sockaddr_in server_sock;
    server_sock.sin_family = AF_INET;
    server_sock.sin_port = htons(port);
    memcpy(&(server_sock.sin_addr), H->h_addr, H->h_length);

    //Connect
    if (connect(sock,( sockaddr *)&server_sock, sizeof(server_sock)) == -1)
        error("Cannot connect to server", 4);

    connection_sock = sock;
}

/*Login to the server*/
bool IMAP::login(std::string login, std::string password){
    clear_error();
    communicate("LOGIN " + login + " "+password);
    return error_happened();
}

bool IMAP::message_ended(std::string msg, std::string id){
    std::string last_line;
    std::size_t last_line_b = msg.find_last_of("\n", msg.size() - 2);
    if (last_line_b == std::string::npos)
        last_line = msg;
    else
         last_line = msg.substr(last_line_b + 1);
    if (last_line[0] == 'A')
        if (!strncmp(last_line.c_str(), id.c_str(), id.size()))
            return true;
    return false;
}


std::string IMAP::communicate(std::string message){
    std::string msg_id;
    msg_id = "A"+std::to_string(message_id++);
    std::string msg = msg_id + " " + message+'\n';
    if (send(connection_sock, msg.c_str(), msg.size(), 0) == -1)
        error("Cannot sent a message",5);

    char buf[1000];
    std::string answer{};
    int recieved;
    while ((recieved = recv(connection_sock, buf, 1000, 0))){
        answer.append(buf, recieved);
        if (message_ended(answer, msg_id) && recieved < 1000)
            break;
    }
    std::size_t last_line_b = answer.find_last_of("\n", answer.size() - 2);
    std::string command_completed = answer.substr(last_line_b + 1);
    std::string final_answer = answer.substr(0, last_line_b - 1);
    std::size_t start_pos = msg_id.size() + 2;
    std::size_t found = command_completed.find("NO");
    if (found == start_pos)
        error(answer, 6);
    else {
        found = command_completed.find("BAD");
        if (found == start_pos)
            error(answer, 7);
    }
    return final_answer;
}

bool IMAP::logout(){
    clear_error();
    communicate("LOGOUT");
    return error_happened();
}

void IMAP::select(std::string mailbox){
    clear_error();
    std::string answer;
    answer = communicate("SELECT " + mailbox);
    std::cout<<answer<<std::endl;
}

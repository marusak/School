#include <iostream>
#include <string>

#include <string.h>
#include <stdlib.h>

#include <netdb.h> //hostent, gethostbyname
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>

#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include "imap.hh"


/* Print error to stderr and exit. */
void IMAP::error(std::string err_msg, unsigned err_code){
    this->err_code = err_code;
    this->err_msg = err_msg;
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
 */
IMAP::IMAP(){
    err_code = 0;
    message_id = 0;
}

bool IMAP::connect_to_server(std::string host, int port){
    secure = false;

    hostent *H = gethostbyname(host.c_str());
    if (!H)
        error("Can not get host by name", 2);

    //Create socket
    int sock;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        error("Can not create socket", 3);

    //Create sockaddr_in (socket on server)
    struct sockaddr_in server_sock;
    server_sock.sin_family = AF_INET;
    server_sock.sin_port = htons(port);
    memcpy(&(server_sock.sin_addr), H->h_addr, H->h_length);

    //Connect
    if (connect(sock,( sockaddr *)&server_sock, sizeof(server_sock)) == -1)
        error("Cannot connect to server", 4);

    connection_sock = sock;
    return error_happened();
}

bool IMAP::connect_to_server_s(std::string host, int port, std::string file, std::string dir){
    secure = true;

    OpenSSL_add_all_algorithms();
    OpenSSL_add_ssl_algorithms();
    ERR_load_BIO_strings();
    SSL_load_error_strings();

    BIO *outbio;
    ctx = SSL_CTX_new(SSLv23_client_method());
    if (!ctx)
        error ("CTX failed", 5);
    SSL *ssl;
    if (! SSL_CTX_load_verify_locations(ctx, (file.empty() ? NULL: file.c_str()), (dir.empty() ? NULL : dir.c_str())))
        error("Could not load certificate", 4);


    outbio = BIO_new_ssl_connect(ctx);
    BIO_get_ssl(outbio, & ssl);
    SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY);

    BIO_set_conn_hostname(outbio, (host+":"+std::to_string(port)).c_str());
    if (BIO_do_connect(outbio) <= 0)
        error(ERR_reason_error_string(ERR_get_error()), 5);

    connection_sock_s = outbio;
    return error_happened();
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
    if (last_line[0] == 'A'){
        if (!strncmp(last_line.c_str(), id.c_str(), id.size())){
            if (last_line[id.size()] == ' '){
                return true;
            }
        }
    }
    return false;
}


std::string IMAP::communicate(std::string message){
    if (secure)
        return communicate_s(message);
    std::string msg_id;
    msg_id = "A"+std::to_string(message_id++);
    std::string msg = msg_id + " " + message+'\n';
    if (send(connection_sock, msg.c_str(), msg.size(), 0) == -1)
        error("Cannot sent a message",5);

    char buf[1024];
    std::string answer{};
    int recieved;
    while ((recieved = recv(connection_sock, buf, 1024, 0))){
        answer.append(buf, recieved);
        if (message_ended(answer, msg_id) && recieved < 1024)
            break;
    }
    std::size_t last_line_b = answer.find_last_of("\n", answer.size() - 2);
    std::string command_completed = answer.substr(last_line_b + 1);
    std::string final_answer = answer.substr(0, last_line_b - 1);
    std::size_t start_pos = msg_id.size() + 2;
    std::size_t found = command_completed.find("NO");
    if (found+1 == start_pos)
        error(answer, 6);
    else {
        found = command_completed.find("BAD");
        if (found+1 == start_pos)
            error(answer, 7);
    }
    return final_answer;
}

bool IMAP::start_tls(){
    stop_tls();
    clear_error();
    communicate_s("STARTTLS");
    return error_happened();
}

bool IMAP::stop_tls(){
    clear_error();
    SSL_CTX_free(ctx);
    return error_happened();
}


std::string IMAP::communicate_s(std::string message){
    std::string msg_id;
    msg_id = "A"+std::to_string(message_id++);
    std::string msg = msg_id + " " + message+'\n';
    while (BIO_write(connection_sock_s, msg.c_str(), msg.size()) <= 0)
    {
        if (! BIO_should_retry(connection_sock_s)){
            error("Could not sent data to the server", 9);
            break;
        }
    }

    char buf[1024];
    std::string answer{};
    int recieved;

    while ((recieved = BIO_read(connection_sock_s, buf, 1024))){

        answer.append(buf, recieved);
        if (message_ended(answer, msg_id))
            break;
    }
    std::size_t last_line_b = answer.find_last_of("\n", answer.size() - 2);
    std::string command_completed = answer.substr(last_line_b + 1);
    std::string final_answer = answer.substr(0, last_line_b - 1);
    std::size_t start_pos = msg_id.size() + 2;
    std::size_t found = command_completed.find("NO");
    if (found+1 == start_pos)
        error(answer, 6);
    else {
        found = command_completed.find("BAD");
        if (found+1 == start_pos)
            error(answer, 7);
    }
    return final_answer;
}


bool IMAP::logout(){
    clear_error();
    communicate("LOGOUT");
    return error_happened();
}

std::string IMAP::select(std::string mailbox){
    clear_error();
    std::string answer;
    answer = communicate("SELECT " + mailbox);
    return answer;
}

std::string IMAP::fetch(std::string ids, std::string type){
    clear_error();
    std::string answer;
    answer = communicate("FETCH " + ids + " " + type);
    return answer;
}

std::string IMAP::search(std::string args){
    clear_error();
    std::string answer;
    answer = communicate("SEARCH " + args);
    return answer;
}

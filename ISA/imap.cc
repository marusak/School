#include <iostream>
#include <string>
#include <regex>

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

/*
 * Return last error message.
 */
std::string IMAP::get_error(){
    return this->err_msg;
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

/*
 * Connect to the server.
 *
 * host - name of server
 * port - port to connect through
 */
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
    /*
    OpenSSL_add_all_algorithms();
    ERR_load_BIO_strings();
    SSL_load_error_strings();

    BIO *bio;
    bio = BIO_new_connect((host+":"+std::to_string(port)).c_str());
    if (bio == NULL)
        error("Could not connect to server", 8);
    if (BIO_do_connect(bio) <= 0)
        error("Could not connect to server", 8);

    secure = true;
    connection_sock_s = bio;
    return error_happened();
    */

}

/*
 * Connect to the server securely.
 *
 * host - name of server
 * port - port to connect through
 * file - certificate file
 * dir  - certificate directory
 */
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
    SSL_CTX_set_default_verify_paths(ctx);
    SSL *ssl;
    //Load certificate
    if (! SSL_CTX_load_verify_locations(ctx, (file.empty() ? NULL: file.c_str()), (dir.empty() ? NULL : dir.c_str())))
        error("Could not load certificate", 4);

    //New connection
    outbio = BIO_new_ssl_connect(ctx);
    BIO_get_ssl(outbio, & ssl);
    SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY);

    //Connect
    BIO_set_conn_hostname(outbio, (host+":"+std::to_string(port)).c_str());
    if (BIO_do_connect(outbio) <= 0)
        error(ERR_reason_error_string(ERR_get_error()), 5);


    //if (SSL_get_verify_result(ssl) != X509_V_OK)
    //    error("Certificate could not be verified", 8);

    connection_sock_s = outbio;
    return error_happened();
}


/*Login to the server*/
bool IMAP::login(std::string login, std::string password){
    clear_error();
    communicate("LOGIN " + login + " "+password);
    return error_happened();
}

/*
 * Test if incoming message ended.
 *
 * msg - recieved message so far
 * id  - messages id
 */
bool IMAP::message_ended(std::string msg, std::string id){
    std::string last_line;
    //Get last line of message
    std::size_t last_line_b = msg.find_last_of("\n", msg.size() - 2);
    if (last_line_b == std::string::npos)
        last_line = msg;
    else
         last_line = msg.substr(last_line_b + 1);
    //Test if last line begins with message id
    if (last_line[0] == 'A'){
        if (!strncmp(last_line.c_str(), id.c_str(), id.size())){
            if (last_line[id.size()] == ' '){
                return true;
            }
        }
    }
    return false;
}

/*
 * Communicate with server.
 *
 * message - message to be sent to the server
 *
 * Sends the message and return message that was answered from server.
 */
std::string IMAP::communicate(std::string message){
    if (secure)
        return communicate_s(message);
    std::string msg_id;
    //Append new message id
    msg_id = "A"+std::to_string(message_id++);
    std::string msg = msg_id + " " + message+"\r\n";
    //Send the message
    if (send(connection_sock, msg.c_str(), msg.size(), 0) == -1)
        error("Cannot sent a message",5);

    char buf[1024];
    std::string answer{};
    int recieved;
    //Read the answer
    while ((recieved = recv(connection_sock, buf, 1024, 0))){
        answer.append(buf, recieved);
        if (message_ended(answer, msg_id) && recieved < 1024)
            break;
    }
    //Parse the answer - if succeeded or not and remove all unnecessary items
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


/*
 * Communicate with the server securely.
 *
 * See communicate method for better understanding.
 */
std::string IMAP::communicate_s(std::string message){
    std::string msg_id;
    msg_id = "A"+std::to_string(message_id++);
    std::string msg = msg_id + " " + message+"\r\n";
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

/*
 * Logout from the account.
 */
bool IMAP::logout(){
    clear_error();
    communicate("LOGOUT");
    return error_happened();
}

/*
 * Cleanup before exiting.
 */
void IMAP::finish(){
    if (secure){
        BIO_free_all(connection_sock_s);
        SSL_CTX_free(ctx);
    }
}

/*
 * Select mailbox to work with.
 *
 * mailbox - name of the mailbox
 */
std::string IMAP::select(std::string mailbox){
    clear_error();
    std::string answer;
    answer = communicate("SELECT " + mailbox);
    return answer;
}

/*
 * Get one or range of messages from server.
 *
 * ids  - id or range od ids of messages to fetch
 * type - type of data to fatch
 */
std::string IMAP::fetch(std::string ids, std::string type){
    clear_error();
    std::string answer;
    answer = communicate("FETCH " + ids + " " + type);
    return answer;
}

/*
 * Find all message ids that follow some condition.
 *
 * args - condition by which messages are selected
 */
std::string IMAP::search(std::string args){
    clear_error();
    std::string answer;
    answer = communicate("SEARCH " + args);
    return answer;
}

std::string get_mailbox_name(const std::string& msg, std::regex r){
    std::smatch match;
    if (std::regex_search(msg.begin(), msg.end(), match, r))
        return match[1];
    else{
        return "";
        }
}

/*
 * List all mailboxes.
 */
std::string IMAP::list(std::string parent, std::string del){
    clear_error();
    std::string all;
    std::string new_mailbox;
    std::regex rhc("^.*HasChildren.*?\\\".*?\\\" \\\"(.*)\\\"\r$");
    std::regex ra("^.*?\\\".*?\\\" \\\"(.*)\\\"\r$");
    std::string answer;
    answer = communicate("LIST \"\" " + parent );
    answer += "\r\n";
    std::istringstream iss(answer);
    for (std::string line; std::getline(iss, line); )
    {
        new_mailbox = get_mailbox_name(line, rhc);
        if (!new_mailbox.empty()){
            all += del+new_mailbox+"\n";
            all += list(new_mailbox+"/%", del+"    ");
        }
        else{
            new_mailbox = get_mailbox_name(line, ra);
            if (!new_mailbox.empty()){
                all += del+new_mailbox+"\n";
            }
        }
    }
    return all;
}

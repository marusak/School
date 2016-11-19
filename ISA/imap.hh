#include <string>
#include <openssl/bio.h>

/*This is only header file. For more information see imap.cc, especially
 * functions' description.
 */

class IMAP
{
    public:
        IMAP();
        bool connect_to_server(std::string server, int port);
        bool connect_to_server_s(std::string server, int port, std::string file, std::string dir);
        void error(std::string err_msg, unsigned err_code);
        bool error_happened();
        bool login(std::string login, std::string password);
        bool logout();
        std::string get_error();
        std::string select(std::string mailbox);
        std::string fetch(std::string ids, std::string type);
        std::string search(std::string args);
    private:
        BIO *connection_sock_s;
        SSL_CTX * ctx;
        unsigned err_code;
        bool secure;
        int connection_sock;
        std::string err_msg;
        std::string communicate(std::string message);
        std::string communicate_s(std::string message);
        unsigned message_id; //unique id for each message - this is counter

        void clear_error();
        bool message_ended(std::string msg, std::string id);
};

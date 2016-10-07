#include <string>
#include <openssl/bio.h>

class IMAP
{
    public:
        IMAP();
        bool connect_to_server(std::string server, int port);
        bool connect_to_server_s(std::string server, int port);
        void error(std::string err_msg, unsigned err_code);
        bool error_happened();
        bool login(std::string login, std::string password);
        bool logout();
        void select(std::string mailbox);
        void fetch(std::string ids, std::string type);
        bool start_tls();
        bool stop_tls();
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

#include <string>

class IMAP
{
    public:
        IMAP(std::string host, int port);
        void error(std::string err_msg, unsigned err_code);
        bool error_happened();
        bool login(std::string login, std::string password);
        bool logout();
        void select(std::string mailbox);
    private:
        unsigned err_code;
        int connection_sock;
        std::string err_msg;
        std::string communicate(std::string message);
        unsigned message_id; //unique id for each message - this is counter

        void clear_error();
        bool message_ended(std::string msg, std::string id);
};

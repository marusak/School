#include "imap.hh"

#include <iostream>
#include <string>

void error(std::string err_msg, int err_code){
    std::cerr<<err_msg;
    exit(err_code);
}


int main(){
    IMAP imap = IMAP("127.0.0.1", 143);
    if (imap.error_happened())
        error("Connecting to server was unsuccessful.", 1);

    if (imap.login("matej", "heslo"))
        error("Could not login to the server", 2);

    imap.select("INBOX");


    if (imap.logout())
        error("Could not logout from the server.", 5);
    return 0;
}

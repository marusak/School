#include "imap.hh"
#include <string.h>

#include <fstream>
#include <iostream>
#include <string>
#include <algorithm>

void error(std::string err_msg, int err_code){
    std::cerr<<err_msg<<std::endl;
    exit(err_code);
}

struct config{
    std::string server;
    int port;
    bool imaps;
    std::string certfile;
    std::string certaddr;
    bool n;
    bool h;
    std::string auth_file;
    std::string mailbox;
    std::string out_dir;
};


/* Simple command line parser.
 * Shamelessly stolen and improved from
 * http://stackoverflow.com/questions/865668/how-to-parse-command-line-arguments-in-c
 */
char *getCmdOption(char ** begin, char ** end, const std::string &option, bool value){
    char ** itr = std::find(begin, end, option);
    if (value){
        if (itr != end){
            if (++itr != end)
                return *itr;
            else
                error("Argument " + option + " needs parameter.", 3);
        }
    }
    else{
        if (itr != end)
            return (char *)1;
    }
    return 0;
}

struct config createConfig(int argc, char* argv[]){
    struct config conf;
    char *arg;

    if (argc < 2)
        error("Missing server", 6);

    conf.server = argv[1];

    //parse imaps
    if (getCmdOption(argv, argv + argc, "-T", false)){
        conf.imaps = true;
        if ((arg = getCmdOption(argv, argv + argc, "-c", true)))
            conf.certfile = arg;
        else
            conf.certfile = "TODO"; //TODO
        if ((arg = getCmdOption(argv, argv + argc, "-C", true)))
            conf.certaddr = arg;
        else
            conf.certaddr = "/etc/ssl/certs";
    }
    else
        conf.imaps = false;

    //Parse port
    arg = getCmdOption(argv, argv + argc, "-p", true);
    if (arg){
        if (!isdigit(arg[0]))
            error("Port is not a valid number", 1);
        conf.port = std::stoi(arg);
        if (strlen(arg) != std::to_string(conf.port).length())
            error("Port is not a valid number", 1);
    }
    else if (conf.imaps)
        conf.port = 993;
    else
        conf.port = 143;

    if (getCmdOption(argv, argv + argc, "-n", false))
        conf.n = true;
    else
        conf.n = false;

    if (getCmdOption(argv, argv + argc, "-h", false))
        conf.h = true;
    else
        conf.h = false;

    if ((arg = getCmdOption(argv, argv + argc, "-a", true)))
        conf.auth_file = arg;
    else
        error("Authentication file was not specified",4);

    if ((arg = getCmdOption(argv, argv + argc, "-b", true)))
        conf.mailbox = arg;
    else
        conf.mailbox = "INBOX";

    if ((arg = getCmdOption(argv, argv + argc, "-o", true)))
        conf.out_dir = arg;
    else
        error("Output directory was not specified",4);

    return conf;
}




int main(int argc, char* argv[]){
    struct config  config = createConfig(argc, argv);

    IMAP con = IMAP();

    if (config.imaps){
        con.connect_to_server_s(config.server, config.port);
    }else
        con.connect_to_server(config.server, config.port);

    if (con.error_happened())
        error("Connecting to server was unsuccessful.", 1);

    if (config.imaps)
        con.start_tls(); //try but ignore errors

    std::ifstream auth_file(config.auth_file);
    if (!auth_file.is_open())
        error("Authentication file does not exist",9);

    std::string line;
    std::string login;
    std::string passwd;

    getline(auth_file, line);
    if (!line.compare(0, 11, "username = "))
        login = line.substr(line.find(" =") + 3);
    else
        error("Invalid authentication file", 4);

    getline(auth_file, line);
    if (!line.compare(0, 11, "password = "))
        passwd = line.substr(line.find(" =") + 3);
    else
        error("Invalid authentication file", 4);

    if (con.login(login, passwd))
        error("Could not login to the server", 2);

    con.select("INBOX");

    con.fetch("1:2", "BODY[TEXT]");

    if (con.logout())
        error("Could not logout from the server.", 5);

    if (config.imaps)
        con.stop_tls();
    return 0;
}

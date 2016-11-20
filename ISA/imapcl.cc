#include "imap.hh"
#include <string.h>

#include <fstream>
#include <iostream>
#include <string>
#include <algorithm>
#include <map>
#include <regex>

#include <openssl/bio.h>
#include <openssl/hmac.h>

/*Print error and exit.
 */
void error(std::string err_msg, int err_code){
    std::cerr<<err_msg<<std::endl;
    exit(err_code);
}

/*
 * Structure to hold all command line arguments.
 */
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
    bool help;
    bool list;
};


/* Simple command line parser.
 * Inspired on:
 * http://stackoverflow.com/questions/865668/how-to-parse-command-line-arguments-in-c
 * and edited
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

/*
 * Test if item in array.
 * Inspired by
 * http://stackoverflow.com/questions/20303821/how-to-check-if-string-is-in-array-of-strings
 */
bool in_array(const std::string &value, const std::vector<std::string> &array){
    return std::find(array.begin(), array.end(), value) != array.end();
}


/*
 * Find first free parameter and consider it as server
 */
std::string getServer(std::vector<std::string> args){
    std::vector<std::string> alone {"-n", "-h", "--list", "-T", "--help"};
    std::vector<std::string> with_p {"-p", "-c", "-C", "-a", "-b", "-o"};
    while (!args.empty()){
        if (in_array(args.front(), alone)){
            args.erase(args.begin());
        }
        else if (in_array(args.front(), with_p)){
            args.erase(args.begin(), args.begin() + 2);
        }
        else
            return args.front();
    }
    return "";

}

/*
 * Print help and exit.
 */
void help(){
    std::cout<<"IMAP client with TLS"<<std::endl;
    std::cout<<std::endl;
    std::cout<<"Usage:"<<std::endl;
    std::cout<<"     imapcl --help   -> show this help and exit\n";
    std::cout<<"     imapcl --list   -> show all mailboxes in hierarchy and exit\n";
    std::cout<<"     imapcl  server [-p port] [-T [-c certfile] [-C certaddr]]";
    std::cout<<"[-n] [-h] -a auth_file [-b MAILBOX] -o out_dir\n";
    exit(0);
}

/*
 * Parse all command line argumets.
 *
 * argc - number of arguments
 * argv - list of all arguments
 *
 * Creates config structure, fills it with passed or default values and
 *  and returnes it.
 */
struct config createConfig(int argc, char* argv[]){
    struct config conf;
    char *arg;

    if (argc < 2)
        error("Wrong arguments, use --help to learn more.", 6);

    //Read help
    if (getCmdOption(argv, argv + argc, "--help", false)){
        help();
    }

    std::vector<std::string> args(argv +1 , argv + argc);
    conf.server = getServer(args);
    if (conf.server.empty())
        error("Host was not specified.", 1);

    //parse imaps
    if (getCmdOption(argv, argv + argc, "-T", false)){
        conf.imaps = true;
        if ((arg = getCmdOption(argv, argv + argc, "-c", true)))
            conf.certfile = arg;
        else
            conf.certfile = "";
        if ((arg = getCmdOption(argv, argv + argc, "-C", true)))
            conf.certaddr = arg;
        else if (conf.certfile == "")
            conf.certaddr = "/etc/ssl/certs";
        else
            conf.certaddr = "";

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

    //Only new messages
    if (getCmdOption(argv, argv + argc, "-n", false))
        conf.n = true;
    else
        conf.n = false;

    //Only message heads
    if (getCmdOption(argv, argv + argc, "-h", false))
        conf.h = true;
    else
        conf.h = false;

    //List option
    if (getCmdOption(argv, argv + argc, "--list", false))
        conf.list = true;
    else
        conf.list = false;

    //Authentication file
    if ((arg = getCmdOption(argv, argv + argc, "-a", true)))
        conf.auth_file = arg;
    else
        error("Authentication file was not specified",4);

    //Mailbox
    if ((arg = getCmdOption(argv, argv + argc, "-b", true)))
        conf.mailbox = arg;
    else
        conf.mailbox = "INBOX";

    //Output directory
    if ((arg = getCmdOption(argv, argv + argc, "-o", true))){
        conf.out_dir = arg;
        conf.out_dir += '/';
    }
    else if (!conf.list)
        error("Output directory was not specified",4);

    return conf;
}

/*
 * Read the length of following message.
 *
 * msg - message from which to get the length
 * r   - regular expression for matching the message style
 *
 * Each message contains its lenght, and it needs to be parser in order to
 *  be able to read the whole message.
 */
int get_next_message_bytes(const std::string& msg, std::regex r){
    std::smatch match;
    if (std::regex_search(msg.begin(), msg.end(), match, r))
        return std::stoi(match[1]);
    else{
        error("Wrong return format from server", 8);
        return 0;
        }
}

/*
 * Read message identification.
 *
 * Works similiar to get_next_message_bytes - see for better understanding.
 */
std::string get_msg_uid(const std::string& msg, std::regex r){
    std::smatch match;
    if (std::regex_search(msg.begin(), msg.end(), match, r))
        return match[1];
    else{
        error("Wrong return format from server", 8);
        return 0;
        }
}


int main(int argc, char* argv[]){
    struct config  config = createConfig(argc, argv);

    IMAP con = IMAP();
    //Connect to server
    if (config.imaps){
        con.connect_to_server_s(config.server, config.port, config.certfile, config.certaddr);
    }else
        con.connect_to_server(config.server, config.port);

    if (con.error_happened())
        error("Connecting to server was unsuccessful.\n"+con.get_error(), 1);

    //Login in
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

    if (config.list){
        std::cout<<con.list();
        exit(0);
    }

    //Select mailbox
    std::string mailbox_info = con.select(config.mailbox);
    if (con.error_happened())
        error("Not valid mailbox name", 11);

    //Find messages based on criteria from command line
    std::string search_string = "ALL";
    if (config.n)
        search_string = "UNSEEN";

    std::string all_msg_ids = con.search(search_string);

    std::string first;
    std::string last;

    std::size_t first_space = all_msg_ids.find(" ", 8);
    if (first_space == std::string::npos)
        error ("No messages to download", 1);

    //Specify type of downloading content
    std::string req_type = "RFC822";
    if (config.h)
        req_type = "(BODY.PEEK[HEADER.FIELDS (DATE FROM TO SUBJECT CC BCC MESSAGE-ID)])";

    int n;
    std::size_t top;
    std::string head;
    std::string body = "";
    std::string uid;
    std::string file_name = config.out_dir + login + '_' + config.server + '_';
    file_name += config.mailbox + '_';

    std::ofstream out_msg;

    size_t pos = 0;
    std::string token;

    std::regex rf("\\* \\d* FETCH \\(.*\\)\\] \\{([0-9]*)\\}");
    std::regex rb(".*\\{(\\d*)\\}");
    std::regex rn("\\* (\\d*) FETCH.*");

    all_msg_ids.erase(0, 9);
    all_msg_ids.append(" ");
    std::string fetch_ans;

    //Download all messages
    int count = 0;
    while ((pos= all_msg_ids.find(" ")) != std::string::npos){
        token = all_msg_ids.substr(0, pos);
        all_msg_ids.erase(0, pos + 1);
        fetch_ans = con.fetch(token, req_type);
        n = get_next_message_bytes(fetch_ans, rb);
        top = fetch_ans.find("\n") + 1;
        body = fetch_ans.substr(top, n);
        top += n + 1;
        fetch_ans = fetch_ans.substr(top);
        out_msg.open(file_name+token);
        if (! out_msg.is_open())
            error("Directory does not exist or is not writable", 11);
        out_msg<<body;
        out_msg.close();
        count++;
    }

    //Logout
    if (con.logout())
        error("Could not logout from the server.", 5);

    //Inform about downloaded messages
    std::cout<<"Downloaded "<<count<<" message(s) from mailbox "<<config.mailbox<<"."<<std::endl;

    con.finish();

    return 0;
}

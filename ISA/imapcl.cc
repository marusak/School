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

//TODO --help, base64, prilohy(aspon zmazat...)

/* Decode base64 encoding
 * http://www.ioncannon.net/programming/122/howto-base64-decode-with-cc-and-openssl/
 * got inspired
 */
char *unbase64(unsigned char *input, int length){
    BIO *b64, *bmem;

    char *buffer = (char *)malloc(length);
    memset(buffer, 0, length);

    b64 = BIO_new(BIO_f_base64());
    bmem = BIO_new_mem_buf(input, length);
    bmem = BIO_push(b64, bmem);

    BIO_read(bmem, buffer, length);

    BIO_free_all(bmem);

    return buffer;
    //TODO should free(returned pointer)
}


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

    if ((arg = getCmdOption(argv, argv + argc, "-o", true))){
        conf.out_dir = arg;
        conf.out_dir += '/';
    }
    else
        error("Output directory was not specified",4);

    return conf;
}

int get_next_message_bytes(const std::string& msg, std::regex r){
    std::smatch match;
    if (std::regex_search(msg.begin(), msg.end(), match, r))
        return std::stoi(match[1]);
    else{
        error("Wrong return format from server", 8);
        return 0;
        }
}

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

    std::string mailbox_info = con.select(config.mailbox);

    std::ifstream conf_file;
    conf_file.open("/var/tmp/imapcl_file.conf");
    std::map <std::string, std::string> mailboxes;
    if (conf_file.is_open()){
        std::string key;
        std::string time;
        while(getline(conf_file, key)){
            getline(conf_file, time);
            mailboxes[key] = time;
        }
    }
    else
        error("Could not open configuration file", 6);
    conf_file.close();

    std::string search_string = "ALL";

    std::string all_msg_ids = con.search(search_string);

    std::string first;
    std::string last;

    std::size_t first_space = all_msg_ids.find(" ", 8);
    if (first_space == std::string::npos)
        error ("No messages to download", 1);
    else
        first = all_msg_ids.substr(first_space +1, all_msg_ids.find(" ", first_space + 1) - first_space -1);

    std::size_t last_space = all_msg_ids.rfind(" ");
    last = all_msg_ids.substr(last_space+1);

    if (config.n)
        if (mailboxes.find(config.mailbox) != mailboxes.end()){
            first = mailboxes[config.mailbox];
            if (first == last)
                error("No new messages", 1);
        }


    std::string req_type = "(BODY[HEADER.FIELDS (DATE FROM TO SUBJECT  CC BCC MESSAGE-ID)] RFC822.TEXT)";
    if (config.h)
        req_type = "(BODY[HEADER.FIELDS (DATE FROM TO SUBJECT  CC BCC MESSAGE-ID)])";
    std::string fetch_ans = con.fetch(first + ":" + last, req_type);

    while (fetch_ans[0] == '\n')
        fetch_ans.erase(0, 1);

    int n;
    std::size_t top;
    std::string head;
    std::string body = "";
    std::string uid;
    std::string file_name = config.out_dir + login + '_' + config.server + '_';

    std::ofstream out_msg;

    std::regex rf("\\* \\d* FETCH \\(.*\\)\\] \\{([0-9]*)\\}");
    std::regex rb(".*\\{(\\d*)\\}");
    std::regex rn("\\* (\\d*) FETCH.*");
    while (fetch_ans.length() > 5){
        while (fetch_ans[0] == '\n' || fetch_ans[0] == '\r')
            fetch_ans.erase(0, 1);
        n = get_next_message_bytes(fetch_ans, rf);
        uid = get_msg_uid(fetch_ans, rn);
        top = fetch_ans.find("\n") + 1;
        head = fetch_ans.substr(top, n);
        top += n + 1;
        fetch_ans = fetch_ans.substr(top);
        if (! config.h){
            n = get_next_message_bytes(fetch_ans, rb);
            top = fetch_ans.find("\n") + 1;
            body = fetch_ans.substr(top, n);
            top += n + 2;
            if (top > fetch_ans.length())
                top -= 1;
            fetch_ans = fetch_ans.substr(top);
        }
        out_msg.open(file_name+uid);
        if (! out_msg.is_open())
            error("Could not create output file", 11);
        out_msg<<head<<body;
        out_msg.close();
    }

    //save one by one to files
    //profit

    mailboxes[config.mailbox] = last;

    if (con.logout())
        error("Could not logout from the server.", 5);

    if (config.imaps)
        con.stop_tls();

    std::ofstream conf_file_w;
    conf_file_w.open("/var/tmp/imapcl_file.conf", std::ios::trunc);
    if (!conf_file_w.is_open())
        error("Could not open configuration file", 5);

    typedef std::map<std::string, std::string>::iterator item;
    for (item iterator = mailboxes.begin(); iterator != mailboxes.end(); iterator++){
        conf_file_w << iterator->first << std::endl;
        conf_file_w << iterator->second << std::endl;
    }

    return 0;
}

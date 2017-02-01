/* Projekt 1: Webovy klient pre VUT FIT predmet IPK
 * Autor: Matej Marusak, xmarus06@fit.vutbr.cz
 * 2015/2016
 */


#include <iostream>
#include <regex>//Ou Jes
#include <string>
#include <cstdlib>
#include <cstring>
#include <netdb.h> //hostent, gethostbyname
#include <sys/types.h>//socket
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <fstream>
#include <unistd.h>

//Struktura pre ulozenie rozprasovaneho linku
//1) vola sa tak hlupo, lebo link rezervovane slovo
//2) typedefujem lebo clang neuznava novsie normy
typedef struct LpLink{
    std::string host;
    unsigned port;
    std::string path;
}pLink;

//vytlaci error na stderr a ukonci program s chybovym kodom
void error(std::string err_message, unsigned err_code){
    std::cerr<<err_message<<std::endl;
    exit(err_code);
}


//rozparsuje pLink na 3 casti
//host
//port (ak nie je, tak sa tam da imlicitne 80)
//cesta k suboru
//#? stuff sa odstrania
pLink parseLink(std::string in){
    std::regex r("^http://([a-zA-Z0-9\\._-]+)(:[0-9]+)?(/(.*)*)*$");
    std::smatch found;
    std::regex_search(in, found, r);
    if (found.size() == 0)
       error("Chybny pLink", 85);

    pLink m;
    m.host = std::string(found[1]);
    m.port = ( found[2] == "" ? 80  : std::stoi(std::string(found[2]).substr(1)));
	m.path = ( found[3] == "" ? "/" : std::string(found[3]));

    size_t f = m.path.find(" ");

    while (f != std::string::npos){
        m.path.replace(f,1,"%20");
        f = m.path.find(" ");
    }
    f = m.path.find("~");

    while (f != std::string::npos){
        m.path.replace(f,1,"%7e");
        f = m.path.find("~");
    }
    return m;
}


//ak su data zachunkovane, tak ich odchunkuje
std::string unchunk(std::string chunked){
    std::string new_s{""};
    int n;
    while (true){
        chunked.erase(0,2);
        size_t pos = chunked.find("\r\n");
        if (pos == std::string::npos)
            error("Chunkujem a nie je co?",53);
        n = std::stoi(chunked.substr(0, pos).c_str(),0,16);
        if (!n)
            break;
        chunked.erase(0,pos+2);
        new_s += chunked.substr(0,n);
        chunked.erase(0,n);
    }
    return new_s;
}


std::string getContent(const char *got_pLink, int depth, bool onePointOne){
    //std::cout<<"Trying "<<(onePointOne ? "1.1\n" : "1.0\n");
    //prelozime si adresu
    pLink parsed = parseLink(got_pLink);


    /*
    hostent *H = gethostbyname(parsed.host.c_str());
    if (!H)
        error("Neviem prelozit adresu",142);
    //vytvorime socket
    int socket_1;
    if ((socket_1 = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
        error("Nejde urobit socket!",42);
    //a este treba sockaddr_in (socket na vzdialenom pocitaci)
    sockaddr_in anotherEnd;
    anotherEnd.sin_family = AF_INET;
    anotherEnd.sin_port = htons(parsed.port);
    memcpy(&(anotherEnd.sin_addr), H->h_addr, H->h_length);


    //CONNECT
    if (connect( socket_1,( sockaddr *)&anotherEnd, sizeof(anotherEnd)) == -1)
        error("Neviem naviazat spojenie",43);
*/

    //podpora ipv6 - pozicane z https://www.akkadia.org/drepper/userapi-ipv6.html
    //moja verzia len s ipv4 je uvedena vyssie
    struct addrinfo *res;
    struct addrinfo hints;
    memset(&hints, '\0', sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags=AI_ADDRCONFIG;
    int e = getaddrinfo(parsed.host.c_str(), "http", &hints, &res);
    if (e != 0) 
	    error("Neviem prelozit adresu",42);
    int socket_1 = -1;
    for (struct addrinfo *r = res; r != NULL; r = r->ai_next){
	    socket_1 = socket(r->ai_family, r->ai_socktype, r->ai_protocol);
	    if (socket_1 != -1 && connect(socket_1, r->ai_addr, r->ai_addrlen) == 0)
		    break;
	    if (socket_1 != -1) {
		    close(socket_1);
		    socket_1 = -1;
	    }
    }
	    
    //SEND
    int sent;
    //TODO HTTP 1.0 a 1.1
    std::string protocol = (onePointOne ? " HTTP/1.1" : " HTTP/1.0");
    std::string text("GET "+parsed.path + protocol +"\r\n"
                        "Host: "+parsed.host + "\r\n"
                        "Connection: close\r\n\r\n"
            );
    if ((sent = send(socket_1, text.c_str(), text.size() + 1, 0)) == -1)
        error("Neviem poslat udaje",44);
    //
    //RECV
    char buf[1000];
    std::string message{};
    int recieved;
    while ((recieved = recv(socket_1, buf, 1000, 0)) != 0)
        message.append(buf, recieved);
   //TODO close!
    //close(socket_1);


    //oddelit hlavicku a telo
    size_t header_pos = message.find("\r\n\r\n");
    if (header_pos == std::string::npos)
        error("NO header",44);
    std::string header = message.substr(0,header_pos);
    message = message.substr(header_pos+2);
    //KONTROLA return kodu
    std::string first_line = header.substr(0, header.find("\n"));
    std::regex r("^.*\\s(\\d+)\\s(.*)\\s*$");
    std::smatch found;
    std::regex_search(first_line, found, r);
    if (found.size() != 3)
       error("Chybny pLink", 85);
    int returned_code = std::stoi(found[1]);
    std::string error_message = found[2];

    //std::cout<<returned_code<<std::endl;
    //rozhodneme sa co ideme robit na zaklade return kodu
    if ( returned_code >= 400 and !onePointOne)
        error(error_message, 52);
    else if ( returned_code >= 400)
        getContent(got_pLink, depth, false);
    else if ( returned_code >= 300){
        if (depth > 5)
            error("Prilis hlboke zanorenie",12);
        std::string new_pLink{""};
        std::smatch found;
        std::regex r("Location: (.+)");
        std::regex_search(header, found,r);
        if (found.size() != 2 )
            error("Redirection but no adress?", 58);
        new_pLink = found[1];
        return getContent(new_pLink.c_str(), ++depth, true);
    }
    //Ak je chunked, musime to odchunk
    //ak nie je, tak ostava CRLF
    if (header.find("Transfer-Encoding: chunked") != std::string::npos)
        message = unchunk(message);
    else
        message.erase(0,2);
    close(socket_1);
    return message;
}

int main(int argc, char *argv[]){
    if (argc != 2)
        error("Nespravny pocet argumentov",1);
    pLink parsed = parseLink(argv[1]);
    std::string message = getContent(argv[1], 0, true);


    //zistime ci sa jedna o konkretny subor a zistime nazov suboru
    std::string file_name{"index.html"};
    if (parsed.path[parsed.path.length()-1] != '/'){
        size_t pos = parsed.path.rfind("/");
        if (pos == std::string::npos)
            error("Nekonci / ale ani neobsahuje /. filename?", 58);
        file_name = parsed.path.substr(pos+1);
    }

    size_t f = file_name.find("%20");

    while (f != std::string::npos){
        file_name.replace(f,3," ");
        f = file_name.find("%20");
    }
    f = file_name.find("%7e");

    while (f != std::string::npos){
        file_name.replace(f,3,"~");
        f = file_name.find("%7e");
    }
    //ZAPIS
    std::ofstream out;
    out.open(file_name, std::ios::out | std::ios::binary);
    if (out.fail())
        error("Cannot open output file", 54);
    out.write(message.c_str(), message.length());
    out.close();

    return 0;
}

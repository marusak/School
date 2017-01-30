#include <iostream>
#include <queue>
#include <fstream>
#include <cstring>


#define DEFAULT_N 10
#define E_DIFF 5
using namespace std;

int tail(int n, istream& file){
    string line;
    if ( n == 0){
        while(getline(file,line));
        return E_DIFF;
        }

    queue <string> buffer;
    int i = 0;
    while (i < n ){
    if (!getline(file,line))
        break;    
    buffer.push(line);
    i++;
    }
    if (n == i){
        while (1){
            if (!getline(file,line))
                break;
            buffer.pop();
            buffer.push(line);
        }
    }
    while (!buffer.empty()){
        cout << buffer.front()<<endl;
        buffer.pop();
    }
    return 0;
}

int from_line(int n, istream& file){
    string line;
    int i = 1;
    while(1){
        if(!getline(file,line)){
            return 0;
        }
        if (i>=n)
            cout<<line<<endl;
        i++;
    }
    return 1;
}



int main(int argc, char *argv[]){
    //ios::sync_with_stdio(false);
    char *err_buff;
    if (argc < 1 || argc > 4)
        cerr << "CHYBA: Nespravna prikazova riadka." << endl;
    else{
        if (argc == 1)
            tail(DEFAULT_N,cin);
        else if (argc == 2){
            if(argv[1][0] == '-' && argv[1][1] == 'n'){
                if (strlen(argv[1]) == 2){
                        cerr << "Parameter -n potrebuje hodnotu" << endl;
                        return E_DIFF;
                        }
                errno = 0;
                long int n = strtol((argv[1]+2),&err_buff,10);
                if (*err_buff != '\0' || errno != 0){
                    cerr << "Warning: Nie je cislo." << endl;       
                    return E_DIFF;
                }   
                if (argv[1][2] == '+')
                    from_line(n,cin);
                else
                    tail(n, cin);
            }
            else{
                fstream infile;
                infile.open(argv[1],ios::in);
                if (!infile.is_open()){
                    cerr << "Nespravny nazov suboru." << endl;
                    return E_DIFF;
                }
                else{ tail(DEFAULT_N,infile);
                    infile.close();
                } 
            }
        }
        else if (argc == 3){
            if(argv[1][0] == '-' && argv[1][1] == 'n'){
                if (strlen(argv[1]) == 2){
                errno = 0;
                long int n = strtol((argv[2]),&err_buff,10);
                if (*err_buff != '\0' || errno != 0){
                    cerr << "Warning: Nie je cislo." <<endl;       
                    return E_DIFF;
                    }
                if (argv[2][0] == '+')
                    from_line(n,cin);
                else
                    tail(n, cin);
                }
                else{
                    errno = 0;
                    long int n = strtol((argv[1]+2),&err_buff,10);
                    if (*err_buff != '\0' || errno != 0){
                        cerr << "Warning: Nie je cislo." <<endl;       
                        return E_DIFF;
                    }
                    fstream infile;
                    infile.open(argv[2],ios::in);
                    if (!infile.is_open()){
                        cerr << "Nespravny nazov suboru." << endl;
                        return E_DIFF;
                    }
                    if (argv[1][2] == '+')
                        from_line(n,infile);
                    else
                        tail(n,infile);
                    infile.close();
                }       
                }
        else {
                cerr << "Nespravny nazov suboru." <<endl;
                return E_DIFF;
            }
        }
        else{

        if (argv[1][0] != '-' || argv[1][1] != 'n' ){
            cerr << "CHYBA:nespravny prikazovy riadok." <<endl;
            return E_DIFF;
        }
        errno = 0;
        long int n = strtol(argv[2],&err_buff,10);
        if (*err_buff != '\0' || errno !=0){
            cerr << "CHYBA: nie je cislo.\n" << endl;
            return E_DIFF;
        }
        fstream infile;
        infile.open(argv[3],ios::in);
        if (!infile.is_open()){
            cerr << "Nespravny nazov suboru." << endl;
            return E_DIFF;
        }
        if (argv[2][0] == '+' )
            from_line(n,infile);
        else
            tail(n,infile);
        infile.close();
        }       
        
        }

    return 0;
}

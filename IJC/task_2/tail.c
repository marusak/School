// 3x ctrl+D pre ukoncenie ked pisem line in stdin aj to ked je len kratsia ako limit...
//ak som na dlhom roadku a dam ctrl+D tak nehasli ze bol dlhy


//tail.c
//Riesenie IJC-DU2, priklad a),3.4.2015
//Autor: Matej Marusak, xmarus06, VUT FIT
//prelozene gcc 4.8.3
//Implementacia fukncie tail v C

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>


//ukoncovaciu nulu neratam v LINE_LEN, ani v paramertoch length (treba o jedna pripocitat pri malloc, fgets...)

enum Errors{
    E_OK,
    E_DIFF,
    END_OF_STREAM,
    END_OF_STREAM_ON_LINE,
    TOO_LONG_LINE,
};

#define Close_file(name) do { if (fclose(name) == EOF)\
    fprintf(stderr,"%s","Nepodarilo sa zatvorit subor.\n");\
    } while(0)


//makro pre vypis riadku aj s odriadkovanim
#define print_line(line) do { \
    if (fputs(line,stdout) == EOF) \
        fprintf(stderr,"%s","Nepodarilo sa vytlacit riadok zaznamu.\n");\
    if (strchr(line,'\n') == NULL )\
        if (fputc('\n',stdout)== EOF)\
            fprintf(stderr,"%s","Nepodarilo sa odriadkovat.\n");}\
    while (0)

//max. dlzka jedneho riadka
#define LINE_LEN 510
#define DEFAULT_N 10

//vypis chyby a ukoncenie programu
#define Error(error) do{ \
    fprintf(stderr,"%s",error);\
    return E_DIFF;\
} while (0)


//typ pre ulozenie jedneho riadka
typedef struct line {
    char data[LINE_LEN+1];
    struct line *next;
}Line;

//typ pre cyklicky zoznam
typedef struct{
    Line *head;
}List;

//inicializacia zoznamu
int init_list(List *wl){
    Line *item = malloc(sizeof(Line));
    if (item == NULL)
        Error("Chyba pri alokacii.\n");
    wl->head = item;
    item->next=item;
    return E_OK;
}

//prida polozku za tu, na ktoru ukazuje head
void add_after(List *wl, Line *item){
    item->next=wl->head->next;
    wl->head->next = item;
    wl->head=item;
}


//odalokuje cely zoznam
void free_list(List *wl){
    Line *tmp = wl->head->next;
    Line *now;
    while (tmp != wl->head){
        now = tmp->next;
        free(tmp);
        tmp = now;
    }
    free(wl->head);
}


//Nacita riadok do s o maximalnej dlzke length, ak je riadok dlhsi tak ho splachne a vrati prislusny return
int read_line(char *s, size_t length, FILE *file){

    int c, over = 0; // pomocne premenne pre flushovanie

    //nacitam riadok, ak NULL tak koniec nacitavania, alebo chyba
    if (fgets(s,length+1,file)== NULL){
        return END_OF_STREAM;
    }
    
    //zistim ci sa vosiel riadok, ak nie zvysok flushnem
    if(s[strlen(s)-1] != '\n'){
        while ( ((c=fgetc(file)) != '\n') && c!=EOF)
            over = 1;
        if (c==EOF) return END_OF_STREAM_ON_LINE;
        if (over == 1)
            return TOO_LONG_LINE;
    }
    return E_OK;
    }

//Cita file alebo stdin, pre prvych n cisel (n=pocet riadkov ktore chceme vypisat) alokuje miesto, pre
//dalsie uz len prepisuje - cyklicky zoznam
int read_in(int n, List *list, FILE *file){

    if (file == NULL) 
        file = stdin;
    int count_err=0;
    int return_v;
    int i=1;

    if ((return_v = read_line(list->head->next->data,LINE_LEN,file)) == END_OF_STREAM)
        return END_OF_STREAM;
    else if (return_v == TOO_LONG_LINE && count_err == 0){
        count_err++;
        fprintf(stderr,"%s","Warning: Dlhy riadok.\n");
    }
    else if (return_v == END_OF_STREAM_ON_LINE)
        return END_OF_STREAM_ON_LINE;

    while (i<n){
        Line *tmp = malloc(sizeof(Line));
        if (tmp == NULL){
            free_list(list);
            Error("Chyba pri alokacii.\n");
        }
        if ((return_v = read_line(tmp->data,LINE_LEN,file)) == END_OF_STREAM){
            free(tmp);
            return END_OF_STREAM;
        }
        else if (return_v == TOO_LONG_LINE && count_err == 0 ){
            count_err++;
            fprintf(stderr,"%s","Warning: Dlhy riadok.\n");
        }
        add_after(list,tmp);
        i++;
    if (return_v == END_OF_STREAM_ON_LINE){
        return END_OF_STREAM_ON_LINE;
    }
        
    }
    char buffer[LINE_LEN+1];   
    while (1){
        if ((return_v=read_line(buffer,LINE_LEN,file)) == END_OF_STREAM){
            return END_OF_STREAM;
        }
        else if (return_v == TOO_LONG_LINE && count_err == 0){
            count_err++;
            fprintf(stderr,"%s","Warning: Dlhy riadok.\n");
        }

        strcpy(list->head->next->data,buffer);
        list->head = list->head->next;
    if (return_v == END_OF_STREAM_ON_LINE)
        return END_OF_STREAM_ON_LINE;
    }

return E_DIFF;
}


//tail s parametrom +
int from_line(int n, FILE *file){
    int i = 1;
    int return_v = E_OK;
    int count_err = 0;

    char line[LINE_LEN+1];
    while(1){
        if ((return_v = read_line(line,LINE_LEN,file)) == END_OF_STREAM){
            Close_file(file);
            return END_OF_STREAM;
        }
        else if (return_v == TOO_LONG_LINE && count_err == 0){
            count_err++;
            fprintf(stderr,"%s","Warning: Dlhy riadok.\n");
        }
        else if (return_v == END_OF_STREAM_ON_LINE){
            if (i >=n)
                print_line(line);
            Close_file(file);
            return END_OF_STREAM_ON_LINE;
        }
        if (i<n)
            i++;
        else 
            print_line(line);   
        }
    //na tuto cast by nemalo prist
    fclose(file);
    return E_DIFF;
}


//vytlaci obsah zoznamu v ktorom by mal byt klasicky tail
void print_out(List *list){
    Line *now = list->head->next;
    while (now != list->head){
        print_line(now->data);
        now = now->next;
    }
    print_line(list->head->data);
}

//klasicky tail bez +
int tail(int n, FILE *file){
    int c;
    if (n == 0){
        while ((c=fgetc(file)) != EOF);
                }
    else{
    List input;
    if (init_list(&input) != E_OK){
        Close_file(file);
        return E_DIFF;
    }
    if (read_in(n,&input,file) == E_DIFF){
        free_list(&input);
        Close_file(file);
        return E_DIFF;
    }
    print_out(&input);
    free_list(&input);
    Close_file(file);
    }
    return E_OK;
}



// main- spracovanie prikazovej riadky
int main(int argc, char *argv[]){
    char *err_buff;
    if (argc < 1 || argc > 4)
        fprintf(stderr,"%s","CHYBA: Nespravna prikazova riadka.\n");
    else{
        if (argc == 1)
            tail(DEFAULT_N,stdin);
        else if (argc == 2){
            if(argv[1][0] == '-' && argv[1][1] == 'n'){
                if (strlen(argv[1]) == 2){
                    fprintf(stderr,"%s","parameter -n potrebuje prepinac\n");
                    return E_DIFF;
                }
                errno = 0;
                long int n = strtol((argv[1]+2),&err_buff,10);
                if (*err_buff != '\0' || errno != 0){
                    fprintf(stderr,"%s","Warning: Nie je cislo.\n");       
                    return E_DIFF;
                }   
                if (argv[1][2] == '+')
                    from_line(n,stdin);
                else
                    tail(n, stdin);
            }
            else{
                FILE *file=fopen(argv[1],"r");
                if (file == NULL){
                    fprintf(stderr,"%s","Nespravny nazov suboru.\n");
                    return E_DIFF;
                }
                else tail(DEFAULT_N,file);       
            }
        }
        else if (argc == 3){
            if(argv[1][0] == '-' && argv[1][1] == 'n'){
                if (strlen(argv[1]) == 2){
                errno = 0;
                long int n = strtol((argv[2]),&err_buff,10);
                if (*err_buff != '\0' || errno != 0){
                    fprintf(stderr,"%s","Warning: Nie je cislo.\n");       
                    return E_DIFF;
                    }
                if (argv[2][0] == '+')
                    from_line(n,stdin);
                else
                    tail(n, stdin);
                }
                else{
                    errno = 0;
                    long int n = strtol((argv[1]+2),&err_buff,10);
                    if (*err_buff != '\0' || errno != 0){
                        fprintf(stderr,"%s","Warning: Nie je cislo.\n");       
                        return E_DIFF;
                    }
                    FILE *file = fopen(argv[2],"r");
                    if (file == NULL){
                        fprintf(stderr,"%s","CHYBA: neexistuje subor.\n");
                        return E_DIFF;
                    }
                    if (argv[1][2] == '+')
                        from_line(n,file);
                    else
                        tail(n,file);
                }       
                }
        else {
                fprintf(stderr,"%s","Nespravny nazov suboru.\n");
                return E_DIFF;
            }
        }
        else{

        if (argv[1][0] != '-' || argv[1][1] != 'n' ){
            fprintf(stderr,"%s","CHYBA:nespravny prikazovy riadok.\n");
            return E_DIFF;
        }
        errno = 0;
        long int n = strtol(argv[2],&err_buff,10);
        if (*err_buff != '\0' || errno != 0){
            fprintf(stderr,"%s","CHYBA: nie je cislo.\n");
            return E_DIFF;
        }
        FILE *file = fopen(argv[3],"r");
        if (file == NULL){
            fprintf(stderr,"%s","CHYBA: neexistuje subor.\n");
            return E_DIFF;
        }
        if (argv[2][0] == '+' )
            from_line(n,file);
        else
            tail(n,file);
        }       
        
        }

    return 0;
}

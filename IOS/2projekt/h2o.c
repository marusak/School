/*
 * IOS projekt 2 - implementacia synchonizacneho problemu building H2O
 * Matej Marusak, xmarus06@stud.fit.vutbr.cz
 * preklad gcc 4.8.3
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/types.h> //pid_t
#include <sys/wait.h> //waitpid
#include <sys/ipc.h>
#include <sys/shm.h>

//bezpecny zapis - vstup do kritickej sekcie, splachnutie vypisu
#define safe_write(OH, i,text) do{\
    sem_wait(s_write);\
    fprintf(mout,"%d:\t%c\t%d:\t%s\n",(*counter)++,OH,i,text);\
    fflush(mout);\
    sem_post(s_write);}\
    while (0)

//cakanie kym vsetci vypisu bond, ukoncenie molekuly, cakanie na dalsie
#define wait_till_done(atoms) do{\
                sem_wait(s_block);\
                (*bond3)++;\
                if (*bond3 == 3){\
                (*atom_counter)++;\
                if(*atom_counter == atoms )\
                    sem_post(s_finished);\
                    (*bond3) = 0;\
                    sem_post(s_now_in);\
                    sem_post(s_now_in);\
                    sem_post(s_now_in);\
                }\
                sem_post(s_block);}\
                while (0)



enum Errors {
    E_OK,   // vsetko OK
    E_SEM, //  chyba pri inicializaci/uvolnovani semaforu
    E_SHM //   chyba pri inicializaci/uvolnovani pamete 
};

//deklarujeme semafory
sem_t *s_oxygen,   //kyslik caka az moze ist do bond-u
      *s_hydrogen, //vodik  caka az moze ist do bond-u
      *barrier,    //kriticka sekcia vytvoreneho atomu
      *s_bonding,  //cakame az vsetci 3aja dokoncia bond
      *s_block,    //kriticka sekcia vyuzita na roznych miestach
      *s_finished, //cakame az vsetci dokoncia aby sme mohli finish-nut
      *s_write,    //kontrola aby iba jeden mohol zapisovat do suboru
      *s_now_in;   //cakam az vsetci dokoncia bonded

//ukazatele zdielanych premennych
int *hydrogens = NULL,    //pocitadlo cakajucich vodikov
    *oxygens = NULL,      //pocitadlo cakajucich kyslikov
    *counter = NULL,      //riadok vypisu
    *atom_counter = NULL, //pocitadlo hotovych atomov
    *bond3 = NULL;        //pomocne pocitadlo do 3

//ukazatej na vystupny subor
FILE *mout = NULL;

//struktura na ulozenie vstupnych parametrov
typedef struct parameters{
    long int n, gh, go, b;
}Parameters;

//deklaracie funkcii
void create_h(int n, int gh, int b);//vytvaranie a riadenie vodikov
void create_o(int n, int go, int b);//vytvaranie a riadenie kyslikov
void bond(char c, int c2, int b);//fukcia na spojenie 2*H a O
int initialize(void);//inicilaizacia semaforov a zdielanej pamete
int params(char *argv[], Parameters *p);//spracovanie parametrov prik. riadky
int free_all(void);//uvolnenie zdiel. pamete a semaforov
void kill_all(pid_t *processes, int n);

//----------------------------------Main--------------------------------------

int main(int argc, char *argv[]){
    Parameters pars;//parametre prik. riadky
    pid_t c_o, c_h=0; //pids pre create_oxygen a create_hydrogen
    
    //--------prikazova riadka ---------
    if (argc == 5){//spravny pocet argumentov
        if (params(argv,&pars) == 1)
            return 1;
    }
    else {//nespravny pocet argumentova
        fprintf(stderr,"%s","Nespravny pocet argumentov.\n");
        return 1;
    }
    
    //--------inicializacia a otvorenie suboru-----
    initialize();//inicializujeme data
    (*counter) = 1;
    if ((mout = fopen("h2o.out","w")) == NULL){
        fprintf(stderr,"Nepodarilo sa otvorit subor h2o.out\n");
            free_all();
            return 2;
    }
    //setbuf(mout,NULL);
    setlinebuf(mout);

    //-------vytvorime dva procesy H a O ----
    c_o = fork();
    if (c_o == 0){ //sme v create_oxygen
        create_o(pars.n, pars.go, pars.b); 
    }
    else if (c_o > 0){
        c_h = fork();
        if (c_h == 0){ //sme v create_hydrogen
            create_h(2*pars.n, pars.gh, pars.b); 
        }
        else if (c_h < 0){
            fprintf(stderr,"%s","Chyba pri fork\n");
            free_all();
            return 2;
        }
    }
    else{
        fprintf(stderr,"%s","Chyba pri fork\n");
        free_all();
        return 2;
    }
    //------------pockame na ukoncenie a uvolnime zdroje--------
    waitpid(c_o, NULL, 0);//pockame kym skonci create_o
    waitpid(c_h, NULL, 0);//pockame kym skonci create_h
    if (free_all() != E_OK)//uvolnime zdroje
        return 2;

    return 0;
}

//spracuva prikazovu riadku a uklada do premennej p
int params(char *argv[], Parameters *p){

    char *endptr;//ukazatel na zvysok retazca
    errno = 0;
    
    //spracujeme vsetky parametry jeden po druhom 
    p->n = strtol((argv[1]),&endptr,10);
    if (*endptr != '\0' || errno != 0 || p->n<=0){
        fprintf(stderr,"%s","Nespravne parametre\n");
        return 1;
    }
        
    errno = 0;
    p->gh = strtol((argv[2]),&endptr,10);
    if (*endptr != '\0' || errno != 0 || p->gh<0 || p->gh>=5001){
        fprintf(stderr,"%s","Nespravne parametre\n");
        return 1;
    }

    errno = 0;
    p->go = strtol((argv[3]),&endptr,10);
    if (*endptr != '\0' || errno != 0 || p->go<0 || p->go >=5001){
        fprintf(stderr,"%s","Nespravne parametre\n");
        return 1;
    }

    errno = 0;
    p->b = strtol((argv[4]),&endptr,10);
    if (*endptr != '\0' || errno != 0 || p->b<0 || p->b>=5001){
        fprintf(stderr,"%s","Nespravne parametre\n");
        return 1;
    }

    return 0;
}

//spoji 2 vodiky a kyslik
void bond(char c, int c2, int b){
    sem_wait(s_block);//vstup do kritickej sekcie
    if (*bond3 == 0)//ak prvy proces zacal bond
        sem_wait(s_bonding);//zamknem kym vsetci nedokoncia bond
    safe_write(c,c2,"begin bonding");
    int delay = 0;
    if (b != 0 )
        delay = (rand() % b);//vypocitam dobu tvorby
    usleep(delay * 1000);//a uspim
    (*bond3)++;//ked sa dokonci, zvysim pocitadlo
    if (*bond3 == 3){//ak to bol treti atom
        (*bond3) = 0;//resetujem pocitadlo
        sem_post(s_block);
        sem_post(s_bonding);//4krat uvolnim semafor (3* atomy a raz v bond)
        sem_post(s_bonding);
        sem_post(s_bonding);
        sem_post(s_bonding);
        return;
    }
    sem_post(s_block);//a vystupim s kriticikej sekcie
}


//Vytvara kyslik a riadi jeho proces 
void create_o(int n, int go, int b){
    srand(getpid());
    int i;
    pid_t pid;
    int delay = 0;
    pid_t *children = malloc(n*sizeof(pid_t));

    for (i = 1; i <= n; i++){ //vytvorime n procesov - pocitame od 1
        
        if (go != 0) //vypocitam si domu tvorby kyslika
            delay = (rand() % go);
        usleep(delay * 1000);//a uspim
        
        pid = fork();//ked sa zobudim vytvorime atom
        if (pid < 0) {//chyba pri tvorbe potomka, koncime s exit code 1
            fprintf(stderr,"Nepodarilo sa vytvorit potomka\n");
            free(children);//uvolnim alokovanu pamet
            free_all();//uvolnim zdielane premenne a semafory
            kill(0,SIGKILL);//a vsetkym poviem ze koncime
        }
        else if (pid == 0){ //sme vnutri noveho kyslika
            srand(getpid());//zasadime semienko pre random
            safe_write('O',i,"started"); 
            sem_wait(barrier);//a vstupime do kritickej sekcie
            if (*hydrogens > 1){//ak mame aspon 2 vodiky mozme zacat bond
                safe_write('O',i,"ready"); 
                (*hydrogens) -= 2;//znizime pocitadlo vodikov
                sem_post(s_hydrogen);//odomknem 2 vodiky
                sem_post(s_hydrogen);
                bond('O',i,b);//a zacnem bond
                sem_wait(s_bonding);//pockam kym vsetci dokoncia bond
                safe_write('O',i,"bonded");
                wait_till_done(n);
                sem_wait(s_now_in);//pockame kym vsetci vypisu bonded
                sem_post(barrier);//uvolnime kriticku sekciu
            }
            else {//mame maximalne jeden vodik
                safe_write('O',i,"waiting"); 
                (*oxygens)++;//zvysime posictadlo cakajucich kyslikov
                sem_post(barrier);//a vyskocime z kritickej sekcie
                sem_wait(s_oxygen);//cakame az nas niekto uvolni
                bond('O',i,b);//niekto na uvolnil, mozme ist bond-ovat
                sem_wait(s_bonding);//pockame az vsetci dokoncia bond
                safe_write('O',i,"bonded"); 
                wait_till_done(n);
                sem_wait(s_now_in);
            }
            sem_wait(s_finished);//pockam kym sa dokoncia vsetky molekuly
            sem_post(s_now_in);
            safe_write('O',i,"finished");
            sem_post(s_finished);
            exit (0);//uspesne ukoncime atom
        }
        else 
            children[i-1] = pid;//ulozim si do tabulky cislo potomka
        }
        for (int m = 0; m < n; m++)
            if(waitpid(children[m], NULL, 0) == -1)
                fprintf(stderr,"%s","Nepodarilo sa pockat na dokoncenie potomka\n");
        free(children);
        exit (0);
}

//vytvori n vodikov a ich riadenie -funguje skoro rovnako ako create_o a preto
//  nekomentuj tuto cast kodu

void create_h(int n, int gh, int b){
    int i;
    pid_t pid;
    int delay = 0;
    pid_t *children = malloc(n*sizeof(pid_t));
    //signal(SIGKILL,kill_all);
    for (i = 1; i <= n; i++){
        if (gh != 0 )
            delay = (rand() % gh);
        usleep(delay * 1000);
        
        pid = fork();
        if (pid < 0) {
            fprintf(stderr,"Nepodarilo sa vytvorit potomka\n");
            free(children);
            free_all();
            kill(0,SIGKILL);
        }
        else if (pid == 0){
            srand(getpid());
            safe_write('H',i,"started");
            sem_wait(barrier);
            if (*hydrogens>0 && *oxygens > 0){
                safe_write('H',i,"ready");
                (*hydrogens)--;
                (*oxygens)--;
                sem_post(s_hydrogen);
                sem_post(s_oxygen);
                bond('H',i,b);
                sem_wait(s_bonding);
                safe_write('H',i,"bonded");
                wait_till_done(n/2);
                sem_wait(s_now_in);
                sem_post(barrier);
            }

            else {
                safe_write('H',i,"waiting");
                (*hydrogens)++;
                sem_post(barrier);
                sem_wait(s_hydrogen);
                bond('H',i,b);
                sem_wait(s_bonding);
                safe_write('H',i,"bonded");
                wait_till_done(n/2);
                sem_wait(s_now_in);
            }
            sem_wait(s_finished);
            sem_post(s_now_in);
            safe_write('H',i,"finished");
            sem_post(s_finished);
        exit (0);
        }
    else 
        children[i-1] = pid;
    }
    for (int m = 0; m < n; m++)
        if(waitpid(children[m], NULL, 0) == -1)
            fprintf(stderr,"%s","Nepodarilo sa pockat na dokoncenie potomka\n");
    free(children);
    exit (0);
}


int initialize(void){
    int err = E_OK;

    //inicializujeme semafory//
    if ((s_oxygen= mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED) err = E_SEM;
    if ((s_hydrogen= mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED) err = E_SEM;
    if ((barrier= mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED) err = E_SEM;
    if ((s_bonding= mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED) err = E_SEM;
    if ((s_block= mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED) err = E_SEM;
    if ((s_finished= mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED) err = E_SEM;
    if ((s_now_in= mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED) err = E_SEM;
    if ((s_write= mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED) err = E_SEM;
    
    if (err == E_OK){
        if (sem_init(s_oxygen, 1, 0) == -1) err = E_SEM;
        if (sem_init(s_hydrogen, 1, 0) == -1) err = E_SEM;
        if (sem_init(barrier, 1, 1) == -1) err = E_SEM;
        if (sem_init(s_bonding, 1, 1) == -1) err = E_SEM;
        if (sem_init(s_block, 1, 1) == -1) err = E_SEM;
        if (sem_init(s_finished, 1, 0) == -1) err = E_SEM;
        if (sem_init(s_now_in, 1, 0) == -1) err = E_SEM;
        if (sem_init(s_write, 1, 1) == -1) err = E_SEM;
    }
    
    //inicializujem zdielane premenne
    if (err == E_OK){
    if ((oxygens= mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED) err = E_SEM;
    if ((hydrogens= mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED) err = E_SEM;
    if ((counter= mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED) err = E_SEM;
    if ((atom_counter= mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED) err = E_SEM;
    if ((bond3= mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED) err = E_SEM;
    
    }
    return err;
}

int free_all(void){
    fclose(mout);//zatvorit subor na vystup
    int err = E_OK;
    //munmap!!!!
    if (sem_destroy(s_oxygen) == -1) err = E_SEM;
    if (sem_destroy(s_hydrogen) == -1) err = E_SEM;
    if (sem_destroy(barrier) == -1) err = E_SEM;
    if (sem_destroy(s_bonding) == -1) err = E_SEM;
    if (sem_destroy(s_block) == -1) err = E_SEM;
    if (sem_destroy(s_finished) == -1) err = E_SEM;
    if (sem_destroy(s_now_in) == -1) err = E_SEM;
    if (sem_destroy(s_write) == -1) err = E_SEM;
    if ( err == E_SEM) fprintf(stderr, "Nepodarilo sa uvolnit semafor\n");

    munmap(s_oxygen,sizeof(sem_t));
    munmap(s_hydrogen,sizeof(sem_t));
    munmap(barrier,sizeof(sem_t));
    munmap(s_bonding,sizeof(sem_t));
    munmap(s_block,sizeof(sem_t));
    munmap(s_finished,sizeof(sem_t));
    munmap(s_now_in,sizeof(sem_t));
    munmap(s_write,sizeof(sem_t));
    munmap(oxygens,sizeof(int));
    munmap(hydrogens,sizeof(int));
    munmap(counter,sizeof(int));
    munmap(atom_counter,sizeof(int));
    munmap(bond3,sizeof(int));
    if ( err == E_SHM) fprintf(stderr, "Nepodarilo sa uvolnit pamet\n");
    
    return err;
}   

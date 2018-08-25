#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <set>
#include <cmath>

#include <stdlib.h>
#include <stdio.h>
#define bool int
#define true 1
#define false 0
#include <signal.h>
#include <unistd.h>
#include <ctype.h>
#include <pthread.h>


#include "sharedspice.h"
#include "netlist.h"

using namespace std;

bool no_bg = true;
int numvecs = 0;
set<string> vecnames;
static bool errorflag = false;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
map<double, double> bcs;
Netlist n;

int
ng_getchar(char* outputreturn, int ident, void* userdata);

int
ng_getstat(char* outputreturn, int ident, void* userdata);

int
ng_exit(int, bool, bool, int ident, void*);

int
ng_thread_runs(bool noruns, int ident, void* userdata);

int
ng_initdata(pvecinfoall intdata, int ident, void* userdata);

int
ng_data(pvecvaluesall vdata, int numvecs, int ident, void* userdata);

int
ng_getvoltage(double* voltage, double t, char* node, int ident, void* userdata);

int
cieq(register char *p, register char *s);

int
ciprefix(const char *p, const char *s);

int main(int argc, char** argv)
{
    if (argc != 2) {
        cout << "Usage: ./main <file.cir>" << endl;
        return 0;
    }
    const string circuitfile = argv[1];
    map<string, string> boundaryconds;
    n.load_from_file(circuitfile, &boundaryconds, 1.0); // too hardcoded?

    int ret;
    char *curplot, *vecname;
    char **vecarray;
    
    ret = ngSpice_Init(ng_getchar, ng_getstat, ng_exit,  NULL, ng_initdata, ng_thread_runs, NULL);
    ret = ngSpice_Init_Sync(ng_getvoltage, NULL, NULL, NULL, NULL);
    cout << "Init thread returned: " << ret << endl;

    cout << "****************************" << endl;
    cout << "**  ngspice shared start  **" << endl;
    cout << "****************************" << endl;

    cout << endl;

    /* load a ngspice input file with a tran simulation running for 15s */
    ret = ngSpice_Circ(n.get_netlist());
    
    pthread_mutex_lock(&mutex);
    ret = ngSpice_Command( (char*) "bg_run");

    // wait for background thread to start
    while(no_bg) {
        pthread_cond_wait(&cond, &mutex);
    }

    // wait for background thread to exit
    while(!no_bg) {
        pthread_cond_wait(&cond, &mutex);
    }
    pthread_mutex_unlock(&mutex);

    // Offer to save
    string save;
    cout << "Save output vectors? [y/n] ";
    getline(cin, save);
    while(save.length() != 1 || (tolower(save[0]) != 'y' && tolower(save[0]) != 'n')) {
        cout << "Please enter one of [y/n]." << endl;
        cout << "Save output vectors? [y/n] ";
        getline(cin, save);
    }
    if (tolower(save[0]) == 'n') {
        cout << "Exiting without saving..." << endl;
        return 0;
    }

    cout << "Available vectors are: " << endl;
    for (string name : vecnames) {
        cout << name << endl;
    }
    string format;
    cout << "Save format [a=ascii/b=binary]: ";
    getline(cin, format);
    while(format.length() != 1 || (tolower(format[0]) != 'a' && tolower(format[0]) != 'b')) {
        cout << "Please enter one of [a/b]." << endl;
        cout << "Save format [a/b]: ";
        getline(cin, format);
    }
    if (tolower(format[0]) == 'a') {
        ret = ngSpice_Command((char*)"set filetype=ascii");
        if (ret != 0) {
            cout << "Failed to set filetype to ASCII. Attempting to save in binary format.";
        }
    }
    string filename;
    cout << "Enter filename to save as: ";
    getline(cin, filename);
    cout << "Enter vectors to save, one per line, (or leave blank for all). Enter a blank line when you're done." << endl;
    string command = "write " + filename;
    string vec;
    while(true) {
        getline(cin, vec);
        if (vec.empty()) break;
        if (vecnames.find(vec) == vecnames.end()) {
            cout << "Vector not avaliable. Enter valid vectors to save and enter a blank line when you're done." << endl;
            continue;
        }
        command += vec;
    }

    cout << "Saving vectors. This may take a moment..." << endl;
    ret = ngSpice_Command(const_cast<char*>(command.c_str()));
    if (ret == 0) {
        cout << "Vectors saved." << endl;
    } else {
        cout << "There was a problem saving your vectors.";
    }

    cout << "Exiting..." << endl;

    return ret;
}

/* Callback function called from bg thread in ngspice to transfer
any string created by printf or puts. Output to stdout in ngspice is
preceded by token stdout, same with stderr.*/
int
ng_getchar(char* outputreturn, int ident, void* userdata)
{
    printf("%s\n", outputreturn);
    /* setting a flag if an error message occurred */
    if (ciprefix("stderr Error:", outputreturn))
        errorflag = true;
    return 0;
}


int
ng_getstat(char* outputreturn, int ident, void* userdata)
{
    printf("%s\n", outputreturn);
    return 0;
}

int
ng_thread_runs(bool noruns, int ident, void* userdata)
{   
    pthread_mutex_lock(&mutex);
    no_bg = noruns;
    if (noruns) {
        pthread_cond_signal(&cond);
        printf("bg not running\n");
    } else {
        pthread_cond_signal(&cond);
        printf("bg running\n");
    }
    pthread_mutex_unlock(&mutex);
    return 0;
}


/* Callback function called from bg thread in ngspice once upon intialization
   of the simulation vectors)*/
int
ng_initdata(pvecinfoall intdata, int ident, void* userdata)
{
    numvecs = intdata->veccount;
    for (int i = 0; i < numvecs; i++) {
        printf("Vector: %s\n", intdata->vecs[i]->vecname);
        vecnames.insert(intdata->vecs[i]->vecname);
    }
    return 0;
}


/* Callback function called from bg thread in ngspice if fcn controlled_exit()
   is hit. Do not exit, but unload ngspice. */
int
ng_exit(int exitstatus, bool immediate, bool quitexit, int ident, void* userdata)
{

    if(quitexit) {
        printf("DNote: Returned form quit with exit status %d\n", exitstatus);
        exit(exitstatus);
    }
    if(immediate) {
        printf("DNote: Unloading ngspice inmmediately is not possible\n");
        printf("DNote: Can we recover?\n");
    }

    else {
        printf("DNote: Unloading ngspice is not possible\n");
        printf("DNote: Can we recover? Send 'quit' command to ngspice.\n");
        errorflag = true;
        ngSpice_Command( (char*) "quit 5");
    }

    return exitstatus;
}

int
ng_getvoltage(double* voltage, double t, char* node, int ident, void* userdata)
{   
    *voltage = n.get_boundary_condition(node, t);
    return 0;
}

/* Case insensitive str eq. */
/* Like strcasecmp( ) XXX */
int
cieq(register char *p, register char *s)
{
    while (*p) {
        if ((isupper(*p) ? tolower(*p) : *p) !=
            (isupper(*s) ? tolower(*s) : *s))
            return(false);
        p++;
        s++;
    }
    return (*s ? false : true);
}

/* Case insensitive prefix. */
int
ciprefix(const char *p, const char *s)
{
    while (*p) {
        if ((isupper(*p) ? tolower(*p) : *p) !=
            (isupper(*s) ? tolower(*s) : *s))
            return(false);
        p++;
        s++;
    }
    return (true);
}


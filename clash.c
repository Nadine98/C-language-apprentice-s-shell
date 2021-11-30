#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <error.h>

#define DirLength  200

static void error(char*);
static void showPrompt(int i);

int main(int argc, char*argv[]){


}


static void error(char *msg){
	perror(msg);
	exit(EXIT_FAILURE);
}

static void showPrompt(int i){

    char *dir= (char *) malloc(DirLength*i*sizeof(dir));
    if(dir==NULL)
        error("malloc");

    char *tmp=getcwd(dir,DirLength-1);

    if (tmp==NULL && errno == ERANGE){
        showPrompt(i++);
    }else if(tmp== NULL && errno!=ERANGE)
        error("getcwd");
    else{
        printf("%s: ", dir);
        fflush(stdout);
    }


}

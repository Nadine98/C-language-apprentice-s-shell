#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <error.h>

#define DirLength  200

static void error(char*);
static void showPrompt();

int main(int argc, char*argv[]){
	

}


static void error(char *msg){
	perror(msg);
	exit(EXIT_FAILURE);	
}

static void showPrompt(){
	
	
}

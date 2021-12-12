#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include "plist.h"

#define UserInputLength 1337
#define ArgumentLength 80
#define MaxArraySize (UserInputLength/ArgumentLength)
 
// ---------------------------function declarations---------------------
static void error(const char*);
static void getPrompt();
static void getArguments(char [MaxArraySize][ArgumentLength], int *, char *);
static void parentProcess(pid_t ,int ,char *);
static void zombie();
static int callback(pid_t, const char *);

//----------------------------------------------------------------------

int main(int argc, char*argv[]){
	
	char args[MaxArraySize][ArgumentLength];
	
	char **arguments=(char**)calloc(MaxArraySize,sizeof(arguments)); 
	if (arguments ==NULL)
		error("malloc");
		
	char userInput [UserInputLength];

	int numberArgs =0;
	int background =0;
	
	while(1){
		
		zombie();
		getPrompt();
		getArguments(args,&numberArgs, userInput);
		
		
		
		// if a blank line appears then continue with the next prompt 
		if (numberArgs<1){
			continue;
		}
			
		
		
		// Checking if the process is put in the background
		if((strcmp(args[numberArgs-1],"&")) ==0){
			
			background=1;
			
			char *c=strchr(userInput,'&');
			*(c-1)='\0';
				
			numberArgs=numberArgs-1;
			
		}
		
		
		// copy the arguments in an array of pointer for the exec function 
		for(int i =0; i < numberArgs; i++)
			arguments[i]=args[i];
			
			
		// Checking if the cd-command is used
		if((strcmp(arguments[0], "cd")) == 0){

			if(chdir(arguments[1]) == -1){
				if(errno =ENOENT){
					fprintf(stderr, "There is not a path with this name\n");
				}else{
				error("chdir");
				}
			}

			continue;
		}
		
		
		// Checking if the jobs-command is used
		else if ((strcmp(arguments[0], "jobs")) == 0){
			walkList(callback);
			continue;
		}
		
			
			
		// Creating a child process	
		pid_t pid= fork(); 
		
		
		
		if (pid < 0){
			error("fork");
		}
		// Being in the parent process 
		else if (pid > 0){
			parentProcess(pid,background,userInput );
			background=0;
		}
		
		// Being in the child process 
		else{
			
			// cleaning up the array for the next command
			// if the previous command is longer than the new command
			for(int i =0; i<numberArgs; i++){
				
				if(arguments[numberArgs]!=NULL){
					arguments[numberArgs]=NULL;
				}else {
					break;
				}
			}

			execvp(arguments[0], arguments);
			error("execvp");
			
		}
			
	}
	
	for(int i =0; i <=MaxArraySize ;i++)
		free(arguments[i]);
	free(arguments);
	
}

// ---------------------------function definitions----------------------

static int callback(pid_t pid, const char *cmdLine){
	printf("%d %s\n", pid, cmdLine);
	return 0;
}


static void zombie (){
	
	int bgStatus;
	pid_t bgpid;
	

		while (1){
			
			bgpid=waitpid(-1, &bgStatus,WNOHANG );
			
			
			// status is not available for any existing process --> Continue with printing prompt
			if(bgpid == 0){
				break;
			}
			// Error occurs
			else if(bgpid < 0){
				// no child process exists 
				if (errno == ECHILD)
					break;
				// error during the process execution 
				error("waitpid");
			}
			
			//status is available for a background process
			else{
				char command[UserInputLength+1];
				
				if((removeElement(bgpid,command,UserInputLength+1)) < 0){
					continue;
				}
				
				if (WIFEXITED(bgStatus)){
					if (printf("Exit Status: [%s] = %d\n", command, WEXITSTATUS(bgStatus))<0){
						error ("printf");	
					}

				}
				
			}
		}
}

static void parentProcess(pid_t pid, int background,char *commandLine){


	// Foreground process
	if(!background ){

		int status;
		if ((waitpid (pid, &status, 0))== -1){
			error("waitpid");
		}	
		
		if (WIFEXITED(status)){
			if(printf("Exit Status: [%s] = %d\n", commandLine, WEXITSTATUS(status)) < 0){
				error("printf");
			}
		}
	}
	// Background process
	else{
		
		// Insert the process in the list
		int x=insertElement(pid, commandLine);
		
		if(x==-2){
			error("insertElement");
		}
		
	}	
}

static void getArguments(char args[MaxArraySize][ArgumentLength], int *number, char *input){
	

	int numberOfArguments=0;
	char userInput[UserInputLength+1];
	char c;
	
	

	// Reading from Stdin --> Using maxLength 1337+2 for the valididation of the input length 
	if (fgets(userInput, UserInputLength+1, stdin) == NULL){
		if(feof(stdin)>0){
			
			if(printf("\n")	< 0)
				error("printf");
			exit(EXIT_SUCCESS);
		}
		error("fegts");
	}
	
	fflush (stdin);	
	
	//Input length is too long ( > 1337) 
	if (strlen(userInput)== UserInputLength && userInput[UserInputLength-1]!='\n'){
		
		if (fprintf(stderr,"Line is too long\n") < 0)
			error("fprintf");
		
		while((c=getchar()) != '\n' && c!=EOF){
			continue;
		}
		
		numberOfArguments=-2;
		
	}
	
	// Blanc line
	if (strlen(userInput)<=1){
		numberOfArguments = -1; 
	}
	
	// Removing the \n
	userInput[strlen(userInput)-1]='\0';

	if (sprintf(input,"%s", userInput )< 0){
		error("sprintf");
	}
	
 
	// Splitting up the input 
	char *temp=strtok(userInput, " \t");
	if(snprintf(args[numberOfArguments++],ArgumentLength, "%s",temp )< 0){
		error("snprintf");
	}
	
	while((temp=strtok(NULL, " \t"))!= NULL){
		
		if (snprintf(args[numberOfArguments++],ArgumentLength, "%s",temp ) < 0){
			error("snprintf");
		}

		// Exceed maximal Array Size --> too many Arguments 
		if (numberOfArguments == MaxArraySize-1){
			if(fprintf(stderr, "Too many arguments") < 0)
				error("fprintf");
			exit(EXIT_FAILURE);
		}
	} 
	
	*number = numberOfArguments;
	
}


static void getPrompt(){
	
    unsigned int dirLength=10;
    char *dir=NULL;
    char *tmp;
   
 
    while(1){
		// fetch the directory
		 tmp=getcwd(dir,dirLength);

		// Checking if the lenght of the directory is too long for dir 
		if (tmp==NULL && errno == ERANGE){

				dirLength= dirLength*2;
				dir = realloc(dir,dirLength);
				if (dir == NULL)
					error("realloc");
		}
	    
		// Checking if an error occurs during the execution of getcwd    
		else if (tmp==NULL && errno != ERANGE){
			error("getcwd");
		}
	    
	    // No problems during the execution of getcwd
	    else{
			break;
		}
		
	}
    

	if (printf("%s: ", dir)< 0){
		error("printf");
	}
	
	if (fflush(stdout)==EOF)
		error("fflush");
		
	free(dir); 
}

// Error handling
static void error(const char *msg){
	perror(msg);
	exit(EXIT_FAILURE);
}



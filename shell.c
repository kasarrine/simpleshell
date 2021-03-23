/*
Group Members: Dalton Avery, Casey Porter and Kirk Sarrine
Course: COSC 4302 - Spring 2021
Instructor: Dr. Sun
Project: A Simple Shell
*/

#include	<stdio.h>
#include	<string.h>
#include	<unistd.h>
#include 	<stdlib.h>

//#define	DEBUG
#define MAX_ARGS	64
#define MAX_ARG_LEN	16
#define	MAX_LINE_LEN	80
#define WHITESPACE	" .,\t\n"


struct command_t {
  char *name;
  int argc;
  char *argv[MAX_ARGS];
};

/* Function prototypes */
int parseCommand(char *, struct command_t *);
int parsePath(char **);
int getarg(char **, char *);
void printPrompt();
void readCommand(char *, int);


int main() {
	int i;
	int pid, numChildren;
	int status;
	FILE *fid;
	char cmdLine[MAX_LINE_LEN];
	struct command_t command;

	//fgets(cmdLine, MAX_LINE_LEN, stdin);

	//printf("The user has entered '%s'\n", cmdLine);
	readCommand(cmdLine, MAX_LINE_LEN);


	/* Process each command in the launch file */
	numChildren  = 0;

	printf("Command Line: '%s'\n", cmdLine);
	parseCommand(cmdLine, &command);
	command.argv[command.argc] = NULL;

	/* Create a child process to execute the command */
	if((pid = fork()) == 0) {
	  /* Child executing command */
//#ifdef DEBUG
		printf("command.name = '%s'\n", command.name);
	    printf("command.argc = '%d'\n", command.argc);
	    for(i = 0; i <= command.argc; i++) { 
	    	printf("command.argv[%d]= '%s'\n", i, command.argv[i]);
	    }
//#endif
	    execvp(command.name, command.argv);
	  /* Parent continuing to the next command in the file */
	    numChildren++;
	}
	printf("\n\nlaunch: Launched %d commands\n", numChildren);
	return 0;
}

/* Reads in user commands from the keyboard */
void readCommand(char *buffer, int buffer_size) {
	fgets(buffer, buffer_size, stdin);
}

int parseCommand(char *cLine, struct command_t *cmd) {
/* Determine command name and construct the parameter list */

    int argc;
    char **clPtr;

/* Initialization */
    clPtr = &cLine;
    argc = 0;
    cmd->argv[argc] = (char *) malloc(MAX_ARG_LEN);
    
/* Fill argv[] */
    while((cmd->argv[argc] = strsep(clPtr, WHITESPACE)) != NULL) {
        cmd->argv[++argc] = (char *) malloc(MAX_ARG_LEN);
    }

/* Set the command name and argc */
    cmd->argc = argc-1;
    cmd->name = (char *) malloc(sizeof(cmd->argv[0]));
    strcpy(cmd->name, cmd->argv[0]);
//    printf("HHH: %s\n", cmd->name);
    return  1;	
}

int parsePath(char *dirs[]) {
	/* This function reads the PATH variable for this environment, 
	then builds an array, dirs[], of the directories in PATH */
	char *pathEnvVar;
	char *thePath;

	pathEnvVar = (char *) getenv("PATH");
	thePath = (char *) malloc(strlen(pathEnvVar) + 1);

	// Intialize first split
	char *split = strtok(pathEnvVar, ":");

	// Loop serves dual purpose. Builds the directories in the path and
	// adds them to the 
	for(int i=0; i<MAX_ARGS; i++){
		if(split != NULL){
			dirs[i] = split;
			split = strtok(NULL,":");
		}
		else
			dirs[i] = NULL;
	}

 	return 1;
 }
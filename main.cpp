/*
Group Members: Dalton Avery, Casey Porter and Kirk Sarrine
Course: COSC 4302 - Spring 2021
Instructor: Dr. Sun
Project: A Simple Shell
*/

#include <atomic>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <iterator>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

using std::atomic;
using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::vector;

class Command_t{
public:
    Command_t(){
        counter = 0;            // Initialize atomic counter
        parsePath();            // Parse the path environment
        user = getenv("USER");  // Get the current user
    }

    void readCommand(){              // Reads in command from user
        input.clear();               // Clear string for previous line input
        getline(cin, input);  // Read in entire line
        getArgs(input, args,' '); // Process line, Delimiter for spaces
    }

    void parsePath(){ getArgs(getenv("PATH"), paths, ':'); } // Delimiter for :
    const string& getUser() {return user;}    // Current user logged in
    char *getCommand() const {return args[0];}  // Command used for 1st parameter in execv
    char *getFullPath() const {return const_cast<char *>(fullPath.c_str());}
    // Args used for 2nd parameter in execv, passed as a pointer to vector (acts like a char* array).
    // Can use either &args[0] or args.data()
    char** getArgs() {return &args[0];}
    bool isInvalid() const {return getFullPath()[0] == '\0';}   // Check for invalid full path
    bool checkForExit() const{return string(getCommand()) == "exit"; } // Check for exit entered by user

    // Prints initial prompt once, then
    void printPrompt() {
        if(counter == 0) {
            cout << "****************************" << endl;
            cout << "***WELCOME TO THUNDERDOME***" << endl;
            cout << "****************************\n" << endl;
            cout << "<<<Enter exit to leave at any time>>>\n" << endl;
            cout << "[" << getUser() << "] K-$HELL >> ";
        }
        else
            cout << "[" << getUser() << "] K-$HELL >> ";
        ++counter; // Increment Atomic counter
    }

    // Lookup the path for the command
    void lookupPath() {
        fullPath.clear();                  // Clear previous fullPath
        string temp;                       // Used to build test fullPath

        if(args[0][0] == '/') {            // Check for '/' as the first character indicating a full path
            for(char* arg: args) {         // Build out test fullPath
                if (arg != nullptr) {      // Check for null terminator
                    // If access is good, then the command exists at the fullPath
                    if (access(arg, F_OK) == 0) {
                        fullPath = arg;
                        return;
                    }
                }
            }
        }

        for(char* path: paths) {               // If here, need to search for the command in path directories
            if (path != nullptr) {
                temp = string(path) + "/" + args.at(0); // Build out temp fullPath

                // If temp fullPath access is ok, return the fullPath and return from function
                if (access(temp.c_str(), F_OK) == 0) {
                    fullPath = temp;
                    return;
                }
            }
        }
    }

private:
    string fullPath, user, input;
    vector<char*> args, paths;
    atomic<int> counter;

    // Used to parse both the path environment and commands from the terminal
    static void getArgs(const string& line, vector<char*>& vec, const char& delimiter)
    {
        vec.clear();             // Clear the vector of previous pointers/data
        char* path = strdup(line.c_str());
        while (char* all = strchr(path, delimiter))
        {
            *all = 0;            // Reset pointer
            vec.push_back(path); // Add each path
            for (path = all + 1; *path == ' '; ++path);
        }
        vec.push_back(path);    // Add the last path
        vec.push_back(nullptr); // Add null pointer to the end
    }
};

// Function for shell
void shell() {
    Command_t command;          // Also parses user environment
    pid_t childProcess;

    while(true) {
        command.printPrompt();  // Print the shell prompt
        command.readCommand();  // Read in the command

        if (command.checkForExit()) // Exit from the shell with keyword
            exit(0);

        command.lookupPath();   // Lookup the full path of command

        if(command.isInvalid()) { // Check for invalid command
            cout << "Invalid command" << endl;      // Print the invalid command
            cout << *command.getArgs() << ": command not found\n" << endl;
        }
        // Else the command is valid, create a child process
        else
            childProcess = fork();

        if (childProcess == 0) {                             // Child process executes
            execv(command.getFullPath(), command.getArgs()); // Execute the command
            exit(0);
        }
        else
            wait(nullptr); // Parent process waits for child to terminate
    }
}

int main() {
    shell();
    return 0;
}
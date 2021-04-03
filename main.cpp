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

using namespace std;

class Command_t{
public:
    Command_t(){
        counter = 0;            // Initialize atomic counter
        parsePath();            // Parse the path environment
        user = getenv("USER");  // Get the current user
    }

    void readCommand(){
        string str;
        getline(cin, str);  // Read in entire line
        getArgs(str, args,' '); // Process line, Delimiter for spaces
    }

    void parsePath(){getArgs(getenv("PATH"), paths, ':');} // Delimiter for :
    const string &getUser() const {return user;}    // Current user logged in
    char *getCommand() const {return args[0];}  // Command used for 1st parameter in execvp
    char *getFullPath() const {return const_cast<char *>(fullPath.c_str());}
    char** getArgs() {return &args[0];} // Args used for 2nd parameter in execvp

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
        ++counter;
    }

    void lookupPath() {
        fullPath.clear();
        string temp;
        if(strcmp(args.at(0),"/") == 0)
            fullPath = args.at(0);

        for(char* item: paths) {
            if (item != nullptr) {
                temp = string(item) + "/" + args.at(0);

                if (access(temp.c_str(), F_OK) == 0) {
                    fullPath = temp;
                }
            }
        }
    }

private:
    string fullPath;
    string user;
    vector<char*> args;
    vector<char*> paths;
    atomic<int> counter;

    void debug() {
        cout << "\n********" << endl;
        cout << "fullPath: " << fullPath << endl;
        cout << "getfullPath: " << getFullPath() << endl;
        cout << "user: " << user << endl;
        cout << "args: ";
        for(char* item: args)
            if (item != nullptr)
                cout << item << endl;
        cout << "********\n" << endl;
    }
    // Used to parse both the path environment and commands from the terminal
    static void getArgs(const string& line, vector<char*>& vec, const char& delimiter)
    {
        vec.clear();
        char* path = strdup(line.c_str());
        while (char* all = strchr(path, delimiter))
        {
            *all = 0; // Reset pointer
            vec.push_back(path); // Add each path
            for (path = all + 1; *path == ' '; ++path);
        }
        vec.push_back(path); // Add the last path
        vec.push_back(nullptr); // Add null pointer to the end
    }
};

// Shell testing
void shell() {
    Command_t command; // Also parses user environment
    pid_t childProcess;

    while(true) {
        command.printPrompt();  // Print the shell prompt
        command.readCommand();  // Read in the command

        if (string(command.getCommand()) == "exit")
            exit(0);

        command.lookupPath();

        if(string(command.getFullPath()).empty()) {
            cout << "Invalid command.\n" << endl;
        }
        else {
            childProcess = fork();
        }

        if (childProcess == 0) {
            execv(command.getFullPath(), command.getArgs());
            exit(0);
        }
        else
            wait(nullptr);
    }
}

int main() {
    shell();
    return 0;
}
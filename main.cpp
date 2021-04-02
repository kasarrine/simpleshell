/*
Group Members: Dalton Avery, Casey Porter and Kirk Sarrine
Course: COSC 4302 - Spring 2021
Instructor: Dr. Sun
Project: A Simple Shell
*/

#include <atomic>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <sstream>
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
    void lookupPath(){ }

private:
    string command;
    char* fullCommandPath{};
    string user;
    vector<char*> args;
    vector<char*> paths;
    atomic<int> counter;

    // Used to parse both the path environment and commands from the terminal
    static void getArgs(const string& line, vector<char*>& vec, const char& delimiter)
    {
        vec.clear(); // Clear vector of previous data
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

// Testing
void shell() {
    Command_t command;
    command.printPrompt();

    while(true) {
        command.readCommand();
        if(string(command.getCommand()) == "exit")
            exit(0);
        int childProcess = fork();
        if(childProcess == 0)
            execvp(command.getCommand(), command.getArgs());
        else{
            if(wait(nullptr) > 0) /* Child terminating */
                command.printPrompt();
            if(string(command.getCommand()) == "exit")
                exit(0);
        }
    }
}

void parseTest(const string& line, vector<char*>* a, const char *delimiter)
{
    char* path = strdup(line.c_str());
    char *split = strtok(path, delimiter);
    // Loop serves dual purpose. Builds the directories in the path and
    // adds them to the
    while(split != nullptr){
        a->push_back(split);
        split = strtok(nullptr,delimiter);
    }
    a->push_back(nullptr);
}


int main() {
    shell();
    return 0;
}
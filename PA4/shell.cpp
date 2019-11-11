#include <fcntl.h>
#include <iostream>
#include <vector>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdio.h>
#include <algorithm>
#include <vector>
using namespace std;
string trim (string input){
    int i=0;
    while (i < input.size() && input [i] == ' ')
        i++;
    if (i < input.size())
        input = input.substr (i);
    else{
        return "";
    }
    
    i = input.size() - 1;
    while (i>=0 && input[i] == ' ')
        i--;
    if (i >= 0)
        input = input.substr (0, i+1);
    else
        return "";
    
    return input;
    

}

vector<string> split (string line, string separator=" "){
    vector<string> result;
    while (line.size()){

        if (line[0] == '\''){
            int quote;
            char eaccent = 130;
            for (int i = 1; i < line.length(); i++){
                if (line[i] == '|'){
                    line[i] = eaccent;
                }
                if (line[i] == '\''){
                    quote = i;
                    break;
                }
            }
            string trim = line.substr(1, quote-1);
            if (line[1] == '{'){
                trim.erase(remove(trim.begin(), trim.end(), ' '), trim.end());
            }
            line = trim + line.substr(quote + 1); 
        }
        if (line[0] == '"'){
            int quote1;
            char eaccent = 130;
            for (int i = 1; i < line.length(); i++){
                if (line[i] == '|'){
                    line[i] = eaccent;
                }
                if (line[i] == '"'){
                    quote1 = i;
                    break;
                }
            }
            string trim = line.substr(1, quote1-1);
            if (line[1] == '{'){
                trim.erase(remove(trim.begin(), trim.end(), ' '), trim.end());
            }
            line = trim + line.substr(quote1 + 1); 
        }
        size_t found = line.find(separator);
        if (found == string::npos){

            string lastpart = trim (line);
            if (lastpart.size()>0){
                result.push_back(lastpart);
            }
            break;
        }

        int findpipe;
        bool ispipe = false;
        bool end = true;
        int quote = 0;
        for (int i = 0; i < line.length(); i++){
            if (line[i] == '\''){
                quote++;
            }
            if (line[i] == '|' && quote != 2 && line.substr(0,4) == "echo"){
                findpipe = i;
                ispipe = true;
                end = false;
                line[i] = 130;
            }
        }
        if(ispipe && !end){
            continue;
        }
        string segment = trim (line.substr(0, found));
        //cout << "line: " << line << "found: " << found << endl;
        line = line.substr (found+1);

        //cout << "[" << segment << "]"<< endl;
        if (segment.size() != 0){ 
            if (segment.size() == 1 && ispipe){
                segment[0] = '|';
            }
            result.push_back (segment);
        }
        
        //cout << line << endl;
    }
    return result;
}

char** vec_to_char_array (vector<string> parts){
    char ** result = new char * [parts.size() + 1]; // add 1 for the NULL at the end
    for (int i=0; i<parts.size(); i++){
        // allocate a big enough string
        result [i] = new char [parts [i].size() + 1]; // add 1 for the NULL byte
        strcpy (result [i], parts[i].c_str());
    }
    result [parts.size()] = NULL;
    return result;
}

void execute (string command){
    vector<string> argstrings = split (command, " "); // split the command into space-separated parts
    int fdwrite , fdread;
    char temp1[FILENAME_MAX];
    char temp2[FILENAME_MAX];


    for (int i = 0; i<argstrings.size(); i++){
        if (argstrings[i] == ">"){
            fdwrite = open(argstrings[i+1].c_str(), O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
            dup2(fdwrite,1);

            argstrings.erase(argstrings.begin()+i+1);
            argstrings.erase(argstrings.begin()+i);
        }
    }
    for (int i = 0; i<argstrings.size(); i++){
        if (argstrings[i] == "<"){
            fdread = open(argstrings[i+1].c_str(), O_RDONLY,  S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
            dup2(fdread,0);

            argstrings.erase(argstrings.begin()+i);
        }
    }

    if (argstrings[0] == "cd"){
        if (argstrings[1] == "-"){
            getcwd(temp2, sizeof(temp2));
            chdir(getenv("HOME"));
            chdir(temp1);
            strncpy(temp1, temp2, sizeof(temp2));

        }
        else{
            getcwd(temp1, sizeof(temp1));
            chdir(argstrings[1].c_str());
            
        }
        return;
    }
   
    char** args = vec_to_char_array (argstrings);// convert vec<string> into an array of char*
    execvp (args[0], args);
}

int main (){
    int stdin = dup(0);
    while (true){ // repeat this loop until the user presses Ctrl + C
        char temp[FILENAME_MAX];
        bool background = true;
        vector<pid_t> pidlist;
        cout << getcwd(temp, sizeof(temp)) << "$ ";
        string commandline = "ls";/*get from STDIN, e.g., "ls  -la |   grep Jul  | grep . | grep .cpp" */
        getline(cin,commandline);
        int pos;
        size_t backgroundexists = commandline.find("&");
        if (backgroundexists != string::npos){
            background = false;
            pos = backgroundexists;
            commandline.erase(commandline.begin()+pos);
        }
        // split the command by the "|", which tells you the pipe levels
        if (commandline != ""){
            vector<string> tparts = split (commandline, "|");
           
            // for each pipe, do the following:
            for (int i=0; i<tparts.size(); i++){
                int fd[2];
                pipe(fd);
                // make pipe
                if (!fork()){
                    // redirect output to the next level
                    // unless this is the last level
                    if (i < tparts.size() - 1){
                        dup2(fd[1],1);
                        // cout<<"in here"<<endl;
                        // int fd1 = open("out.txt", O_CREAT, O_WRONLY);
                        // redirect STDOUT to fd[1], so that it can write to the other side
                        close (fd[1]);   // STDOUT already points fd[1], which can be closed
                    }
                    //execute function that can split the command by spaces to 
                    // find out all the arguments, see the definitionls 
                    execute (tparts [i]); // this is where you execute
                }else{
                    pidlist.push_back(getpid());
                    if (background == false){
                        for (int i = 0; i < pidlist.size(); i++){
                            waitpid(i,0, WNOHANG);
                        }
                    }
                    else{
                        wait(0);            // wait for the child process
                        // then do other redirects
                        dup2(fd[0],0);
                        close(fd[1]);
                    }
                }
            }
            dup2(stdin, 0);
            // cout<<endl;
            }
        }
    // }
}
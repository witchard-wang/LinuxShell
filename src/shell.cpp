// Richard Wang
#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h> 
#include <fcntl.h>
#include <vector>
#include <bits/stdc++.h> 

using namespace std;

//prints vector
void checkComm(vector<string> commands){
    for(int i = 0; i < commands.size(); i++){
        cerr << commands[i] << endl;
    }
    cerr << "command size: " << commands.size()<< endl;
}
string trim (string s){
    string c = "";
    for (int i = 0; i< s.length()-1; i++){
        if(s[i] == ' ' && s[i+1]== ' '){
            continue;
        }
        c += s[i];
    }
    c += s[s.length()-1];
    c.erase(0, c.find_first_not_of(" "));
	c.erase(c.find_last_not_of(" ") + 1);

    return c;
}


vector<string> splitSpace(string arg, string delim){
    // cout << "Process arg: "<< arg << endl;
    string x = "";
    bool quote = false;
    vector<string>com;
    for (int i = 0; i < arg.length(); i++){
        if (!quote && x.length() > 1 && arg[i] == '>'){
            com.push_back(x);
            com.push_back(">");
            x="";
            continue;
        } else if (!quote && x.length() > 1 && arg[i] == '<'){
            com.push_back(x);
            com.push_back("<");
            x="";
            continue;
        }
        if((arg[i] == '\'' || arg[i] == '\"') && !quote){
            quote = true;
            continue;
        } else if ((arg[i] == '\'' || arg[i] == '\"') && quote){
            quote = false;
            continue;
        } else if (quote){
            x += arg[i];
            continue;
        }
        
        if(!quote && arg[i] != delim[0]){
            x += arg[i];
        } else if (!quote && x.length() > 0 && arg[i] == delim[0]){
            com.push_back(x);
            x="";
        }
    }
    if (x.length() > 0){
            com.push_back(x);
    }
    return com;
}
vector<string> splitPipe(string arg, string delim){
    string x = "";
    bool quote = false;
    vector<string>com;
    for (int i = 0; i < arg.length(); i++){
        if((arg[i] == '\'' || arg[i] == '\"') && !quote){
            quote = true;
            x += arg[i];
            continue;
        } else if ((arg[i] == '\'' || arg[i] == '\"') && quote && x.length() > 1){
            quote = false;
            x += arg[i];
            continue;
        } else if (quote){
            x += arg[i];
            continue;
        }
        
        if(!quote && arg[i] != delim[0]){
            x += arg[i];
        } else if (!quote && x.length() > 1 && arg[i] == delim[0]){
            com.push_back(x);
            x="";
        } 
    }
    if (x.length() > 0){
            com.push_back(x);
    }
    return com;
}

int getIdx(vector<string> s, string f){
    auto it = find(s.begin(),s.end(), f);
    if(it != s.end()){
        int index = distance(s.begin(),it);
        return index;
    } else{
        return -1;
    }
}

const string currentDateTime() {
    time_t now = time(0);
    struct tm tstruct;
    char buf[100];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y/%m/%d %X", &tstruct);

    return buf;
}

int main(){
    cout << "----------------------------------" << endl;
    cout << "------------Linux Shell-----------" << endl;
    cout << "----------------------------------" << endl;
    
    //bg processes
    vector<int> bgs;

    char text[255];
    FILE *name;
    name = popen("whoami", "r");
    fgets(text, sizeof(text), name);
    pclose(name);
    size_t ln = strlen(text) - 1;
    if (*text && text[ln] == '\n') 
        text[ln] = '\0';
    string date = currentDateTime();

    //maintain fd locations
    int std_in = dup(0);
    int std_out = dup(1);

    //current directory
    char cwd[1024];
    char prev[1024];
    getcwd(cwd, sizeof(cwd));
    getcwd(prev, sizeof(prev));
    bool cdir = false;

    while(1){
        for(int i = 0; i < bgs.size(); i++){
            if(waitpid(bgs[i], 0, WNOHANG) == bgs[i]){
                // cout << "Process: "<< bgs[i]<<endl;
                bgs.erase(bgs.begin()+i);
                i--;
            }
        }

        cout << text << ": " << date << " <My Shell>$ ";        
        string input;
        dup2(std_in,0);
        getline(cin, input);

        if(input == "exit"){ //exit out of loop
            cout << "Shell Terminated" << endl;
            break;
        } else if (input.find("cd")==0){ //check cd
            string cd = splitSpace(input," ")[1];
            if(cd == "-"){
                char temp[1024];
                getcwd(temp,sizeof(temp));
                chdir(prev);
                memcpy(prev, temp, sizeof(temp));
                getcwd(cwd, sizeof(cwd));
                printf("%s \n", cwd);
            } else {
                getcwd(prev, sizeof(prev));
                chdir(cd.c_str());
                getcwd(cwd, sizeof(cwd));
                printf("%s \n", cwd);
            }
            continue;
        }
        
        vector<string> pipe_p = splitPipe(input, "|");
        // checkComm(pipe_p);
        for(int i = 0; i < pipe_p.size(); i++){
            int fds[2];
            pipe(fds); 
            
            //background
            bool backg = false;
            int pos = pipe_p[i].find("&");
            if(pos != -1){
                pipe_p[i] = pipe_p[i].substr(0, pos-1);
                backg = true;
                // cout << "Bg"<< endl;
            }

            pid_t pid = fork();

        //Child Process
            if(pid == 0){
            //input processing    
                input = trim(pipe_p[i]);
                vector<string> commands = splitSpace(pipe_p[i], " ");
                
                // checkComm(commands);

            //I/O processing
                int pos = getIdx(commands,">");
                if(pos != -1){
                    string fname = commands[pos+1];
                    int fd = open(fname.c_str(), O_WRONLY|O_CREAT,S_IWUSR|S_IRUSR);
                    dup2(fd,1);
                    close(fd);
                    commands.erase(commands.begin()+pos);
                    commands.erase(commands.begin()+pos);
                }
                pos = getIdx(commands,"<");
                if(pos != -1){
                    string fname = commands[pos+1];

                    int fd = open(fname.c_str(), O_RDONLY|O_CREAT, S_IWUSR|S_IRUSR);
                    dup2(fd,0);
                    close(fd);
                    commands.erase(commands.begin()+pos);
                    commands.erase(commands.begin()+pos);
                }
                if(i<pipe_p.size()-1){
                    dup2(fds[1],1);
                }
                
                //check commands
                // cout << "check commands : " << endl;
                // checkComm(commands);

                char** args = new char*[commands.size()+1];
                for(int i = 0; i< commands.size();i++){
                    args[i] = (char*) commands[i].c_str();
                }
                args[commands.size()] = NULL;
                execvp(args[0],args);
                // exit(0);
                } else{
                    if(!backg){
                        waitpid(pid, 0, 0);
                    } else {
                        bgs.push_back(pid);
                    }
                    dup2(fds[0],0);
                    close(fds[1]);
                }
            
            }

        }
    return 0;
}
#include "TCPSocket.h"
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sstream>
#include <vector>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <assert.h>

using namespace std;

int httpStatus(string msg){
    int status = 0;
    string code = "";
    // cout << msg;
    for (int i = 9; i < 12; i++){
        code += msg[i];
    }
    int num = stoi(code);
    cout << endl << num << " code returned" << endl;

    if(num >= 300 && num < 400){
        status = 1;
    } else if(num == 404){
        status = 2;
    } else if (num >= 400){
        status = 3;
    }
    return status;
}

int nthOccurrence(string str, string let, int n){
    int pos = 0;
    int cnt = 0;

    while (cnt != n){
        pos += 1;
        pos = str.find(let, pos);
        if (pos == string::npos)
            return -1;
        cnt++;
    }
    return pos;
}

void parseInput(string input, string* dom, string* req){
    string host = input;
    string query = "";
    string request = "GET ";

    int loc = nthOccurrence(input , "/", 1);
    host = input.substr(0,loc);
    if(loc != input.length()-1)
        query = input.substr(loc,input.length()-3);
    
    cout << "host: " << host << endl;
    cout << "extra: " << query << endl;
    
    request += "/ HTTP/1.1\r\nHost: " + host +"\r\n\r\n";
    if (query != "")
        request = "GET "+ query + " HTTP/1.1\r\nHost: " + host +"\r\n\r\n";
    cout << request << endl;
    *dom = host;
    *req = request;
}

string parseRecv(string recv){
    bool chunked = false;
    if(recv.find("Transfer-Encoding: chunked") != string::npos){
        chunked = true;
    }
    recv = recv.substr(recv.find("\r\n\r\n") + 4);
    string newrecv = recv;
    if(chunked){
        newrecv = "";
        int f = 0;
        
        while(1){
            int l = recv.find("\n");
            // cout << recv.substr(0,l) << endl;
            int num = stol(recv.substr(f,l),0,16);
            if(num == 0)
                break;
            cout << "xd" << endl;
            newrecv += recv.substr(l+1,num);
            recv = recv.substr(num + l + 3);
            // cout << recv << endl;
            // cout << f << " " << l << endl;
            
            
        }
    }
    return newrecv;
}

int main(int argc, char *argv[]){
    //get input request
    string port = "80";
    string input = "";
    string fname = "file";
    int c;
    while ((c = getopt (argc, argv, "w:f:")) != -1){
        switch (c){
            case 'w':
                input = optarg;
                break;
            case 'f':
                fname = optarg;
                break;
        }
    }

    //parse input, setup http request
    cout << input << endl;
    input = input.substr(input.find("//")+2);
    cout << input << endl;
    string host;
    string request;
    parseInput(input, &host, &request);
    cout << input << endl;

    //setup socket to host
    TCPSocket* mainSock = new TCPSocket(host, port);
    string msgrecv;

    //send and receive http messages
    int status = 0;
    int redirects = 0;
    bool error = false;
    // vector<vector<char>> returned;
    while(redirects < 3){
        char buf[1024];
        int byte_count;

        cout << "Sending request" << endl << request;
        int w = mainSock->write((void*) request.c_str(), request.length());
        
        
        const char *start = "\r\n\r\n";
        const char *chunkName = "Transfer-Encoding: chunked";
        bool chunked = false;
        bool head = false;
        bool firstchunk = true;
        bool reached = true;
        int curr = 0;
        int loc = 0;
        while ((byte_count = mainSock->read(buf, sizeof(buf))) > 0) {
            cout << "received " << byte_count << "bytes" << endl;
            // // vector<char> ret(*buf, byte_count);
            // int dist = 0;
            
            
            // //check if http response is chunked
            // if (!chunked && !head){
            //     auto it = std::search(ret.begin(), ret.end(), chunkName, chunkName + strlen(chunkName));
            //     for(int i = 0; i < ret.size(); i++){
            //         cout << ret[i];
            //     }
            //     cout << endl << it - ret.begin() << endl;
            //     if (it != ret.end()){
            //         chunked = true;
            //         cout << "chunked" << endl;
            //     }
            // }

            // //header not finished
            // if(!head){
            //     auto it = search(ret.begin(), ret.end(), start, start + strlen(start));
            //     dist = distance(ret.begin(), it);
            //     head = true;
            // }

            // //header found
            // if(head){
            //     if(chunked && firstchunk){
            //         char* temp; 
            //         vector<char> range( &ret[dist], &ret[dist]+6);
            //         assert(range.size() <= 6);
            //         memcpy(temp,range.data(),range.size());
            //         long hex = std::stol (temp,nullptr,16);
            //         firstchunk = false;
            //         cout << hex << endl;
            //     }
            // }

            // returned.push_back(ret);
            msgrecv.append(buf, byte_count);
        }
        cout << "HTTP returned" << endl;
        // cout << msgrecv << endl;
        status = httpStatus(msgrecv);

        if(status == 0){
            break;
        }
        if(status == 1){
            redirects++;
            string newinp = "";
            string f = msgrecv.substr(msgrecv.find("Location: ")+10);
            newinp = f.substr(f.find("//")+2,f.find("\r\n")-f.find("//")-2);
            string newHost;
            parseInput(newinp,&newHost, &request);
            mainSock->changeSocket(newHost, port);
            msgrecv = "";
            cout << "Redirect: "<< redirects << endl << endl;
            error = true;
        }
        if (status == 2){
            cout << "404 error page not found";
            error = true;
            break;
        }
        if(status == 3){
            cout << "Other client error found" << endl;
            error = true;
            break;
        }
    }
    
    if(!error){
        //write to file
        cout << msgrecv.substr(0, msgrecv.find("\r\n\r\n") + 4) << endl;
        string finalmsg = parseRecv(msgrecv);
        // msgrecv = msgrecv.substr(msgrecv.find("\r\n\r\n") + 4);
        ofstream file(fname);
        file << finalmsg;
        cout << "File printed to: " << fname << endl;
    }
    
    delete mainSock;
}
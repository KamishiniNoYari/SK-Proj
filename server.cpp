#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <cstdlib>
#include <netinet/in.h>
#include <cstring>
#include <pthread.h>
#include <mutex>
#include "server.h"
#include <unordered_map>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
using namespace std;


//Global Variables
int server_fd;
int gamestart = 0;
vector<int> sockets;
int players = 0;
unordered_map<int, vector<string>> global_answers;

int main() {
    int new_socket;
    struct sockaddr_in new_address;
    int opt = 1;
    int addrlen = sizeof(new_address);
    char buffer[BUFFER_SIZE];
    pthread_t handlerThreadId;
    pthread_create(&handlerThreadId,NULL,timer_handler,(void *)&opt);
    //Create socket desc
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Socked creation failed");
        exit(EXIT_FAILURE);
    }

    //Attach socket to the port
    if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,&opt,sizeof(opt)))
    {
        perror( " Setting socket opt failed");
        exit(EXIT_FAILURE);
    }

    new_address.sin_family = AF_INET;
    new_address.sin_addr.s_addr = INADDR_ANY;
    new_address.sin_port = htons( PORT );

    //Bind
    if(bind(server_fd, (struct sockaddr *)&new_address,addrlen)<0)
    {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }
    //Listening
    if (listen(server_fd, 3 ) < 0)
    {
        perror("Listen error");
        exit(EXIT_FAILURE);
    }


    //Create new sockets loop
    while(true){
        //Wait for a client and accept
        if((new_socket = accept(server_fd,NULL,NULL)) < 0)
        {
            perror("Failed to accept client");
            close(server_fd);
            exit(1);
        }

        players++;
        sockets.push_back(new_socket);
        cout << "New client connected\n";

        //Create new thread to handle the client

        pthread_t threadid;
        pthread_create(&threadid, NULL,client_handler, (void *)&new_socket);
    }


    return 0;
}
void sendtoall(string message) {
    char cstr[message.size() + 1];
    memset(cstr,0,sizeof(cstr));
    strcpy(cstr, message.c_str());
    for (int fd : sockets) {
        write(fd, cstr, message.size() + 1);
    }
}
int iscountry(string word,char letter)
{
    int result;
    int multiplies = 0;
    if(word[0]!=letter)result = 0;
    else if ()
    return result;
}
int isanimal(string word,char letter)
{
    return 10;
}
int iscity(string word, char letter)
{
    return 10;
}
int isplant(string word, char letter)
{
    return 10;
}
int isname(string word, char letter)
{
    return 10;
}

void * timer_handler(void *arg)
{
    char buffer[BUFFER_SIZE];
    bool gamestarted = false;
    vector<string> emptyvector;
    srand(time(NULL));
    char random_letter;
    unordered_map<int, int> timeaswered;
    unordered_map<int, int> points;
    int actual_time = 0;
    int players_answered = 0;
    while(true)
    {
        if (gamestart>=players/2 && players>=2) {
            gamestarted = true;
            for (int player : sockets) {
                points[player] = 0;
            }
        }
            while(gamestarted)
            {
                //generate random letter
                string temp;
                random_letter = 'a' +(rand() % 26);
                temp.push_back(random_letter);
                sendtoall(temp);

                for( int player : sockets){
                    timeaswered[player] = -1;
                }
                //sendtoall("Wylosowana literka: "+random_letter);

                while(actual_time<30 && players_answered <= players/2)
                {
                    actual_time++;
                    for( const auto&[key, value]: global_answers){
                        if (value == emptyvector){continue;}
                        else{if (timeaswered[key] ==-1) {
                                timeaswered[key] = actual_time;
                                players_answered++;
                            }
                        }
                    }
                    sleep(1);
                }
                players_answered=0;
                sendtoall("End of round\n");
                //calculation points
                sendtoall("Calculating points\n");
                for( const auto&[key, value]: global_answers){
                    points[key]+=iscountry(value[0],random_letter)+iscity(value[1],random_letter)+
                            isanimal(value[2],random_letter)+isplant(value[3],random_letter)+
                            isname(value[4],random_letter);
                    if (timeaswered[key] !=-1) {points[key]+=15 - 0.5*timeaswered[key];}
                    string point_mess=to_string(points[key]);
                    timeaswered[key]=-1;
                    global_answers[key]=emptyvector;
                    char point_message[BUFFER_SIZE];
                    memset(point_message,0,sizeof(point_mess));
                    for(int i=0;i<sizeof(point_mess);i++){point_message[i]=point_mess[i];}
                    write(key,point_message,sizeof(point_message));

                }



                actual_time = 0;
                for ( int player : sockets){timeaswered[player] = -1;}
            }



    }
    return (void *)0;
}
void * client_handler(void * arg)
{
    int valread;
    int client_sock = *(int *)arg;
    char buffer[BUFFER_SIZE];
    bool client_connected = true;
    string outputMessage = "";
    bool starting = false;

    while(client_connected)
    {
        memset(buffer, 0, sizeof(buffer));
        valread = read( client_sock , buffer, BUFFER_SIZE);
        //write(client_sock,buffer,BUFFER_SIZE);
        string readstring = "";
        for(int i=0;i<=sizeof(buffer);i++){ if(buffer[i]!=NULL && buffer[i]!='\n')readstring.push_back(buffer[i]);}

        if(readstring == START && !starting){
            starting = true;

            gamestart++;
        }
        if(readstring.substr(0,2)==ANSWER)
        {
            string answerstring="";
            vector<string> answers;
            for(int i=3;i<sizeof(readstring);i++){
                if(readstring[i] == ';')
                {
                    answers.push_back(answerstring);
                    answerstring="";
                    continue;
                }
                answerstring.push_back(readstring[i]);
            }
            global_answers[client_sock] = answers;

        }



    }

    return (void *)0;
}

#pragma clang diagnostic pop

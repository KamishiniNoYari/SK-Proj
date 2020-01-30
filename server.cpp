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
#include <algorithm>
#include <condition_variable>
#include <chrono>
#include <ctime>


#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
using namespace std;

//Global Variables
int server_fd;
int gamestart = 0;
vector<int> sockets;
int players = 0;
unordered_map<int, vector<string>> global_answers;
mutex m1;
condition_variable cv;
bool gamestarted = false;
bool roundfinish = false;
unordered_map<int, int> timeaswered;
bool endgame = false;
int main() {
    int new_socket;
    struct sockaddr_in new_address;
    int opt = 1;
    int addrlen = sizeof(new_address);

  //  char buffer[BUFFER_SIZE];

    pthread_t handlerThreadId;

    pthread_create(&handlerThreadId,NULL,timer_handler,(void *)&opt);
    //Create socket desc
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Socked creation failed");
        exit(EXIT_FAILURE);
    }

    //Attach socket to the port
    if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT,&opt,sizeof(opt)))
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
    while(!endgame){
        //Wait for a client and accept
        if(endgame||(new_socket = accept(server_fd,NULL,NULL)) < 0)
        {
            if(endgame){close(server_fd);
                return 0;};
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
    shutdown(server_fd,SHUT_RDWR);
    close(server_fd);


    return 0;
}
void sendtoall(string message) {
    for (int fd : sockets) {
        write(fd, message.c_str(), message.size() + 1);
    }
}
int iscountry(string word,char letter)
{
    int result;
    int multiplies = 0;
    if(word[0]!=letter)result = 0;
    else{
        int n = sizeof(countries)/sizeof(countries[0]);
        auto itr = find(countries,countries+n,word);
        if(itr!=end(countries))
        {
            for( const auto&[key, value]: global_answers){
                if(value[0]==word)multiplies++;
                if(multiplies>=2)
                {
                    result = 5;
                    break;
                }

            }
            if(multiplies<=1)result = 10;
        }
        else result = 0;
    }
    return result;
}
int isanimal(string word,char letter)
{
    int result;
    int multiplies = 0;
    if(word[0]!=letter)result = 0;
    else{
        int n = sizeof(animals)/sizeof(animals[0]);
        auto itr = find(animals,animals+n,word);
        if(itr!=end(animals))
        {
            for( const auto&[key, value]: global_answers){
                if(value[2]==word)multiplies++;
                if(multiplies>=2)
                {
                    result = 5;
                    break;
                }

            }
            if(multiplies<=1)result = 10;
        }
        else result = 0;
    }
    return result;
}
int iscity(string word, char letter)
{
    int result;
    int multiplies = 0;
    if(word[0]!=letter)result = 0;
    else{
        int n = sizeof(cities)/sizeof(cities[0]);
        auto itr = find(cities,cities+n,word);
        if(itr!=end(cities))
        {
            for( const auto&[key, value]: global_answers){
                if(value[1]==word)multiplies++;
                if(multiplies>=2)
                {
                    result = 5;
                    break;
                }

            }
            if(multiplies<=1)result = 10;
        }
        else result = 0;
    }
    return result;
}
int isplant(string word, char letter)
{
    int result;
    int multiplies = 0;
    if(word[0]!=letter)result = 0;
    else{
        int n = sizeof(plants)/sizeof(plants[0]);
        auto itr = find(plants,plants+n,word);
        if(itr!=end(plants))
        {
            for( const auto&[key, value]: global_answers){
                if(value[3]==word)multiplies++;
                if(multiplies>=2)
                {
                    result = 5;
                    break;
                }

            }
            if(multiplies<=1)result = 10;
        }
        else result = 0;
    }
    return result;
}
int isname(string word, char letter)
{
    int result;
    int multiplies = 0;
    if(word[0]!=letter)result = 0;
    else{
        int n = sizeof(names)/sizeof(names[0]);
        auto itr = find(names,names+n,word);
        if(itr != end(names))
        {
            for( const auto&[key, value]: global_answers){
                if(value[4]==word)multiplies++;
                if(multiplies>=2)
                {
                    result = 5;
                    break;
                }

            }
            if(multiplies<=1)result = 10;
        }
        else result = 0;
    }
    return result;
}
string placechecker(int fd,unordered_map<int,int> points)
{
    vector<pair<int,int>> elems(points.begin(),points.end());
    sort(elems.begin(),elems.end(),[](const pair<int,int>& a, const pair<int,int>& b)
    {
        if(a.second != b.second) return a.second>b.second;
        return a.first>b.first;
    });
    for(size_t i=0 ; i<elems.size();++i)
    {
        if(elems[i].first==fd)
        {
            return to_string(i+1);
        }
    }




}
void * timer_handler(void *arg)
{
    char buffer[BUFFER_SIZE];
    srand(time(NULL));

    char random_letter;
    unordered_map<int, int> points;
    int actual_time = 0;
    int players_answered = 0;
    int rounds = 0;
    unique_lock<mutex>lk(m1);
    cv.wait(lk,[]{return gamestarted;});
    for (int player : sockets) {
        points[player] = 0;
    }
    lk.unlock();
    cv.notify_all();
    while(gamestarted)
    {
        //generate random letter
        string temp ="";
        random_letter = 'a' +(rand() % 26);
        temp.push_back(random_letter);
        sendtoall(temp+"\n");
        for( int player : sockets){
            timeaswered[player] = -1;
        }
        //sendtoall("Wylosowana literka: "+random_letter);
        rounds++;
        sendtoall("RO\n");
        roundfinish = false;
        unique_lock<mutex> lk(m1);
        cv.wait_until(lk,std::chrono::system_clock::now()+30s,[]{return roundfinish;});
        players_answered=0;
        sendtoall("EOR\n");
        //calculation points
        sendtoall("CP\n");
        for( const auto&[key, value]: global_answers){
            points[key]+=iscountry(value[0],random_letter)+
                    iscity(value[1],random_letter)+
                    isanimal(value[2],random_letter)+
                    isplant(value[3],random_letter)+
                    isname(value[4],random_letter);
            if (timeaswered[key] !=-1 && timeaswered[key]<=30) {points[key]+=15 - 0.5*timeaswered[key];}
            string point_mess=to_string(points[key]);
            timeaswered[key]=-1;
            global_answers[key].clear();
            char point_message[BUFFER_SIZE];
            for(int i=0;i<sizeof(point_mess);i++){
                if(point_mess[i]=='\n')break;
                point_message[i]=point_mess[i];}
            write(key,point_message,5);
        }
        actual_time = 0;
        for ( int player : sockets){timeaswered[player] = -1;}
        if(rounds==roundsNumber)
        {
            for (int player : sockets)
            {
                string place = "PLACE "+placechecker(player,points)+"\n";
                char cstr[place.size()+1];
                strcpy(cstr,place.c_str());
                write(player,cstr,place.size()+1);
                sendtoall("END\n");
            }
            //przechodzimy do wysyłania rankingu i odłączamy użytkowników
            //do zrobienia
            endgame=true;
            shutdown(server_fd,SHUT_RDWR);
            pthread_exit(NULL);
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
    auto start = std::chrono::system_clock::now();

    while(client_connected)
    {
        memset(buffer, 0, sizeof(buffer));
        valread = read( client_sock , buffer, BUFFER_SIZE);
        //write(client_sock,buffer,BUFFER_SIZE);
        if(valread==-1)
        {
            perror("Read error");
            //exit(EXIT_FAILURE);
        }
        string readstring = "";
        try {
            readstring.append(buffer, valread - 1);
        }
        catch(length_error)
        {
            perror("Length error");
            close(client_sock);
            sockets.erase(remove(sockets.begin(),sockets.end(),client_sock),sockets.end());
            players--;
            pthread_exit(NULL);
        }

        if(readstring == START && !starting){
            starting = true;
            gamestart++;
            if (gamestart>=players/2 && players>=2) {
                lock_guard<mutex>lk(m1);
                gamestarted=true;
                cv.notify_all();
            }
        }
        if(readstring.substr(0,2) == ROUND ||readstring.substr(3,5) == ROUND)
        {
            auto start = std::chrono::system_clock::now();
        }
        if(readstring.substr(0,2)==ANSWER && timeaswered[client_sock]==-1)
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
            auto end = std::chrono::system_clock::now();
            std::chrono::duration<double> elapsed = end - start;
            timeaswered[client_sock]=(int)elapsed.count();
            int players_answered = 0;
            for( const auto&[key, value]: timeaswered){
                if(value!=-1)
                {
                    players_answered++;
                }
            }
            if(players_answered>players/2){
                lock_guard<mutex>lk(m1);
                roundfinish=true;
                cv.notify_all();
            }

        }
        if(readstring.substr(0,3)==END)
        {
            close(client_sock);
            pthread_exit(NULL);

        }




    }

    return (void *)0;
}

#pragma clang diagnostic pop

//
// Created by krystian on 22.12.2019.
//

#ifndef SK_PROJ_SERVER_H
#define SK_PROJ_SERVER_H
#include <cstdio>
#include <unistd.h>
#include <sys/socket.h>
#include <cstdlib>
#include <netinet/in.h>
#include <string>
#include <cstring>
#include <arpa/inet.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <iterator>
#include <list>
#include <pthread.h>
using namespace std;

//Global definitions
#define PORT 8000
#define BUFFER_SIZE 500

//Commands
const string START = "START";
const string ANSWER = "AN";
void * client_handler(void * arg);
void * timer_handler(void * arg);
void sendtoall(string message);
int iscountry(string word, char letter);
int iscity(string word, char letter);
int isplant(string word, char letter);
int isanimal(string word, char letter);
int isname(string word, char letter);



#endif //SK_PROJ_SERVER_H

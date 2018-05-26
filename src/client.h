#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <iostream>

#include <cstdio>
#include <cstdlib>

#define MAX_NAME_LENGTH         32

#define CLIENT_BUFFER_SIZE      256

using namespace std;

class Client {
public:
    char *name;
    int id;
    int sock;
    int fd[2];
    char buffer[CLIENT_BUFFER_SIZE];
    unsigned int offset;

public:
    Client();
    ~Client();
    void setName(const char *name);
    void setId(int id);

    void process();
};

#endif

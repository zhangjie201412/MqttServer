#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <iostream>

#include <cstdio>
#include <cstdlib>

#define MAX_NAME_LENGTH         32

using namespace std;

class Client {
public:
    char *name;
    int id;
    int sock;
    int fd[2];

public:
    Client();
    ~Client();
    void setName(const char *name);
    void setId(int id);
};

#endif

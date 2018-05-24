#ifndef __MQTT_SERVER_H__
#define __MQTT_SERVER_H__

#include <iostream>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include "client.h"
#include "thread.h"

#define PORT            8989
#define MAXEVENT        24
#define MAX_LISTENER    2048

using namespace std;

class MqttServer {

public:
    MqttServer();
    void acceptAndDispatch();
    static void *handleClient(void *args);
    static void *wdgThreadProc(void *args);

private:
    static vector<Client> clients;
    int serverSock;
    int clientSock;
    struct sockaddr_in serverAddr;
    struct sockaddr_in clientAddr;

private:
    static int findClientIndex(Client *client);
    int index;
    Thread *wdgThread;
};

#endif

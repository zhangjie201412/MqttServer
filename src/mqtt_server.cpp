#include "mqtt_server.h"

#include <sys/select.h>
#include <sys/wait.h>
#include <sys/epoll.h>
#include <errno.h>
#include "common.h"

using namespace std;

vector<Client> MqttServer::clients;

MqttServer::MqttServer()
{
    Thread::initMutex();

    int yes = 1;
    serverSock = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serverAddr, 0, sizeof(sockaddr_in));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    setsockopt(serverSock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    if(bind(serverSock, (struct sockaddr *)&serverAddr,
                sizeof(sockaddr_in)) < 0) {
        cerr << "Failed to bind" << endl;
    }

    listen(serverSock, MAX_LISTENER);
    index = 0;
    wdgThread = new Thread();
    wdgThread->create((void *)MqttServer::wdgThreadProc, this);
}

void *MqttServer::wdgThreadProc(void *args)
{
    MqttServer *self = (MqttServer *)args;

    while(true) {
        usleep(10 * 1000000);
        for(size_t i = 0; i < clients.size(); i++) {
            Client *client = &MqttServer::clients[i];
            int fd = client->fd[0];
            write(fd, "X", 1);
        }
    }

    return NULL;
}

void MqttServer::acceptAndDispatch()
{
    Client *client;
    Thread *thread;

    socklen_t cliSize = sizeof(sockaddr_in);

    while(true) {
        client = new Client();
        thread = new Thread();
        client->sock = accept(serverSock, (struct sockaddr *)&clientAddr, &cliSize);
        if(client->sock < 0) {
            cerr << "Error on accept" << endl;
        } else {
            Thread::lockMutex();
            cout << "Client " << this->index << " joining" << endl;
            //create socketpair
            int rc = socketpair(AF_UNIX, SOCK_STREAM, 0, client->fd);
            if(rc < 0) {
                cerr << "Failed to create socketpair " << strerror(errno) << endl;
                delete client;
                delete thread;
                Thread::unlockMutex();
                continue;
            }
            client->setId(this->index);
            memset(client->buffer, 0x00, CLIENT_BUFFER_SIZE);
            client->offset = 0;
            this->index ++;
            Thread::unlockMutex();
            thread->create((void *)MqttServer::handleClient, client);
        }
    }
}

void *MqttServer::handleClient(void *args)
{
    Client *client = (Client *)args;
    int n;
    int index;
    int efd;
    struct epoll_event event;
    struct epoll_event *events;
    int rc;

    MqttServer::clients.push_back(*client);

    efd = epoll_create(8);
    if(efd < 0) {
        cerr << "Failed to create epoll " << strerror(errno) << endl;
        close(client->sock);
        return NULL;
    }

    event.data.fd = client->sock;
    event.events = EPOLLIN | EPOLLET;
    rc = epoll_ctl(efd, EPOLL_CTL_ADD, client->sock, &event);
    if(rc < 0) {
        cerr << "Failed to add client sock " << strerror(errno) << endl;
        close(client->sock);
        return NULL;
    }

    event.data.fd = client->fd[1];
    event.events = EPOLLIN | EPOLLET;
    rc = epoll_ctl(efd, EPOLL_CTL_ADD, client->fd[1], &event);
    if(rc < 0) {
        cerr << "Failed to add fd[1]  " << strerror(errno) << endl;
        close(client->sock);
        return NULL;
    }

    events = (struct epoll_event *)calloc(MAXEVENT, sizeof(struct epoll_event));

    while(true) {
        int event_num, i;

        event_num = epoll_wait(efd, events, MAXEVENT, -1);
        for(i = 0; i < event_num; i++) {
            if((events[i].events & EPOLLERR) ||
                    (events[i].events & EPOLLHUP) ||
                    (!(events[i].events & EPOLLIN))) {
                cerr << "Client " << client->name << "disconnected" << endl;
                close(client->sock);
                //remove client in static clients
                Thread::lockMutex();
                index = MqttServer::findClientIndex(client);
                if(index >= 0) {
                    cout << "Remove position " << index << " whose name is: " << MqttServer::clients[index].name <<endl;
                    MqttServer::clients.erase(MqttServer::clients.begin() + index);
                }
                Thread::unlockMutex();
                break;
            } else if(client->sock == events[i].data.fd) {
                n = read(client->sock, client->buffer + client->offset,
                        CLIENT_BUFFER_SIZE);
                if(n == 0) {
                    cout << "Client " << client->name << " disconnected" << endl;
                    close(client->sock);
                    //remove client in static clients
                    Thread::lockMutex();
                    index = MqttServer::findClientIndex(client);
                    if(index >= 0) {
                        cout << "Remove position " << index << " whose name is: " << MqttServer::clients[index].name <<endl;
                        MqttServer::clients.erase(MqttServer::clients.begin() + index);
                    }
                    Thread::unlockMutex();
                    break;
                } else if(n < 0 && errno == EAGAIN) {
                    cerr << "Error while receiving message from client: " <<
                        client->name << ": " << strerror(errno) << endl;
                } else {
                    client->offset += n;
                    if(client->buffer[client->offset - 1] == '\n') {
                        //handle the recv message
                        cout << "Recv " << n << " bytes: " << client->buffer << endl;
#ifdef DEBUG_DUMP_RAW
                        for(unsigned int i = 0; i < client->offset; i++) {
                            printf("%02x ", client->buffer[i]);
                        }
                        cout << endl;
#endif
                        client->process();
                        memset(client->buffer, 0x00, CLIENT_BUFFER_SIZE);
                        client->offset = 0;
                    }
                }
            } else if(client->fd[1] == events[i].data.fd) {
                char x;
                read(client->fd[1], &x, 1);
                if(x == 'X') {
                    //check something
                }
            }
        }
    }

    return NULL;
}

int MqttServer::findClientIndex(Client *client)
{
    for(size_t i = 0; i < clients.size(); i++) {
        if(MqttServer::clients[i].id == client->id) {
            return (int)i;
        }
    }

    cerr << "Client id not found." << endl;
    return -1;
}

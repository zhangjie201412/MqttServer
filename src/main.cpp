#include <iostream>
#include "thread.h"
#include "mqtt_server.h"

using namespace std;

int main(void)
{
    MqttServer *server;
    server = new MqttServer();
    server->acceptAndDispatch();

    return 0;
}

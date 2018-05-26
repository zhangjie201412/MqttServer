#include "client.h"

#include <vector>
#include <string.h>
#include "common.h"

using namespace std;

Client::Client()
{
    name = (char *)malloc(MAX_NAME_LENGTH + 1);
    id = 0;
}

Client::~Client()
{
    if(name) {
        free(name);
        name = NULL;
    }
}

void Client::setName(const char *name)
{
    snprintf(this->name, MAX_NAME_LENGTH + 1, "%s", name);
}

void Client::setId(int id)
{
    this->id = id;
}

void Client::process()
{
    char *buf = this->buffer;
    char topic[256];
    char message[256];
    std::string input(buf);
    std::vector<std::string> outputVec;

    outputVec = split(input, ',');
#ifdef DEBUG_DUMP_RAW
    for(unsigned int i = 0; i < outputVec.size(); i++) {
        cout << "[" << i <<"] - " << outputVec[i] << endl;
    }
#endif
    if(outputVec.size() != 4) {
        cerr << "cannot process invalid buffer" << endl;
        return;
    }
    //get id
    if(outputVec[0].compare(0, 2, ">>") == 0) {
        memcpy(this->name, outputVec[0].c_str() + 2, outputVec[0].length());
    }

    memcpy(topic, outputVec[1].c_str(), CLIENT_BUFFER_SIZE);
    memcpy(message, outputVec[2].c_str(), CLIENT_BUFFER_SIZE);
    //TODO: skip cs
    //TODO send topic & message to mqtt server
    cout << this->name << ", " << topic << ", " << message << endl;
    return;
}

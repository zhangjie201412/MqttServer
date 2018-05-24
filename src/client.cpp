#include "client.h"

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

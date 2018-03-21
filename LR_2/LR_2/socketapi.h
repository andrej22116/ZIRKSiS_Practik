#ifndef SOCKETAPI_H
#define SOCKETAPI_H

#ifdef __linux__

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#elif _WIN32

#include <winsock2.h>

#endif

#endif // SOCKETAPI_H

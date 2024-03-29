#ifndef NET_H
#define NET_H

#include "client.h"

#define NET_DEFAULT_SERVER "41.216.182.2"
#define NET_PORT_MAINNET 62443
#define NET_PORT_TESTNET 61338

int NET_ConnectSocketToServer(cli_t *socket, char *server, int port);

int NET_CloseSocket(cli_t *socket);

// detects if socket is recieving data
int NET_DetectSignal(cli_t *restrict socket);
int NET_HandlePacket(cli_t *socket, char *buffer);

// if socket is okay, recieve it of maximum n_bytes
int NET_RecvPacket(cli_t *socket, char *buffer, size_t n_bytes);

// if socket is okay, sends it of maximum n_bytes
int NET_SendPacket(cli_t *socket, char *buffer, size_t n_bytes);

/*
int NET_CreateBroadcastServer(void);
void NET_CloseConnection(void);
*/

#endif

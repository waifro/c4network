#ifndef NET_UTILS_H
#define NET_UTILS_H

#define cli_t int

#include <time.h>
#include "../pp4m/pp4m_net.h"

#define MAX_CLIENTS 2
#define MAX_LOBBY MAX_CLIENTS/2

typedef enum {
    LB_AVAIL = 0,
    LB_FULL = 1,
    LB_BUSY = 2,
    LB_ERROR = 3,
    LB_BLOCKED = 4
} LOBBY_STATUS;

// room of two clients
typedef struct {
    cli_t *cli_a, *cli_b;
} pair_cli_t;

typedef struct {
    pair_cli_t pair;
	
	clock_t clock_a, clock_b; 	// saves clock passed
	cli_t utimer;	// determines which player is under timer
	clock_t timestamp; // initialization of lobby
	
    LOBBY_STATUS status;
} net_lobby;

int retrieve_code(char *buffer);
int verify_code(int code);

int verify_socket(cli_t *socket);

int verify_mesg(char *buffer);
int verify_len_mesg(char *buffer);
int verify_mesg_isdigit(char *buffer);

int generate_val(int max);
void init_client_list(cli_t *client_list, int max);
void init_lobby_list(net_lobby *lobby_list, int max);

int client_accept(int master_socket, struct sockaddr_in *addr);
int client_disconnect(cli_t *client_list, int index, net_lobby *lobby, struct sockaddr_in *addr, int *connected, int max_clients);

#endif

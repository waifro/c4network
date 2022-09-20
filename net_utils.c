#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "client.h"
#include "lobby.h"
#include "../pp4m/pp4m_net.h"

int retrieve_code(char *mesg) {

    int code = 0;
    sscanf(mesg, "%d", &code);

    return code;
}

int verify_mesg_recv(char *mesg) {

    if (strlen(mesg) < 3) return -1;
    else if (strlen(mesg) > 255) return -1;

    return 0;
}

int generate_val(int max) {
    //srand(time(NULL));
    return (rand() % max);
}

void init_client_list(cli_t *client_list, int max) {
    for (int i = 0; i < max; i++) {
        client_list[i] = 0;
    }
}

void init_lobby_list(net_lobby *lobby_list, int max) {
	for (int i = 0; i < max; i++)
		lobby_updateroom_reset(lobby_list, i); // used just for reference
	
	return;
}

int client_accept(int master_socket, struct sockaddr_in *addr) {
    cli_t new_client = -1;
    socklen_t addr_size = sizeof(struct sockaddr);
    new_client = accept(master_socket, (struct sockaddr*)addr, &addr_size);
    return new_client;
}

int client_disconnect(cli_t *client_list, int index, net_lobby *lobby, sockaddr_in *addr, int *connected, int max_clients) {
	
	socklen_t addr_size = sizeof(struct sockaddr);
	getpeername(client_list[index], (struct sockaddr*)addr, &addr_size);

	printf("client discnct: %s:%d\t[%d of %d] | ", inet_ntoa(addr->sin_addr), htons(addr->sin_port), (*connected -= 1), max_clients);

    int room = lobby_updateroom_cli_left(lobby, client_list[index]);
    client_list[index] = 0;

    if (room >= MAX_LOBBY) printf("\n");
    else printf("roomId %d[%p:%p]\n", room, lobby[room].pair.cli_a, lobby[room].pair.cli_b);
	
	return 0;
}


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "client.h"
#include "lobby.h"
#include "../pp4m/pp4m_net.h"

int retrieve_code(char *buffer) {

    int code = 0;
    sscanf(buffer, "%d %*s", &code);

    return code;
}

int verify_code(int code) {
	
	if (code > 100 && code < 400)
		return 1;
	
	return -1;
}

int verify_socket(cli_t *socket) {
	
	if (socket == NULL)
		return -1;
		
	if (*socket == 0)
		return -2;

	return 1;
}

int verify_mesg(char *buffer) {
	
	// first, check if is at least long 3 characters and less then 256
	if (verify_len_mesg(buffer) != 1)
		return -1;
	
	// verify the messages contains at least few digits
	if (verify_mesg_isdigit(buffer) != 1)
		return -2;
	
	// verify the message is an actual code
	if (verify_code(retrieve_code(buffer)) != 1)
		return -3;
	
	return 1;
}

int verify_len_mesg(char *buffer) {
	if (buffer == NULL) return -1;
	
    if (strlen(buffer) < 3) return -2;
    else if (strlen(buffer) > 255) return -3;

    return 1;
}

int verify_mesg_isdigit(char *buffer) {
	
	for(int i = 0; i < 3; i++)
		if (isdigit(buffer[i]) != 0)
			return -1;
	
	return 1;
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

int client_disconnect(cli_t *client_list, int index, net_lobby *lobby, struct sockaddr_in *addr, int *connected, int max_clients) {
	
	socklen_t addr_size = sizeof(struct sockaddr);
	getpeername(client_list[index], (struct sockaddr*)addr, &addr_size);

	printf("client discnct: %s:%d\t[%d of %d] | ", inet_ntoa(addr->sin_addr), htons(addr->sin_port), (*connected -= 1), max_clients);

    int room = lobby_updateroom_cli_left(lobby, &client_list[index]);
    client_list[index] = 0;

    if (room >= MAX_LOBBY) printf("\n");
    else printf("roomId %d[%p:%p]\n", room, lobby[room].pair.cli_a, lobby[room].pair.cli_b);
	
	return 0;
}


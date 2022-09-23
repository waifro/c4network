#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lobby.h"
#include "server.h"
#include "client.h"
#include "net.h"
#include "net_utils.h"

#include "../pp4m/pp4m.h"
#include "../pp4m/pp4m_net.h"

int lobby_checkroom_avail(net_lobby *lobby, int room) {
    if (lobby[room].status == LB_AVAIL) {
        if (lobby[room].pair.cli_a == NULL) return 1;
        else if (lobby[room].pair.cli_b == NULL) return 2;
    }

    return -1;
}

int lobby_checkroom_isfull(net_lobby *lobby, int room) {
    if (lobby[room].status == LB_AVAIL)
        if (lobby[room].pair.cli_a != NULL && lobby[room].pair.cli_b != NULL) return 1;

    return -1;
}

int lobby_checkroom_isready(net_lobby *lobby, int room) {
    if (lobby[room].status == LB_FULL) return 1;
    else return -1;
}

int lobby_checkroom_isbusy(net_lobby *lobby, int room) {
    if (lobby[room].status == LB_BUSY) return 1;
    else return -1;
}

int lobby_checkroom_iserror(net_lobby *lobby, int room) {
    if (lobby[room].status == LB_ERROR) return 1;
    else return -1;
}

int lobby_checkroom_isblocked(net_lobby *lobby, int room) {
    if (lobby[room].status == LB_BLOCKED) return 1;
    else return -1;
}

int lobby_checkroom_endcycle(net_lobby *lobby, int room) {
	if (lobby[room].status == LB_BUSY || lobby[room].status == LB_ERROR) {
		if (lobby[room].pair.cli_a == NULL && lobby[room].pair.cli_b == NULL) return 1;
	}
	
	return -1;
}

int lobby_checkroom_cli(net_lobby *lobby, cli_t *client, int room) {
    if (lobby[room].pair.cli_a == client) return 1;
    else if (lobby[room].pair.cli_b == client) return 2;
    else return -1;
}

int lobby_assign_cli(net_lobby *lobby, cli_t *client) {
    int result = -1;

    for (int i = 0; i < MAX_LOBBY; i++) {
        result = lobby_checkroom_avail(lobby, i);
        if (result == -1) continue;

        // assign the lobby
        if (result == 1) lobby[i].pair.cli_a = client;
        else if (result == 2) lobby[i].pair.cli_b = client;

        // target the lobby
        if (lobby_checkroom_isfull(lobby, i) == 1) lobby[i].status = LB_FULL;
		
		char buffer[256];
		result = sv_redirect_svcode_STATE(SV_STATE_CONFIRM, NULL, client, i, buffer);
		
		if (result != 1)
			printf("lobby_assign_cli: error redirect svcode\n");
		
        break;
    }

    return 0;
}

int lobby_random_start(net_lobby *lobby, int room, char *fen) {
    int result = -1;
	
	cli_t foo[2];
	if (generate_val(100) > 50){ 
		foo[0] = *lobby[room].pair.cli_a;
		foo[1] = *lobby[room].pair.cli_b;
	} else {
		foo[0] = *lobby[room].pair.cli_b;
		foo[1] = *lobby[room].pair.cli_a;
	}
	
    char buf[256];
    sprintf(buf, "%d w %s", SV_LOBBY_POST_INIT, fen);
	
	for (int n = 0; n < 2; n++) {
		result = send(foo[n], buf, strlen(buf) + 1, 0);
		
		buf[4] = 'b';
		
		if (result == -1) {
			perror("lobby_random_start");
			return -1;
		}
	}
	
	lobby[room].utimer = foo[0];
	lobby[room].timestamp = clock();
	
    lobby[room].status = LB_BUSY;
	
    return result;
}

int lobby_updateroom_cli_left(net_lobby *lobby, cli_t *client) {
    int result = -1;
    int room = -1;
	
	// we end up here in case the client left forcely the server
	
    for (room = 0; room < MAX_LOBBY; room++) {
        result = lobby_checkroom_cli(lobby, client, room);
        if (result == -1) continue;

        if (result == 1) lobby[room].pair.cli_a = NULL;
        else if (result == 2) lobby[room].pair.cli_b = NULL;

        lobby[room].status = LB_ERROR;

        break;
    }

    return room;
}

int lobby_updateroom_reset(net_lobby *lobby, int room) {
	
	lobby[room].pair.cli_a = NULL;
    lobby[room].pair.cli_b = NULL;
	lobby[room].clock_a = 0;
	lobby[room].clock_b = 0;
	lobby[room].utimer = 0;
	lobby[room].timestamp = 0;
    lobby[room].status = LB_AVAIL;
	
	return 0;
}

int lobby_SV_POST_LOBBY_MOVE(net_lobby *lobby, cli_t *client, int room, char *buffer) {
    int result = -1;
	
    // overwrite old code
    for (int i = 0; i < 3; i++)
        buffer[i] = '0' + pp4m_p_int_index(SV_LOBBY_POST_MOVE, i);

    // send new message
    result = lobby_redirect_buf(lobby, client, room, buffer);
	
	// modify utimer to set other client and update clock
	if (lobby[room].utimer == *lobby[room].pair.cli_a) {
		
		lobby[room].utimer = *lobby[room].pair.cli_b;
		lobby[room].clock_a += (clock() - lobby[room].timestamp) - lobby[room].clock_b - lobby[room].clock_a;
	} else {
		lobby[room].utimer = *lobby[room].pair.cli_a;
		lobby[room].clock_b += (clock() - lobby[room].timestamp) - lobby[room].clock_a - lobby[room].clock_b;
	}
	
    return result;
}

int lobby_SV_POST_LOBBY_MESG(net_lobby *lobby, cli_t *client, int room, char *buffer) {
    int result = -1;

    // overwrite old code
    for (int i = 0; i < 3; i++)
        buffer[i] = '0' + pp4m_p_int_index(SV_LOBBY_POST_MESG, i);

    // send new message
    result = lobby_redirect_buf(lobby, client, room, buffer);

    return result;
}

int lobby_SV_LOBBY_POST_LEAVE(net_lobby *lobby, cli_t *client, int room) {
	int result = -1;
	
	// send command code that he left the game
	char buffer[256];
	snprintf(buffer, 255, "%d", SV_LOBBY_POST_PARTNER_LEFT);
    result = lobby_redirect_buf(lobby, client, room, buffer);
	
	if (result == -1)
		printf("lobby_SV_LOBBY_POST_LEAVE error [%p:%p]\n", lobby[room].pair.cli_a, lobby[room].pair.cli_b);
	
	result = lobby_checkroom_cli(lobby, client, room);
    if (result != -1) {
    	
    	 if (result == 1) lobby[room].pair.cli_a = NULL;
    	else if (result == 2) lobby[room].pair.cli_b = NULL;

    	lobby[room].status = LB_ERROR;
    	
    }
	
	return result;
}

int lobby_SV_POST_LOBBY_TIME(net_lobby *lobby, int room) {
    int result = -1;
	
	char buf[256];
	
	if (lobby[room].utimer == *lobby[room].pair.cli_a)
		sprintf(buf, "%d %ld", SV_LOBBY_POST_TIME, lobby[room].clock_a);
	else sprintf(buf, "%d %ld", SV_LOBBY_POST_TIME, lobby[room].clock_b);
	
	result = lobby_spam_buf(lobby, room, buf);
	if (result == -1) perror("lobby_SV_POST_LOBBY_TIME");
	
	return result;
}

int lobby_spam_buf(net_lobby *lobby, int room, char *buffer) {
	int result = -1;
	
    result = NET_SendPacket(lobby[room].pair.cli_a, buffer, strlen(buffer) + 1);
	
	if (result == -1)
		perror("lobby_spam_buf 1");
	
    result = NET_SendPacket(lobby[room].pair.cli_b, buffer, strlen(buffer) + 1);
	
	if (result == -1)
		perror("lobby_spam_buf 2");
	
    return result;
}

int lobby_redirect_buf(net_lobby *lobby, cli_t *client, int room, char *buffer) {
    int result = -1;

    result = lobby_checkroom_cli(lobby, client, room);
    if (result == -1) return -2;
    else if (result == 1) result = NET_SendPacket(lobby[room].pair.cli_b, buffer, strlen(buffer) + 1);
    else if (result == 2) result = NET_SendPacket(lobby[room].pair.cli_a, buffer, strlen(buffer) + 1);

    return result;
}

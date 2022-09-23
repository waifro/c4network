#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "server.h"
#include "lobby.h"

#include "net.h"
#include "net_utils.h"
#include "../pp4m/pp4m.h"
#include "../pp4m/pp4m_net.h"

int sv_status_STATE(int code) {
    return (code > SV_STATE_START && code < SV_STATE_END ? 0 : -1);
}

int sv_status_REQ(int code) {
    return (code > SV_REQ_START && code < SV_REQ_END ? 0 : -1);
}

int sv_status_POST(int code) {
    return (code > SV_POST_START && code < SV_POST_END ? 0 : -1);
}

int sv_status_LOBBY(int code) {
    return ((code > SV_LOBBY_REQ_START && code < SV_LOBBY_REQ_END) || (code > SV_LOBBY_POST_START && code < SV_LOBBY_POST_END) ? 0 : -1);
}

int sv_status_LOBBY_REQ(int code) {
    return (code > SV_LOBBY_REQ_START && code < SV_LOBBY_REQ_END ? 0 : -1);
}

int sv_status_LOBBY_POST(int code) {
    return (code > SV_LOBBY_POST_START && code < SV_LOBBY_POST_END ? 0 : -1);
}





int sv_redirect_svcode_STATE(int code, net_lobby *lobby, cli_t *client, int room, char *buffer) {
    (void)code; (void)client; (void)room; (void)buffer; (void)lobby;
    int result = -1;
	
    switch(code) {
		case SV_STATE_IDLE:
			result = sv_SV_STATE_IDLE(buffer);
			break;
			
		case SV_STATE_CONFIRM:
			result = sv_SV_STATE_CONFIRM(buffer);
			break;
			
		case SV_STATE_DENY:
			result = sv_SV_STATE_DENY(buffer);
			break;
			
		case SV_STATE_ERROR:
			result = sv_SV_STATE_ERROR(buffer);
			break;
			
        default:
            break;
    }
	
	// should we need to send a SV_STATE_ERROR if an error occurred?
	result = NET_SendPacket(client, buffer, strlen(buffer)+1);
	
    return result;
}

int sv_redirect_svcode_REQ(int code, net_lobby *lobby, cli_t *client, int room, char *buffer) {
    (void)code; (void)client; (void)room; (void)buffer; (void)lobby;
    int result = -1;

    switch(code) {
        case SV_REQ_NICKNAME:
            break;

        default:
            break;
    }

    return result;
}

int sv_redirect_svcode_POST(int code, net_lobby *lobby, cli_t *client, int room, char *buffer) {
    (void)code; (void)client; (void)room; (void)buffer; (void)lobby;
    int result = -1;

    switch(code) {
        case SV_LOBBY_POST_INIT:
            result = lobby_random_start(lobby, room, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 0");
            break;

        default:
            break;
    }

    return result;
}

int sv_redirect_svcode_LOBBY_REQ(int code, net_lobby *lobby, cli_t *client, int room, char *buffer) {
    (void)code; (void)client; (void)room; (void)buffer; (void)lobby;
    int result = -1;

    switch(code) {
        default:
            break;
    }

    return result;
}

int sv_redirect_svcode_LOBBY_POST(int code, net_lobby *lobby, cli_t *client, int room, char *buffer) {
    (void)code; (void)client; (void)room; (void)buffer; (void)lobby;
    int result = -1;

    switch(code) {
		
		case SV_LOBBY_POST_TIME:
			result = lobby_SV_POST_LOBBY_TIME(lobby, room);
			break;
        default:
            break;
    }

    return result;
}







int sv_redirect_clcode_STATE(int code, net_lobby *lobby, cli_t *client, int room, char *buffer) {
    (void)code; (void)client; (void)room; (void)buffer; (void)lobby;
    int result = -1;

    switch(code) {

        default:
            break;
    }

    return result;
}

int sv_redirect_clcode_REQ(int code, net_lobby *lobby, cli_t *client, int room, char *buffer) {
    (void)code; (void)client; (void)room; (void)buffer; (void)lobby;
    int result = -1;

    switch(code) {
        case CL_REQ_ASSIGN_LOBBY:
            result = lobby_assign_cli(lobby, client);
            break;

        default:
            break;
    }

    return result;
}

int sv_redirect_clcode_POST(int code, net_lobby *lobby, cli_t *client, int room, char *buffer) {
    (void)code; (void)client; (void)room; (void)buffer; (void)lobby;
    int result = -1;

    switch(code) {

        default:
            break;
    }

    return result;
}

int sv_redirect_clcode_LOBBY_REQ(int code, net_lobby *lobby, cli_t *client, int room, char *buffer) {
    (void)code; (void)client; (void)room; (void)buffer; (void)lobby;
    int result = -1;

    switch(code) {

        case CL_LOBBY_REQ_NICKNAME:
            break;

        default:
            break;
    }

    return result;
}

int sv_redirect_clcode_LOBBY_POST(int code, net_lobby *lobby, cli_t *client, int room, char *buffer) {
    (void)code; (void)client; (void)room; (void)buffer; (void)lobby;
    int result = -1;

    switch(code) {
        case CL_LOBBY_POST_LEAVE:
            result = lobby_SV_LOBBY_POST_LEAVE(lobby, client, room);
            break;

        case CL_LOBBY_POST_MOVE:
            result = lobby_SV_POST_LOBBY_MOVE(lobby, client, room, buffer);
            break;

        case CL_LOBBY_POST_MESG:
            result = lobby_SV_POST_LOBBY_MESG(lobby, client, room, buffer);
            break;

        default:
            break;
    }

    return result;
}

// server: client sent a packet to the server
int sv_clcode_redirect(int code, net_lobby *lobby, cli_t *client, int room, char *buffer) {
    int result = 0;

    if (cl_status_STATE(code) == 0) result = 0; // im not sure what to do with this and cli_t.status
    else if (cl_status_REQ(code) == 0) result = sv_redirect_clcode_REQ(code, lobby, client, room, buffer);
    else if (cl_status_POST(code) == 0) result = sv_redirect_clcode_POST(code, lobby, client, room, buffer);
    else if (cl_status_LOBBY_REQ(code) == 0) result = sv_redirect_clcode_LOBBY_REQ(code, lobby, client, room, buffer);
    else if (cl_status_LOBBY_POST(code) == 0) result = sv_redirect_clcode_LOBBY_POST(code, lobby, client, room, buffer);

    return result;
}

int sv_SV_STATE_IDLE(char *buffer) {
	if (buffer == NULL)
		return -1;
	
	for (int i = 0; i < 3; i++)
		buffer[i] = '0' + pp4m_p_int_index(SV_STATE_IDLE, i);
	
	buffer[3] = '\0';
	
	return 1;
}

int sv_SV_STATE_CONFIRM(char *buffer) {
	if (buffer == NULL)
		return -1;
	
	for (int i = 0; i < 3; i++)
		buffer[i] = '0' + pp4m_p_int_index(SV_STATE_CONFIRM, i);
	
	buffer[3] = '\0';
	
	return 1;
}

int sv_SV_STATE_DENY(char *buffer) {
	if (buffer == NULL)
		return -1;
	
	for (int i = 0; i < 3; i++)
		buffer[i] = '0' + pp4m_p_int_index(SV_STATE_DENY, i);
	
	buffer[3] = '\0';
	
	return 1;
}

int sv_SV_STATE_ERROR(char *buffer) {
	if (buffer == NULL)
		return -1;
	
	for (int i = 0; i < 3; i++)
		buffer[i] = '0' + pp4m_p_int_index(SV_STATE_ERROR, i);
	
	buffer[3] = '\0';
	
	return 1;
}
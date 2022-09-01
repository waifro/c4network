#ifndef LOBBY_H
#define LOBBY_H

#include "net_utils.h"

/* on success, returns 1 or 2 either cli_a or cli_b is empty and the lobby is signed LB_AVAIL, otherwise returns -1 */
int lobby_checkroom_avail(net_lobby *lobby, int room);

/* on success, returns 1 if the lobby is still signed LB_AVAIL and both clients are full, otherwise returns -1 */
int lobby_checkroom_isfull(net_lobby *lobby, int room);

/* on success, returns 1 if lobby is signed LB_FULL, otherwise returns -1 */
int lobby_checkroom_isready(net_lobby *lobby, int room);

/* on success, returns 1 if lobby is signed LB_BUSY, otherwise returns -1 */
int lobby_checkroom_isbusy(net_lobby *lobby, int room);

/* on success, returns 1 if lobby is signed LB_ERROR, otherwise returns -1 */
int lobby_checkroom_iserror(net_lobby *lobby, int room);

/* on success, returns 1 if lobby is signed LB_BLOCKED, otherwise returns -1 */
int lobby_checkroom_isblocked(net_lobby *lobby, int room);

/* on success, returns 1 if lobby is empty and either LB_BUSY or LB_ERROR, otherwise returns -1 */
int lobby_checkroom_endcycle(net_lobby *lobby, int room);

/* on success, returns 1 or 2 if either cli_a or cli_b matches the client, otherwise returns -1 */
int lobby_checkroom_cli(net_lobby *lobby, cli_t *client, int room);

/* on success, assignes client into a lobby signed LB_AVAIL and updates the room if full */
int lobby_assign_cli(net_lobby *lobby, cli_t *client);

/* on success, initializes both client randomly and starts the game */
int lobby_random_start(net_lobby *lobby, int room, char *fen);

/* on success, emptys the appropriate client on the lobby and signs the lobby as LB_ERROR */
int lobby_updateroom_cli_left(net_lobby *lobby, cli_t *client);

/* resets a room by default */
int lobby_updateroom_reset(net_lobby *lobby, int room);

/* on success, updates old code by overwriting it | pr: lobby_redirect_buf() */
int lobby_SV_POST_LOBBY_MOVE(net_lobby *lobby, cli_t *client, int room, char *buffer);
int lobby_SV_POST_LOBBY_MESG(net_lobby *lobby, cli_t *client, int room, char *buffer);

/* on success, redirects packet to the other client inside lobby */
int lobby_redirect_buf(net_lobby *lobby, cli_t *client, int room, char *buffer);

#endif

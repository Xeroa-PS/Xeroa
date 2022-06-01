#ifndef __PLAYER_LOGIN_REQ_H_
#define __PLAYER_LOGIN_REQ_H_

#include "game/Kcp.h"
#include "util/BasePacket.h"

void HandlePlayerLoginReq(Kcp* client, BasePacket packet);

#endif //__PLAYER_LOGIN_REQ_H_
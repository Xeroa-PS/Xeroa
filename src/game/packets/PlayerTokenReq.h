#ifndef __PLAYER_TOKEN_REQ_
#define __PLAYER_TOKEN_REQ_

#include <game/Kcp.h>
#include "util/BasePacket.h"

#include <GetPlayerTokenReq.pb.h>
#include <GetPlayerTokenRsp.pb.h>

void HandlePlayerTokenReq(Kcp* client, BasePacket packet);

#endif //__PLAYER_TOKEN_REQ_
#ifndef __GET_PLAYER_SOCIAL_DETAIL_REQ_H
#define __GET_PLAYER_SOCIAL_DETAIL_REQ_H

#include "game/Kcp.h"
#include "util/BasePacket.h"

void HandleGetPlayerSocialDetailReq(Kcp* client, BasePacket packet);

#endif //__GET_PLAYER_SOCIAL_DETAIL_REQ_H
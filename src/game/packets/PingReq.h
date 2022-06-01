#ifndef __PING_REQ_H_
#define __PING_REQ_H_

#include "game/Kcp.h"
#include "util/BasePacket.h"

void HandlePingReq(Kcp* client, BasePacket packet);

#endif //__PING_REQ_H_
#pragma once
#include <3ds.h>
#include <3ds/services/iru.h>

// Initialize/exit IR system
void netInit(void);
void netExit(void);

// Send/receive packets over IR
int  netSendPacket(const void* data, u32 size);
int  netRecvPacket(void* data, u32 size);

// Select role: host or client
typedef enum {
    NET_MODE_NONE,
    NET_MODE_IR_HOST,
    NET_MODE_IR_CLIENT
} NetMode;

extern NetMode currentMode;

// Called when the player taps the touchscreen to select host/client
void netSelectModeFromTouch(touchPosition* touch);
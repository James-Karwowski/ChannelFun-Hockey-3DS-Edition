#include "net_ir.h"
#include <stdio.h>
#include <string.h>
#include <3ds/services/iru.h>

#define IRU_SHARED_MEM_SIZE 0x1000
static u32* irSharedMem = NULL;

NetMode currentMode = NET_MODE_NONE;

void netInit(void) {
    if (!irSharedMem) {
        irSharedMem = (u32*)memalign(0x1000, IRU_SHARED_MEM_SIZE);
    }
    Result r = iruInit(irSharedMem, IRU_SHARED_MEM_SIZE);
    if (R_FAILED(r)) {
        printf("IRU init failed: 0x%08lX\n", r);
    }
    IRU_EnableRecvEvent();
}

void netExit(void) {
    iruExit();
    if (irSharedMem) {
        free(irSharedMem);
        irSharedMem = NULL;
    }
}

int netSendPacket(const void* data, u32 size) {
    if (currentMode == NET_MODE_NONE) return -1;

    // IRU expects u8*, not const
    Result r = iruSendData((u8*)data, size, true);  
    if (R_FAILED(r)) {
        return -1;
    }
    return size;
}

int netRecvPacket(void* data, u32 size) {
    if (currentMode == NET_MODE_NONE) return -1;

    u32 transferred = 0;
    Result r = iruRecvData((u8*)data, size, 0 /* flag */, &transferred, false /* non-block */);
    if (R_FAILED(r) || transferred == 0) {
        return 0; // no data yet
    }
    return (int)transferred;
}

void netSelectModeFromTouch(touchPosition* touch) {
    if (!touch) return;

    if (touch->px < 160) {
        currentMode = NET_MODE_IR_CLIENT;
        printf("Selected IR Client\n");
    } else {
        currentMode = NET_MODE_IR_HOST;
        printf("Selected IR Host\n");
    }

    netInit();
}
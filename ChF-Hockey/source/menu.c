#include "menu.h"
#include <stdio.h>
#include <string.h>

static MenuState currentState = MENU_MAIN;
static Role selectedRole = ROLE_NONE;
static bool waitingForConfirm = false;

static touchPosition touch;

void menuInit() {
    currentState = MENU_MAIN;
    selectedRole = ROLE_NONE;
    waitingForConfirm = false;
}

void menuExit() {
    // nothing yet
}

MenuState menuGetState() {
    return currentState;
}

Role menuGetRole() {
    return selectedRole;
}

static bool touchInCircle(int cx, int cy, int r) {
    int dx = touch.px - cx;
    int dy = touch.py - cy;
    return (dx*dx + dy*dy) <= (r*r);
}

void menuUpdate() {
    hidScanInput();
    u32 kDown = hidKeysDown();
    u32 kHeld = hidKeysHeld();
    hidTouchRead(&touch);

    // --- MAIN MENU ---
    if (currentState == MENU_MAIN) {
        // Touch "PLAY" button
        if (kDown & KEY_TOUCH) {
            if (touchInCircle(160, 100, 40)) {
                currentState = MENU_GAME;
            }
        }

        // Check IR role selection (Host/Client)
        if ((kHeld & KEY_X) && (kDown & KEY_L)) {
            selectedRole = ROLE_HOST;
            currentState = MENU_ROLE_CONFIRM;
        } else if ((kHeld & KEY_X) && (kDown & KEY_R)) {
            selectedRole = ROLE_CLIENT;
            currentState = MENU_ROLE_CONFIRM;
        }
    }

    // --- ROLE CONFIRM MENU ---
    else if (currentState == MENU_ROLE_CONFIRM) {
        if (kDown & KEY_TOUCH) {
            // Confirm button rectangle
            if (touch.py > 40 && touch.py < 90) {
                // Confirm tapped
                currentState = MENU_GAME;
            }
            // Go Back button rectangle
            else if (touch.py > 120 && touch.py < 170) {
                currentState = MENU_MAIN;
                selectedRole = ROLE_NONE;
            }
        }
    }
}

// --- DRAWING FUNCTIONS ---
void menuDrawTop(gfxScreen_t screen) {
    gfxSetScreenFormat(screen, GSP_BGR8_OES);
    u8* fb = gfxGetFramebuffer(screen, GFX_LEFT, NULL, NULL);

    // clear screen
    memset(fb, 0, 240*400*3);

    if (currentState == MENU_MAIN) {
        printf("\x1b[2;12HPong 2: Hockey");
        printf("\x1b[4;12HCreated by JKTheDragon");
    }
    else if (currentState == MENU_ROLE_CONFIRM) {
        if (selectedRole == ROLE_HOST) {
            printf("\x1b[2;2HYou are going to be a HOST for an MIR session.");
        } else {
            printf("\x1b[2;2HYou are going to be a CLIENT for an MIR session.");
        }
        printf("\x1b[4;2HPlease Confirm.");
    }
}

void menuDrawBottom(gfxScreen_t screen) {
    gfxSetScreenFormat(screen, GSP_BGR8_OES);
    u8* fb = gfxGetFramebuffer(screen, GFX_LEFT, NULL, NULL);

    // clear
    memset(fb, 0, 240*320*3);

    if (currentState == MENU_MAIN) {
        // PLAY button (circle)
        printf("\x1b[8;14H[ PLAY ]");
    }
    else if (currentState == MENU_ROLE_CONFIRM) {
        printf("\x1b[6;10H[ Confirm ]");
        printf("\x1b[12;10H[ Go Back ]");
    }
}
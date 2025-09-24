//If the amount of errors is 2, these are likely #include issues, but the program will still compile and run fine.
//If the amount of errors is more than 2, these are likely syntax issues, and the program will not compile or run.

#include <3ds.h>
#include <citro2d.h>
#include <citro3d.h>
#include <stdio.h>
#include <time.h>

/*
static struct timespec puckAccelerationDelay = {
    .tv_sec = 0,
    .tv_nsec = 500000000 // 500 million nanoseconds (0.5 seconds)
};
*/

// citro2d targets
static C3D_RenderTarget* top;
static C3D_RenderTarget* bottom;

static float def1X = 180.0f;
static float def1Y = 100.0f;

static float def2X = 180.0f;
static float def2Y = 50.0f;

static float defG1X = 50.0f;
static float defG1Y = 50.0f;

static float defG2X = 50.0f;
static float defG2Y = 50.0f;

static float puckX = 200.0f;
static float puckY = 120.0f;
static float puckAcceleration = 1.0f;

// Forward declarations
void changeScreen(int menu);
void loadMenu(int menu);
void loadAssets(int menu, float player1X, float player1Y, float player2X, float player2Y, float goalie1X, float goalie1Y, float goalie2X, float goalie2Y, float puckX, float puckY);
void resetScreen();
void loadPlayerAssets(float player1X, float player1Y, float player2X, float player2Y, float goalie1X, float goalie1Y, float goalie2X, float goalie2Y, u32 player1Color, u32 player2Color);
void loadPuckAssets(float puckX, float puckY, float puckSizeX, float puckSizeY, u32 puckColor);

int main(int argc, char** argv) {
    // Init graphics + console
    gfxInitDefault();
    consoleInit(GFX_BOTTOM, NULL);

    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();

    // Top screen target
    top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
    bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);

    // Start on welcome screen
    loadMenu(0);
    int currentMenu = 0;

    while (aptMainLoop()) {
        hidScanInput();
        u32 kDown = hidKeysDown();

        if (kDown & KEY_START) break;

        // Handle menu logic
        if (currentMenu == 0) {
            if (kDown & KEY_A) {
                currentMenu = 2;
                changeScreen(currentMenu);
            } else if (kDown & KEY_B) {
                currentMenu = 3;
                changeScreen(currentMenu);
            }
        } else if (currentMenu == 2) {
            if (kDown & KEY_X) {
                currentMenu = 0;
                changeScreen(currentMenu);
            }
        } else if (currentMenu == 3) {
            if (kDown & KEY_X) {
                currentMenu = 0;
                changeScreen(currentMenu);
            }
        }

        // ---- TOP SCREEN GRAPHICS ----
        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
        C2D_TargetClear(top, C2D_Color32(0, 0, 0, 255));
        C2D_SceneBegin(top);

        if (currentMenu == 0) {
            // Draw a background rectangle on welcome
            C2D_DrawRectSolid(0.0f, 30.0f, 0.0f, 400.0f, 240.f, C2D_Color32(83, 100, 93, 255));
        } else if (currentMenu == 1) {
            // Draw a background rectangle on welcome
            C2D_DrawRectSolid(0.0f, 30.0f, 0.0f, 400.0f, 240.f, C2D_Color32(83, 150, 93, 255));
        } else if (currentMenu == 2) {
            // Draw a background rectangle on welcome
            changeScreen(2);
            loadAssets(2, def1X, def1Y, def2X, def2Y, defG1X, defG1Y, defG2X, defG2Y, puckX, puckY);
        } else if (currentMenu == 3) {
            // Draw a background rectangle on welcome
            changeScreen(3);
            loadAssets(3, def1X, def1Y, def2X, def2Y, defG1X, defG1Y, defG2X, defG2Y, puckX, puckY);
        }

        C3D_FrameEnd(0);
    }

    // Cleanup
    C2D_Fini();
    C3D_Fini();
    gfxExit();
    return 0;
}

void loadAssets(int menu, float player1X, float player1Y, float player2X, float player2Y, float goalie1X, float goalie1Y, float goalie2X, float goalie2Y, float puckX, float puckY) {
    // Placeholder for graphics assets depending on menu
    //(void)menu;
    u32 kDown = hidKeysDown();
    int collisionCnt = 0;
    bool hitCollision = false;

    if(menu == 2) {
        // Draw players and puck        
        C2D_DrawRectSolid(player1X, player1Y, 0.0f, 40.0f, 20.0f, C2D_Color32(255, 0, 0, 255)); // Player 1 (Red)
        C2D_DrawRectSolid(player2X, player2Y, 0.0f, 40.0f, 20.0f, C2D_Color32(0, 0, 255, 255)); // Player 2 (Blue)

        C2D_DrawRectSolid(goalie1X, goalie1Y, 0.0f, 40.0f, 20.0f, C2D_Color32(255, 0, 0, 255)); // Player 1 (Red)
        C2D_DrawRectSolid(goalie2X, goalie2Y, 0.0f, 40.0f, 20.0f, C2D_Color32(0, 0, 255, 255)); // Player 2 (Blue)

        C2D_DrawRectSolid(puckX, puckY, 0.0f, 10.0f, 10.0f, C2D_Color32(255, 255, 255, 255)); // Puck (White)
        while(!(kDown & KEY_X)) {
            if(hitCollision == false){
                puckX += 5.0f;
                if(((player1X + 20) > puckX && player1X < (puckX + 10)) && ((player1Y + 20) > puckY && player1Y < (puckY + 10)) || ((goalie1X + 20) > puckX && goalie1X < (puckX + 10)) && ((goalie1Y + 20) > puckY && goalie1Y < (puckY + 10)) || ((goalie2X + 20) > puckX && goalie2X < (puckX + 10)) && ((goalie2Y + 20) > puckY && goalie2Y < (puckY + 10)) || ((player2X + 20) > puckX && player2X < (puckX + 10)) && ((player2Y + 20) > puckY && player2Y < (puckY + 10))){
                    hitCollision = true;
                    puckX += 5.0f * puckAcceleration;
                    collisionCnt++;
                }
            }
            hidScanInput();
            u32 kDown = hidKeysDown();
            if (kDown & KEY_CPAD_UP) {
                player1Y -= 5.0f;
            }
            if (kDown & KEY_CPAD_DOWN) {
                player1Y += 5.0f;
            }
            if(kDown & KEY_CPAD_LEFT) {
                player1X -= 5.0f;
            }
            if(kDown & KEY_CPAD_RIGHT) {
                player1X += 5.0f;
            }
            if (kDown & KEY_L) {
                goalie1Y -= 5.0f;
            }
            if (kDown & KEY_R) {
                goalie1Y += 5.0f;
            }

            if((((player1X + 20) > puckX && player1X < (puckX + 10)) && ((player1Y + 20) > puckY && player1Y < (puckY + 10)) || ((goalie1X + 20) > puckX && goalie1X < (puckX + 10)) && ((goalie1Y + 20) > puckY && goalie1Y < (puckY + 10)) || ((goalie2X + 20) > puckX && goalie2X < (puckX + 10)) && ((goalie2Y + 20) > puckY && goalie2Y < (puckY + 10)) || ((player2X + 20) > puckX && player2X < (puckX + 10)) && ((player2Y + 20) > puckY && player2Y < (puckY + 10)))){
                hitCollision = true;
                collisionCnt++;
                puckX += 5.0f * puckAcceleration;
                puckY += 5.0f * puckAcceleration;
                puckAcceleration += 0.25f; // Increase acceleration each time the puck is hitcollision = true;
            }
        }
    }else if (menu == 3) {
        // Draw players and puck
        //loadPlayerAssets(player1X, player1Y, player2X, player2Y, goalie1X, goalie1Y, goalie2X, goalie2Y, 0.0f, 40.0f, 20.0f, C2D_Color32(255, 0, 0, 255), C2D_Color32(0, 0, 255, 255));
        /*
        C2D_DrawRectSolid(player1X, player1Y, 0.0f, 40.0f, 20.0f, C2D_Color32(255, 0, 0, 255)); // Player 1 (Red)
        C2D_DrawRectSolid(player2X, player2Y, 0.0f, 40.0f, 20.0f, C2D_Color32(0, 0, 255, 255)); // Player 2 (Blue)

        C2D_DrawRectSolid(goalie1X, goalie1Y, 0.0f, 40.0f, 20.0f, C2D_Color32(255, 0, 0, 255)); // Player 1 (Red)
        C2D_DrawRectSolid(goalie2X, goalie2Y, 0.0f, 40.0f, 20.0f, C2D_Color32(0, 0, 255, 255)); // Player 2 (Blue)
        */
        C2D_DrawRectSolid(puckX, puckY, 0.0f, 10.0f, 10.0f, C2D_Color32(255, 255, 255, 255)); // Puck (White)

        while(!(kDown & KEY_X)) {
            hidScanInput();
            u32 kDown = hidKeysDown();
            if (kDown & KEY_CPAD_UP) {
            player1Y -= 5.0f;
            }
            if (kDown & KEY_CPAD_DOWN) {
                player1Y += 5.0f;
            }
            if(kDown & KEY_CPAD_LEFT) {
                player1X -= 5.0f;
            }
            if(kDown & KEY_CPAD_RIGHT) {
                player1X += 5.0f;
            }
            if (kDown & KEY_DUP) {
                player2Y -= 5.0f;
            }
            if (kDown & KEY_DDOWN) {
                player2Y += 5.0f;
            }
            if(kDown & KEY_DLEFT) {
                player2X -= 5.0f;
            }
            if(kDown & KEY_DRIGHT) {
                player2X += 5.0f;
            }
            if(kDown & KEY_X){
                loadMenu(0);
            }

            if((((player1X + 20) > puckX && player1X < (puckX + 10)) && ((player1Y + 20) > puckY && player1Y < (puckY + 10)) || ((goalie1X + 20) > puckX && goalie1X < (puckX + 10)) && ((goalie1Y + 20) > puckY && goalie1Y < (puckY + 10)) || ((goalie2X + 20) > puckX && goalie2X < (puckX + 10)) && ((goalie2Y + 20) > puckY && goalie2Y < (puckY + 10)) || ((player2X + 20) > puckX && player2X < (puckX + 10)) && ((player2Y + 20) > puckY && player2Y < (puckY + 10)))){
                hitCollision = true;
                collisionCnt++;
                puckX += 5.0f * puckAcceleration;
                puckY += 5.0f * puckAcceleration;
                puckAcceleration += 0.25f; // Increase acceleration each time the puck is hitcollision = true;
            }
        }
    }
    return;
}

void loadMenu(int menu) {
    consoleClear();
    if (menu == 0) {
        printf("\nWelcome!\n\n"
               "Fairchild 'Hockey' (3DS Edition)\nVersion: 0.0.2\n\n"
               "Press A to begin Single-player Mode.\nPress B for Multi-player Mode.\nPress START to exit.\n\n");
    } else if (menu == 1) {
        printf("!DEBUG!\nMain Menu:\n"
               "A = Single-player\n"
               "B = Multi-player (stub)\n"
               "START = Go back\n");
    } else if (menu == 2) {
        printf("\nSingle-player Mode Loaded!\nPress X to go back.\n");
    } else if (menu == 3) {
        printf("\nMulti-player Mode (not implemented).\nPress X to go back.\n");
    }
}

void resetScreen() {
    consoleClear();
}

void changeScreen(int menu) {
    resetScreen();
    C2D_TargetClear(top, C2D_Color32(0, 0, 0, 255));
    C2D_SceneBegin(top);
    loadMenu(menu);
}

void loadPlayerAssets(float player1X, float player1Y, float player2X, float player2Y, float goalie1X, float goalie1Y, float goalie2X, float goalie2Y, u32 player1Color, u32 player2Color){
    C2D_DrawRectSolid(player1X, player1Y, 0.0f, 40.0f, 20.0f, player1Color); // Player 1
    C2D_DrawRectSolid(player2X, player2Y, 0.0f, 40.0f, 20.0f, player2Color); // Player 2

    C2D_DrawRectSolid(goalie1X, goalie1Y, 0.0f, 40.0f, 20.0f, C2D_Color32(255, 0, 0, 255)); // Player 1 (Red)
    C2D_DrawRectSolid(goalie2X, goalie2Y, 0.0f, 40.0f, 20.0f, C2D_Color32(0, 0, 255, 255)); // Player 2 (Blue)
}

void loadPuckAssets(float puckX, float puckY, float puckSizeX, float puckSizeY, u32 puckColor){
    C2D_DrawRectSolid(puckX, puckY, 0.0f, puckSizeX, puckSizeY, puckColor);
}
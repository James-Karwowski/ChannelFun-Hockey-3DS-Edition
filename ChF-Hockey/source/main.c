#include <3ds.h>
#include <citro2d.h>
#include <citro3d.h>
#include <stdio.h>

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

// Forward declarations
void changeScreen(int menu);
void loadMenu(int menu);
void loadAssets(int menu, float player1X, float player1Y, float player2X, float player2Y, float goalie1X, float goalie1Y, float goalie2X, float goalie2Y, float puckX, float puckY);
void resetScreen();

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
                currentMenu = 1;
                changeScreen(currentMenu);
            }
        } else if (currentMenu == 1) {
            if (kDown & KEY_A) {
                currentMenu = 2;
                changeScreen(currentMenu);
            } else if (kDown & KEY_B) {
                currentMenu = 3;
                changeScreen(currentMenu);
            } else if (kDown & KEY_START) {
                currentMenu = 0;
                changeScreen(currentMenu);
            }
        } else if (currentMenu == 2) {
            if (kDown & KEY_START) {
                currentMenu = 0;
                changeScreen(currentMenu);
            }
        } else if (currentMenu == 3) {
            if (kDown & KEY_START) {
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
        } else if (currentMenu == 3) {
            // Draw a background rectangle on welcome
            changeScreen(3);
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

    if(menu == 2) {
        // Draw players and puck        
        C2D_DrawRectSolid(player1X, player1Y, 0.0f, 40.0f, 20.0f, C2D_Color32(255, 0, 0, 255)); // Player 1 (Red)
        C2D_DrawRectSolid(player2X, player2Y, 0.0f, 40.0f, 20.0f, C2D_Color32(0, 0, 255, 255)); // Player 2 (Blue)

        C2D_DrawRectSolid(goalie1X, goalie1Y, 0.0f, 40.0f, 20.0f, C2D_Color32(255, 0, 0, 255)); // Player 1 (Red)
        C2D_DrawRectSolid(goalie2X, goalie2Y, 0.0f, 40.0f, 20.0f, C2D_Color32(0, 0, 255, 255)); // Player 2 (Blue)

        C2D_DrawRectSolid(puckX, puckY, 0.0f, 10.0f, 10.0f, C2D_Color32(255, 255, 255, 255)); // Puck (White)

        while((kDown & KEY_START)) {
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
        }
        
    }else if (menu == 3) {
        // Draw players and puck
        C2D_DrawRectSolid(player1X, player1Y, 0.0f, 40.0f, 20.0f, C2D_Color32(255, 0, 0, 255)); // Player 1 (Red)
        C2D_DrawRectSolid(player2X, player2Y, 0.0f, 40.0f, 20.0f, C2D_Color32(0, 0, 255, 255)); // Player 2 (Blue)

        C2D_DrawRectSolid(goalie1X, goalie1Y, 0.0f, 40.0f, 20.0f, C2D_Color32(255, 0, 0, 255)); // Player 1 (Red)
        C2D_DrawRectSolid(goalie2X, goalie2Y, 0.0f, 40.0f, 20.0f, C2D_Color32(0, 0, 255, 255)); // Player 2 (Blue)

        C2D_DrawRectSolid(puckX, puckY, 0.0f, 10.0f, 10.0f, C2D_Color32(255, 255, 255, 255)); // Puck (White)

        while((kDown & KEY_START)) {
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
            if(kDown & KEY_START){
                loadMenu(1);
            }
        }
        
    }
    return;
}

void loadMenu(int menu) {
    consoleClear();
    if (menu == 0) {
        printf("\nWelcome! Please review the info on this screen:\n\n"
               "Fairchild 'Hockey' (3DS Edition)\nVersion: 0.0.1\n\n"
               "Press A to go to the main menu.\nPress START to exit.\n\n");
    } else if (menu == 1) {
        printf("Main Menu:\n"
               "A = Single-player\n"
               "B = Multi-player (stub)\n"
               "START = Go back\n");
    } else if (menu == 2) {
        printf("\nSingle-player Mode Loaded!\nPress START to go back.\n");
    } else if (menu == 3) {
        printf("\nMulti-player Mode (not implemented).\nPress START to go back.\n");
    }
}

void resetScreen() {
    consoleClear();
}

void changeScreen(int menu) {
    resetScreen();
    C2D_TargetClear(top, C2D_Color32(0, 0, 0, 255));
    //C2D_SceneBegin(top);
    loadMenu(menu);
}
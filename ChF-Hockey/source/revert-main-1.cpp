//If the amount of errors is 2, these are likely #include issues, but the program will still compile and run fine.
//If the amount of errors is more than 2, these are likely syntax issues, and the program will not compile or run.

#include <3ds.h>
#include <citro2d.h>
#include <stdio.h>
#include <stdlib.h>

// Screen constants
#define SCREEN_WIDTH  400
#define SCREEN_HEIGHT 240

// Rink
#define RINK_WIDTH  380
#define RINK_HEIGHT 180

// Puck
#define PUCK_SIZE 16

typedef struct {
    float x, y;
    float dx, dy;
} Puck;

static C3D_RenderTarget* topTarget;
static Puck puck;

// Initialize puck in middle
void initPuck(void) {
    puck.x = (SCREEN_WIDTH - PUCK_SIZE) / 2.0f;
    puck.y = (SCREEN_HEIGHT - PUCK_SIZE) / 2.0f;
    puck.dx = 2.5f;
    puck.dy = 1.5f;
}

// Update puck with simple wall collisions
void updatePuck(void) {
    puck.x += puck.dx;
    puck.y += puck.dy;

    if (puck.x < 0) { puck.x = 0; puck.dx = -puck.dx; }
    if (puck.x + PUCK_SIZE > SCREEN_WIDTH) { puck.x = SCREEN_WIDTH - PUCK_SIZE; puck.dx = -puck.dx; }
    if (puck.y < 0) { puck.y = 0; puck.dy = -puck.dy; }
    if (puck.y + PUCK_SIZE > SCREEN_HEIGHT) { puck.y = SCREEN_HEIGHT - PUCK_SIZE; puck.dy = -puck.dy; }
}

// Render rink + puck
void renderTop(void) {
    C2D_TargetClear(topTarget, C2D_Color32(0, 0, 0, 255));
    C2D_SceneBegin(topTarget);

    // Center rink rectangle
    float rinkX = (SCREEN_WIDTH - RINK_WIDTH) / 2.0f;
    float rinkY = (SCREEN_HEIGHT - RINK_HEIGHT) / 2.0f;
    C2D_DrawRectSolid(rinkX, rinkY, 0.0f, RINK_WIDTH, RINK_HEIGHT, C2D_Color32(50, 100, 200, 255));

    // Puck
    C2D_DrawRectSolid(puck.x, puck.y, 0.0f, PUCK_SIZE, PUCK_SIZE, C2D_Color32(255, 255, 255, 255));
}

int main(int argc, char* argv[]) {
    // Init
    gfxInitDefault();
    consoleInit(GFX_BOTTOM, NULL);

    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();
    C2D_SceneSize(SCREEN_WIDTH, SCREEN_HEIGHT, false);

    topTarget = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
    if (!topTarget) {
        printf("Failed to create top screen target!\n");
        gfxExit();
        return -1;
    }

    initPuck();

    // Loop
    while (aptMainLoop()) {
        hidScanInput();
        u32 kDown = hidKeysDown();
        if (kDown & KEY_START) break;

        updatePuck();
        renderTop();

        // Show coords on bottom console
        printf("\x1b[1;1HPuck (%.1f, %.1f)   ", puck.x, puck.y);

        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    }

    // Cleanup
    C2D_Fini();
    gfxExit();
    return 0;
}
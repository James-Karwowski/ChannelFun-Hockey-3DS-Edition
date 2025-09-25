//If the amount of errors is 2, these are likely #include issues, but the program will still compile and run fine.
//If the amount of errors is more than 2, these are likely syntax issues, and the program will not compile or run.

#include <3ds.h>
#include <citro2d.h>
#include <citro3d.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>

// Screen size
#define SCREEN_WIDTH  400
#define SCREEN_HEIGHT 240

// Paddle
float paddleX = SCREEN_WIDTH / 2;
float paddleY = SCREEN_HEIGHT - 40;
float paddleW = 80.0f;
float paddleH = 16.0f;
float paddleAngle = 0.0f;

// Puck
float puckX = SCREEN_WIDTH / 2;
float puckY = SCREEN_HEIGHT / 2;
float puckVX = 0.0f;
float puckVY = 2.0f;
float puckRadius = 8.0f;

// Goal size (bottom border opening)
float goalWidth = 100.0f;

// -------------------- Drawing --------------------

void drawPaddle(float x, float y, float angle) {
    C2D_DrawRectSolid(
        x - paddleW / 2, y - paddleH / 2, 0,
        paddleW, paddleH,
        C2D_Color32(200, 50, 50, 255),
        angle, x, y   // rotation and pivot
    );
}

void drawPuck(float x, float y) {
    C2D_DrawCircleSolid(x, y, 0, puckRadius, C2D_Color32(50, 200, 50, 255));
}

void drawBorder() {
    u32 borderColor = C2D_Color32(255, 255, 255, 255);
    float thickness = 4.0f;

    // Top border
    C2D_DrawRectSolid(0, 0, 0, SCREEN_WIDTH, thickness, borderColor);

    // Left border
    C2D_DrawRectSolid(0, 0, 0, thickness, SCREEN_HEIGHT, borderColor);

    // Right border
    C2D_DrawRectSolid(SCREEN_WIDTH - thickness, 0, 0, thickness, SCREEN_HEIGHT, borderColor);

    // Bottom border with goal opening
    float goalX = (SCREEN_WIDTH - goalWidth) / 2;
    C2D_DrawRectSolid(0, SCREEN_HEIGHT - thickness, 0, goalX, thickness, borderColor);
    C2D_DrawRectSolid(goalX + goalWidth, SCREEN_HEIGHT - thickness, 0, SCREEN_WIDTH - (goalX + goalWidth), thickness, borderColor);
}

// -------------------- Game Logic --------------------

void updatePaddle() {
    hidScanInput();
    circlePosition circle;
    hidCircleRead(&circle);

    paddleX += circle.dx * 0.05f;
    paddleY += circle.dy * 0.05f;

    u32 kHeld = hidKeysHeld();
    if (kHeld & KEY_L) paddleAngle -= 0.05f;
    if (kHeld & KEY_R) paddleAngle += 0.05f;

    // Clamp
    if (paddleX < paddleW/2) paddleX = paddleW/2;
    if (paddleX > SCREEN_WIDTH - paddleW/2) paddleX = SCREEN_WIDTH - paddleW/2;
    if (paddleY < paddleH/2) paddleY = paddleH/2;
    if (paddleY > SCREEN_HEIGHT - paddleH/2) paddleY = SCREEN_HEIGHT - paddleH/2;
}

void updatePuck() {
    puckX += puckVX;
    puckY += puckVY;

    // Left/right wall
    if (puckX - puckRadius < 0 || puckX + puckRadius > SCREEN_WIDTH) puckVX = -puckVX;

    // Top wall
    if (puckY - puckRadius < 0) puckVY = -puckVY;

    // Bottom border (goal opening in middle)
    float goalX = (SCREEN_WIDTH - goalWidth) / 2;
    if (puckY + puckRadius > SCREEN_HEIGHT) {
        if (puckX < goalX || puckX > goalX + goalWidth) {
            puckVY = -fabs(puckVY); // bounce back
            puckY = SCREEN_HEIGHT - puckRadius - 1;
        } else {
            // GOAL scored
            puckX = SCREEN_WIDTH / 2;
            puckY = SCREEN_HEIGHT / 2;
            puckVX = 0;
            puckVY = 2;
        }
    }
}

void checkPaddleCollision() {
    if (puckX > paddleX - paddleW/2 && puckX < paddleX + paddleW/2 &&
        puckY + puckRadius > paddleY - paddleH/2 && puckY - puckRadius < paddleY + paddleH/2) {

        // Transform puck velocity into paddle local space
        float cosA = cosf(-paddleAngle);
        float sinA = sinf(-paddleAngle);

        float localVX = puckVX * cosA - puckVY * sinA;
        float localVY = puckVX * sinA + puckVY * cosA;

        localVY = -fabs(localVY); // bounce upwards relative to paddle

        puckVX = localVX * cosf(paddleAngle) - localVY * sinf(paddleAngle);
        puckVY = localVX * sinf(paddleAngle) + localVY * cosf(paddleAngle);

        puckY = paddleY - paddleH/2 - puckRadius - 1;
    }
}

// -------------------- Main --------------------

int main() {
    // Init libs
    gfxInitDefault();
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();

    // Create top screen render target
    C3D_RenderTarget* top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);

    while (aptMainLoop()) {
        hidScanInput();
        if (hidKeysDown() & KEY_START) break;

        updatePaddle();
        updatePuck();
        checkPaddleCollision();

        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
        C2D_TargetClear(top, C2D_Color32(0, 0, 0, 255));
        C2D_SceneBegin(top);

        drawBorder();
        drawPaddle(paddleX, paddleY, paddleAngle);
        drawPuck(puckX, puckY);

        C3D_FrameEnd(0);
    }

    C2D_Fini();
    C3D_Fini();
    gfxExit();
    return 0;
}
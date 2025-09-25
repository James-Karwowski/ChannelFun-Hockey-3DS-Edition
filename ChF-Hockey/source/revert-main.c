//If the amount of errors is 2, these are likely #include issues, but the program will still compile and run fine.
//If the amount of errors is more than 2, these are likely syntax issues, and the program will not compile or run.

#include <3ds.h>
#include <citro2d.h>
#include <citro3d.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>

// main.c
// Pong-style hockey demo with rotating paddles, puck physics, and rink borders.
// Uses citro2d + citro3d according to official docs.

// Screen constants
#define SCREEN_W 400
#define SCREEN_H 240

// Rink
#define RINK_W 400.0f
#define RINK_H 180.0f

// Gameplay geometry
#define PUCK_SIZE 12.0f
#define PADDLE_W 6.0f
#define PADDLE_H 40.0f

// Speeds
#define PUCK_SPEED_INIT 2.5f
#define PUCK_SPEED_BOOST 1.08f
#define PADDLE_SPEED 3.5f
#define AI_SPEED 2.0f
#define ROT_SPEED 0.05f

typedef struct {
    float x, y;
    float angle; // radians
} Paddle;

typedef struct {
    float x, y;
    float vx, vy;
    float size;
} Puck;

// Globals
static C3D_RenderTarget* top_target;
static C3D_RenderTarget* bot_target;

static Paddle left_paddle;
static Paddle right_paddle;
static Puck puck;

// Reset puck to center, moving right
static void reset_puck(void) {
    puck.size = PUCK_SIZE;
    puck.x = (SCREEN_W - puck.size) * 0.5f;
    puck.y = (SCREEN_H - puck.size) * 0.5f;
    float ang = ((rand() % 60) - 30) * (M_PI / 180.0f);
    puck.vx = cosf(ang) * PUCK_SPEED_INIT;
    puck.vy = sinf(ang) * PUCK_SPEED_INIT;
    if (puck.vx < 0) puck.vx = -puck.vx; // ensure toward right
}

// Init game objects
static void init_game(void) {
    left_paddle.x = 40;
    left_paddle.y = SCREEN_H * 0.5f;
    left_paddle.angle = 0;

    right_paddle.x = SCREEN_W - 40;
    right_paddle.y = SCREEN_H * 0.5f;
    right_paddle.angle = 0;

    reset_puck();
}

// Draw paddle as a thick line (rotatable)
static void drawPaddle(Paddle *p, u32 color) {
    float halfLen = PADDLE_W / 2.0f;
    float cosA = cosf(p->angle);
    float sinA = sinf(p->angle);

    float x1 = p->x - halfLen * cosA;
    float y1 = p->y - halfLen * sinA;
    float x2 = p->x + halfLen * cosA;
    float y2 = p->y + halfLen * sinA;

    C2D_DrawLine(x1, y1, color, x2, y2, color, PADDLE_H, 0);
}

// Handle paddle-puck collision (reflect using paddle angle)
static void handlePaddleCollision(Paddle *p) {
    float dx = puck.x + puck.size/2 - p->x;
    float dy = puck.y + puck.size/2 - p->y;
    float dist = sqrtf(dx*dx + dy*dy);

    if (dist < (PADDLE_W/2 + puck.size/2)) {
        // reflect puck based on paddle angle
        float nx = cosf(p->angle);
        float ny = sinf(p->angle);

        float dot = puck.vx*nx + puck.vy*ny;
        puck.vx -= 2*dot*nx;
        puck.vy -= 2*dot*ny;

        puck.vx *= PUCK_SPEED_BOOST;
        puck.vy *= PUCK_SPEED_BOOST;
    }
}

// Update puck (walls + paddles)
static void update_puck(void) {
    puck.x += puck.vx;
    puck.y += puck.vy;

    float rink_top = (SCREEN_H - RINK_H) * 0.5f;
    float rink_bottom = rink_top + RINK_H;
    float rink_left = (SCREEN_W - RINK_W) * 0.5f;
    float rink_right = rink_left + RINK_W;

    if (puck.y < rink_top) { puck.y = rink_top; puck.vy = -puck.vy; }
    if (puck.y + puck.size > rink_bottom) { puck.y = rink_bottom - puck.size; puck.vy = -puck.vy; }
    if (puck.x < rink_left) { puck.x = rink_left; puck.vx = -puck.vx; }
    if (puck.x + puck.size > rink_right) { puck.x = rink_right - puck.size; puck.vx = -puck.vx; }

    handlePaddleCollision(&left_paddle);
    handlePaddleCollision(&right_paddle);
}

// Simple AI
static void update_ai(void) {
    if (fabsf(puck.y - left_paddle.y) > 2.0f) {
        if (puck.y < left_paddle.y) left_paddle.y -= AI_SPEED;
        else left_paddle.y += AI_SPEED;
    }
}

// Draw rink + objects
static void draw_scene(void) {
    float rink_x = (SCREEN_W - RINK_W) * 0.5f;
    float rink_y = (SCREEN_H - RINK_H) * 0.5f;

    // Rink border
    C2D_DrawRectSolid(rink_x, rink_y, 0, RINK_W, RINK_H, C2D_Color32(0,0,0,255));
    C2D_DrawRectSolid(rink_x, rink_y, 0, RINK_W, 4, C2D_Color32(255,255,255,255));
    C2D_DrawRectSolid(rink_x, rink_y + RINK_H - 4, 0, RINK_W, 4, C2D_Color32(255,255,255,255));

    float goal_gap = 60;
    float gap_y = (SCREEN_H - goal_gap) * 0.5f;
    C2D_DrawRectSolid(rink_x, rink_y, 0, 4, gap_y - rink_y, C2D_Color32(255,255,255,255));
    C2D_DrawRectSolid(rink_x, gap_y + goal_gap, 0, 4, rink_y + RINK_H - (gap_y + goal_gap), C2D_Color32(255,255,255,255));
    C2D_DrawRectSolid(rink_x + RINK_W - 4, rink_y, 0, 4, gap_y - rink_y, C2D_Color32(255,255,255,255));
    C2D_DrawRectSolid(rink_x + RINK_W - 4, gap_y + goal_gap, 0, 4, rink_y + RINK_H - (gap_y + goal_gap), C2D_Color32(255,255,255,255));

    // Paddles
    drawPaddle(&left_paddle, C2D_Color32(0,200,255,255));
    drawPaddle(&right_paddle, C2D_Color32(0,200,0,255));

    // Puck
    C2D_DrawRectSolid(puck.x, puck.y, 0.2f, puck.size, puck.size, C2D_Color32(255,255,255,255));
}

int main(int argc, char** argv) {
    srand((unsigned int)svcGetSystemTick());

    gfxInitDefault();
    consoleInit(GFX_BOTTOM, NULL);
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();
    C2D_SceneSize(SCREEN_W, SCREEN_H, false);

    top_target = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
    bot_target = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);

    init_game();

    while (aptMainLoop()) {
        hidScanInput();
        u32 kDown = hidKeysDown();
        u32 kHeld = hidKeysHeld();

        if (kDown & KEY_START) break;
        if (kDown & KEY_A) reset_puck();

        circlePosition cp;
        hidCircleRead(&cp);
        right_paddle.x += (float)cp.dx / 427.68f * PADDLE_SPEED;
        right_paddle.y += (float)cp.dy / 427.68f * PADDLE_SPEED;

        if (kHeld & KEY_L) right_paddle.angle -= ROT_SPEED;
        if (kHeld & KEY_R) right_paddle.angle += ROT_SPEED;

        update_ai();
        update_puck();

        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
        C2D_TargetClear(top_target, C2D_Color32(0,0,0,255));
        C2D_SceneBegin(top_target);
        draw_scene();
        C3D_FrameEnd(0);

        gspWaitForVBlank();
    }

    C2D_Fini();
    C3D_Fini();
    gfxExit();
    return 0;
}
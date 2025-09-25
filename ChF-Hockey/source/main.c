//If the amount of errors is 2, these are likely #include issues, but the program will still compile and run fine.
//If the amount of errors is more than 2, these are likely syntax issues, and the program will not compile or run.

// main.c
// Clean citro2d / citro3d example supplying a rink, moving puck, player paddle, and basic collisions.
// Follows the recommended init / frame / cleanup flow from citro2d docs.

#include <3ds.h>
#include <citro2d.h>
#include <citro3d.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Screen constants
#define SCREEN_W 400
#define SCREEN_H 240

// Rink (drawn centered; width limited to screen width)
#define RINK_W 400.0f     // can't exceed 400 on top screen
#define RINK_H 180.0f

// Gameplay geometry
#define PUCK_SIZE 12.0f
#define PADDLE_W 8.0f
#define PADDLE_H 30.0f

// Speeds
#define PUCK_SPEED_INIT 2.5f
#define PUCK_SPEED_BOOST 1.08f
#define PADDLE_SPEED 3.5f
#define AI_SPEED 2.0f

typedef struct {
    float x, y;
    float w, h;
} Rect;

typedef struct {
    float x, y;
    float vx, vy;
    float size;
} Puck;

// Global render targets
static C3D_RenderTarget* top_target;
static C3D_RenderTarget* bot_target;

// Game objects
static Rect left_paddle;
static Rect right_paddle;
static Puck puck;

// Helper: reset puck to center and set initial velocity toward player (right)
static void reset_puck_towards_player(void) {
    puck.size = PUCK_SIZE;
    puck.x = (SCREEN_W - puck.size) * 0.5f;
    puck.y = (SCREEN_H - puck.size) * 0.5f;
    // initial velocity: toward right player with slight random vertical
    float ang = ((rand() % 60) - 30) * (M_PI / 180.0f);
    puck.vx = cosf(ang) * PUCK_SPEED_INIT;
    puck.vy = sinf(ang) * PUCK_SPEED_INIT;
    if (puck.vx < 0) puck.vx = -puck.vx; // ensure toward player (right)
}

// Initialize paddles and puck
static void init_game_objects(void) {
    // Place left paddle near left border, vertically centered
    left_paddle.w = PADDLE_W; left_paddle.h = PADDLE_H;
    left_paddle.x = 16.0f; left_paddle.y = (SCREEN_H - left_paddle.h) * 0.5f;

    // Right paddle (player) near right border
    right_paddle.w = PADDLE_W; right_paddle.h = PADDLE_H;
    right_paddle.x = SCREEN_W - 16.0f - right_paddle.w;
    right_paddle.y = (SCREEN_H - right_paddle.h) * 0.5f;

    reset_puck_towards_player();
}

// AABB collision test
static bool rects_overlap(float ax, float ay, float aw, float ah,
                          float bx, float by, float bw, float bh) {
    return !(ax + aw <= bx || bx + bw <= ax || ay + ah <= by || by + bh <= ay);
}

// Resolve puck-paddle collision: reflect horizontal velocity and slightly change vertical velocity
static void handle_paddle_collision(Rect *p, Puck *k) {
    // Simple resolution: push puck out and reflect vx
    if (k->vx > 0 && k->x + k->size > p->x) {
        // puck moving right and overlapping right paddle
        k->x = p->x - k->size - 0.5f;
        k->vx = -k->vx * PUCK_SPEED_BOOST;
    } else if (k->vx < 0 && k->x < p->x + p->w) {
        // puck moving left and overlapping left paddle
        k->x = p->x + p->w + 0.5f;
        k->vx = -k->vx * PUCK_SPEED_BOOST;
    }
    // Add vertical deflection based on where on the paddle the puck hit
    float paddleCenter = p->y + p->h * 0.5f;
    float rel = (k->y + k->size * 0.5f) - paddleCenter; // -h/2..h/2
    float norm = rel / (p->h * 0.5f); // -1..1
    k->vy += norm * 1.2f; // tweak vertical influence
}

// Update puck position & collision with walls and paddles
static void update_puck(void) {
    puck.x += puck.vx;
    puck.y += puck.vy;

    // Walls: top & bottom of rink area (rink centered vertically)
    float rink_top = (SCREEN_H - RINK_H) * 0.5f;
    float rink_bottom = rink_top + RINK_H;

    if (puck.y < rink_top) {
        puck.y = rink_top;
        puck.vy = -puck.vy;
    } else if (puck.y + puck.size > rink_bottom) {
        puck.y = rink_bottom - puck.size;
        puck.vy = -puck.vy;
    }

    // Left/right scoring / bounce on rink sides (we'll bounce off side walls for now)
    float rink_left = (SCREEN_W - RINK_W) * 0.5f;
    float rink_right = rink_left + RINK_W;

    if (puck.x < rink_left) {
        puck.x = rink_left;
        puck.vx = -puck.vx;
    } else if (puck.x + puck.size > rink_right) {
        puck.x = rink_right - puck.size;
        puck.vx = -puck.vx;
    }

    // Paddle collisions
    // left paddle
    if (rects_overlap(puck.x, puck.y, puck.size, puck.size,
                      left_paddle.x, left_paddle.y, left_paddle.w, left_paddle.h)) {
        handle_paddle_collision(&left_paddle, &puck);
    }
    // right paddle
    if (rects_overlap(puck.x, puck.y, puck.size, puck.size,
                      right_paddle.x, right_paddle.y, right_paddle.w, right_paddle.h)) {
        handle_paddle_collision(&right_paddle, &puck);
    }
}

// Simple AI: move left paddle toward puck Y
static void update_ai(void) {
    float paddleCenter = left_paddle.y + left_paddle.h * 0.5f;
    float puckCenter = puck.y + puck.size * 0.5f;
    if (fabsf(puckCenter - paddleCenter) > 2.0f) {
        if (puckCenter < paddleCenter) left_paddle.y -= AI_SPEED;
        else left_paddle.y += AI_SPEED;
    }
    // clamp to rink vertical bounds
    float rink_top = (SCREEN_H - RINK_H) * 0.5f;
    float rink_bottom = rink_top + RINK_H - left_paddle.h;
    if (left_paddle.y < rink_top) left_paddle.y = rink_top;
    if (left_paddle.y > rink_bottom) left_paddle.y = rink_bottom;
}

// Draw functions (top screen)
static void draw_rink_and_objects(void) {
    // Clear and begin scene on top target
    C2D_TargetClear(top_target, C2D_Color32(0, 0, 0, 255));
    C2D_SceneBegin(top_target);

    // Draw rink (centered)
    float rink_x = (SCREEN_W - RINK_W) * 0.5f;
    float rink_y = (SCREEN_H - RINK_H) * 0.5f;
    C2D_DrawRectSolid(rink_x, rink_y, 0.0f, RINK_W, RINK_H, C2D_Color32(8, 8, 8, 255));       // black border
    // inner white border 4px thick: draw inner rectangle slightly inset
    float inner_x = rink_x + 4.0f;
    float inner_y = rink_y + 4.0f;
    C2D_DrawRectSolid(inner_x, inner_y, 0.0f, RINK_W - 8.0f, RINK_H - 8.0f, C2D_Color32(0, 0, 0, 255));
    // border lines (white)
    // Top border line
    C2D_DrawRectSolid(rink_x, rink_y, 0.0f, RINK_W, 4.0f, C2D_Color32(255,255,255,255));
    // Bottom border
    C2D_DrawRectSolid(rink_x, rink_y + RINK_H - 4.0f, 0.0f, RINK_W, 4.0f, C2D_Color32(255,255,255,255));
    // Left border
    C2D_DrawRectSolid(rink_x, rink_y, 0.0f, 4.0f, RINK_H, C2D_Color32(255,255,255,255));
    // Right border
    C2D_DrawRectSolid(rink_x + RINK_W - 4.0f, rink_y, 0.0f, 4.0f, RINK_H, C2D_Color32(255,255,255,255));

    // center openings 40px height at horizontal center (gap in left/right borders)
    float gap_h = 40.0f;
    float gap_y = (SCREEN_H * 0.5f) - (gap_h * 0.5f);
    // clear left border gap
    C2D_DrawRectSolid(rink_x, gap_y, 0.0f, 4.0f, gap_h, C2D_Color32(0,0,0,255));
    // clear right border gap
    C2D_DrawRectSolid(rink_x + RINK_W - 4.0f, gap_y, 0.0f, 4.0f, gap_h, C2D_Color32(0,0,0,255));

    // Draw paddles
    C2D_DrawRectSolid(left_paddle.x, left_paddle.y, 0.1f, left_paddle.w, left_paddle.h, C2D_Color32(0, 200, 255, 255)); // cyan AI
    C2D_DrawRectSolid(right_paddle.x, right_paddle.y, 0.1f, right_paddle.w, right_paddle.h, C2D_Color32(0, 200, 0, 255));  // green player

    // Draw puck as circle-ish (small filled rectangle for now)
    C2D_DrawRectSolid(puck.x, puck.y, 0.2f, puck.size, puck.size, C2D_Color32(255, 255, 255, 255));
}

// Bottom-screen HUD (console simple text)
static void draw_bottom_console(void) {
    // We use the text console (consoleInit on bottom) for simple debug/HUD strings
    // Print position and velocities (overwrite top-left of console)
    printf("\x1b[1;1HPuck: (%.1f, %.1f)  Vel: (%.2f, %.2f)    \n",
           puck.x, puck.y, puck.vx, puck.vy);
    printf("Player Y: %.1f  AI Y: %.1f  (A to reset puck) \n", right_paddle.y, left_paddle.y);
}

// main
int main(int argc, char** argv) {
    // seed RNG
    srand((unsigned int)svcGetSystemTick());

    // init services
    gfxInitDefault();

    // keep console on bottom only
    consoleInit(GFX_BOTTOM, NULL);

    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();

    // set scene size (width, height, tiltFlag)
    C2D_SceneSize(SCREEN_W, SCREEN_H, false);

    // create render targets (store pointers)
    top_target = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
    bot_target = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);
    if (!top_target || !bot_target) {
        printf("Failed to create C2D render targets!\n");
        // cleanup
        if (top_target) C2D_TargetClear(top_target, C2D_Color32(0,0,0,255));
        C2D_Fini();
        C3D_Fini();
        gfxExit();
        return 1;
    }

    // initialize objects
    init_game_objects();

    // main loop
    while (aptMainLoop()) {
        hidScanInput();
        u32 kDown = hidKeysDown();
        u32 kHeld = hidKeysHeld();

        // exit
        if (kDown & KEY_START) break;
        if (kDown & KEY_A) {
            // reset puck toward player when A pressed
            reset_puck_towards_player();
        }

        // read circle pad for player movement (right paddle)
        circlePosition cp;
        hidCircleRead(&cp);
        // cp.dx, cp.dy range roughly [-32768..32767], scale to paddle speed
        float nx = (float)cp.dx / 32768.0f;
        float ny = (float)cp.dy / 32768.0f;
        // move only vertically
        right_paddle.y += ny * PADDLE_SPEED;

        // clamp right paddle inside rink vertical bounds
        float rink_top = (SCREEN_H - RINK_H) * 0.5f;
        float rink_bottom = rink_top + RINK_H - right_paddle.h;
        if (right_paddle.y < rink_top) right_paddle.y = rink_top;
        if (right_paddle.y > rink_bottom) right_paddle.y = rink_bottom;

        // update AI
        update_ai();

        // update puck
        update_puck();

        // frame begin
        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

        // Top scene
        C2D_TargetClear(top_target, C2D_Color32(0,0,0,255));
        C2D_SceneBegin(top_target);
        draw_rink_and_objects();

        // Bottom scene (we'll just clear to black so console text shows)
        C2D_TargetClear(bot_target, C2D_Color32(0,0,0,255));
        C2D_SceneBegin(bot_target);
        // (we're using the console on bottom to print text; C2D bottom draw is optional)

        C3D_FrameEnd(0);

        // bottom console HUD text
        draw_bottom_console();

        // wait for vblank (citro2d/C3D handles buffer swap)
        gspWaitForVBlank();
    }

    // cleanup
    C2D_Fini();
    C3D_Fini();
    gfxExit();
    return 0;
}
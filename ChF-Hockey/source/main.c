// main.c

// Pong-style hockey demo with rotating paddles, puck physics, and rink borders.
// Uses citro2d, citro3d, and libctru according to documentation.

// Fairchild 'Hockey' (3DS Edition) - Go to https://github.com/James-Karwowski for more 3DS Homebrew projects!
// This demo is inspired by the Channel F's 'Hockey' game with its unique twists such as rotating paddles and a secondary goalie paddle.

// This code is completely open-source, free to use, and modify.
// If you want to modify this project, you can change these constants to adjust gameplay.
// If you need to add functions, structs, etc., please add separate .c/.h files and include them under "Mods" and above "End Mods".
// If you want to add more features, feel free to do so!
// Enjoy!



// If the amount of errors is 2, these are likely #include issues, but the program will still compile and run fine.
// I would make sure though.
// If the amount of errors is more than 2, these are likely syntax issues, and the program will not compile or run properly.

#include <3ds.h>
#include <citro2d.h>
#include <citro3d.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <malloc.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <stdint.h>
#include "menu.c"

// Multiplayer Infrared (MIR) Functionality:
#include "net_ir.c"


// Mods Go Below
// Remove this comment, add your includes here
// Mods Go Above

// Screen constants
#define SCREEN_W 400
#define SCREEN_H 240

// Rink
#define RINK_W 400.0f
#define RINK_H 180.0f

// Gameplay geometry
#define PUCK_SIZE 12.0f
#define PADDLE_W 6.0f
#define PADDLE_H 60.0f

#define GOALIE_W 10.0f
#define GOALIE_H 30.0f
#define GOALIE_X 10.0f
#define GOALIE_Y_LIMIT_TOP (SCREEN_H - 75.0f)
#define GOALIE_Y_LIMIT_BOTTOM (75.0f)

// Speeds
#define PUCK_SPEED_INIT 2.5f
#define PUCK_SPEED_BOOST 1.08f
#define PUCK_SPEED_MAX 6.0f
#define PADDLE_SPEED 10.0f
#define AI_SPEED 3.5f
#define ROT_SPEED 0.05f

typedef struct {
    float x, y;
    float angle; // radians
} Paddle;

typedef struct {
    float x, y;
} Goalie;

typedef struct {
    float x, y;
    float vx, vy;
    float size;
    int justScored;
} Puck;

// Globals
static C3D_RenderTarget* top_target;
static C3D_RenderTarget* bot_target;

static Paddle left_paddle;
static Paddle right_paddle;

static Goalie left_goalie;
static Goalie right_goalie;

static Puck puck;

// Scores
static int left_score = 0;
static int right_score = 0;

// Reset puck to center, moving toward right
static void reset_puck(Puck* puck) {
    puck->size = PUCK_SIZE;
    puck->x = (SCREEN_W - puck->size) * 0.5f;
    puck->y = (SCREEN_H - puck->size) * 0.5f;
    float ang = ((rand() % 60) - 30) * (M_PI / 180.0f);
    puck->vx = cosf(ang) * PUCK_SPEED_INIT;
    puck->vy = sinf(ang) * PUCK_SPEED_INIT;
    if (puck->vx < 0) puck->vx = -puck->vx; // ensure toward right
    puck->justScored = 0;
}

// Init game objects
static void init_game() {
    left_paddle.x = 40;
    left_paddle.y = SCREEN_H * 0.5f;
    left_paddle.angle = 0;

    right_paddle.x = SCREEN_W - 40;
    right_paddle.y = SCREEN_H * 0.5f;
    right_paddle.angle = 0;

    left_goalie.x = GOALIE_X;
    left_goalie.y = SCREEN_H * 0.5f;

    right_goalie.x = SCREEN_W - GOALIE_X;
    right_goalie.y = SCREEN_H * 0.5f;

    reset_puck(&puck);
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

    C2D_DrawLine(x1, y1, color, x2, y2, color, PADDLE_H - 20, 0);
}

static void drawGoalie(Goalie *p, u32 color) {
    float halfLen = GOALIE_W / 2.0f;
    float cosA = cosf(0.0f);
    float sinA = sinf(0.0f);

    float x1 = p->x - halfLen * cosA;
    float y1 = p->y - halfLen * sinA;
    float x2 = p->x + halfLen * cosA;
    float y2 = p->y + halfLen * sinA;

    C2D_DrawLine(x1, y1, color, x2, y2, color, GOALIE_H - 10, 0);
}

// Handle paddle-puck collision (reflect using paddle angle)
static void handlePaddleCollision(Paddle *p) {
    float dx = puck.x + puck.size/2 - p->x;
    float dy = puck.y + puck.size/2 - p->y;
    float dist = sqrtf(dx*dx + dy*dy);

    if((dist < (PADDLE_H/2 + puck.size/2 - 15))) {
        // reflect puck based on paddle angle
        float nx = cosf(p->angle);
        float ny = sinf(p->angle);
        float dot = puck.vx*nx + puck.vy*ny;
        puck.vx -= 2*dot*nx;
        puck.vy -= 2*dot*ny;

        puck.vx *= PUCK_SPEED_BOOST;
        puck.vy *= PUCK_SPEED_BOOST;
        if (puck.vx > PUCK_SPEED_MAX) {
            puck.vx = PUCK_SPEED_MAX;
        }
        if (puck.vy > PUCK_SPEED_MAX) {
            puck.vy = PUCK_SPEED_MAX;
        }
    }
}

// Handle goalie-puck collision (simple AABB)
static void handleGoalieCollision(Goalie *g) {
    if (puck.x < g->x + GOALIE_W && puck.x + puck.size > g->x &&
        puck.y < g->y + GOALIE_H && puck.y + puck.size > g->y) {
        // Simple reflection
        puck.vx = -puck.vx * PUCK_SPEED_BOOST;
        puck.vy = -puck.vy * PUCK_SPEED_BOOST;
        if (puck.vx > PUCK_SPEED_MAX) {
            puck.vx = PUCK_SPEED_MAX;
        }
        if (puck.vy > PUCK_SPEED_MAX) {
            puck.vy = PUCK_SPEED_MAX;
        }
    }
}

// Update puck (walls + paddles + goals)
static void update_puck() {
    puck.x += puck.vx;
    puck.y += puck.vy;

    float rink_top = (SCREEN_H - RINK_H) * 0.5f;
    float rink_bottom = rink_top + RINK_H;
    float rink_left = (SCREEN_W - RINK_W) * 0.5f;
    float rink_right = rink_left + RINK_W;

    // Bounce off top/bottom
    if (puck.y < rink_top) { puck.y = rink_top; puck.vy = -puck.vy; }
    if (puck.y + puck.size > rink_bottom) { puck.y = rink_bottom - puck.size; puck.vy = -puck.vy; }

    // Goal detection
    float goal_gap = 60;
    float gap_y = (SCREEN_H - goal_gap) * 0.5f;

    if (puck.x < rink_left) {
        if (puck.y > gap_y && puck.y + puck.size < gap_y + goal_gap) {
            // Goal for right player
            right_score++;
            reset_puck(&puck);
            return;
        } else {
            puck.x = rink_left;
            puck.vx = -puck.vx;
        }
    }
    if (puck.x + puck.size > rink_right) {
        if (puck.y > gap_y && puck.y + puck.size < gap_y + goal_gap) {
            // Goal for left player
            left_score++;
            reset_puck(&puck);
            return;
        } else {
            puck.x = rink_right - puck.size;
            puck.vx = -puck.vx;
        }
    }

    // Paddle collisions
    handlePaddleCollision(&left_paddle);
    handlePaddleCollision(&right_paddle);

    // Goalie collisions
    handleGoalieCollision(&left_goalie);
    handleGoalieCollision(&right_goalie);
}

// Clamp paddle inside rink
static void clamp_paddle(Paddle *p) {
    float rink_top = (SCREEN_H - RINK_H) * 0.5f;
    float rink_bottom = rink_top + RINK_H;

    // Keep paddle center within borders, accounting for paddle half-height
    float halfLen = PADDLE_H / 2.0f;
    if (p->y - halfLen < rink_top) p->y = rink_top + halfLen;
    if (p->y + halfLen > rink_bottom) p->y = rink_bottom - halfLen;
}

// Clamp paddle inside rink
static void clamp_goalie(Goalie *p, int side) {
    if(side == 0){
        p->x = GOALIE_X;
    } else if(side == 1) {
        p->x = SCREEN_W - GOALIE_X;
    }
    // Keep paddle center within borders, accounting for paddle half-height
    float halfLen = GOALIE_H / 2.0f;
    if (p->y - halfLen < GOALIE_Y_LIMIT_BOTTOM) p->y = GOALIE_Y_LIMIT_BOTTOM + halfLen;
    if (p->y + halfLen > GOALIE_Y_LIMIT_TOP) p->y = GOALIE_Y_LIMIT_TOP - halfLen;
}

// Simple AI
static void update_ai() {
    if (fabsf(puck.y - left_paddle.y) > 2.0f) {
        if (puck.y < left_paddle.y) left_paddle.y -= AI_SPEED;
        else left_paddle.y += AI_SPEED;
        
    }
    if(fabsf(puck.x - left_paddle.x) < 25.0f) {
        if (puck.x < left_paddle.x) left_paddle.x += AI_SPEED;
        else{
            left_paddle.x -= AI_SPEED;
            if(left_paddle.x < 40) left_paddle.x = 40;
            if(left_paddle.x > SCREEN_W/2) left_paddle.x = SCREEN_W/2;
        }
    }
    if (fabsf(puck.y - left_goalie.y) > 2.0f) {
        if (puck.y < left_goalie.y) left_goalie.y -= AI_SPEED;
        else left_goalie.y += AI_SPEED;

    }
    clamp_paddle(&left_paddle);
    clamp_goalie(&left_goalie, 0);
}

// Draw rink + objects
static void draw_scene() {
    float rink_x = (SCREEN_W - RINK_W) * 0.5f;
    float rink_y = (SCREEN_H - RINK_H) * 0.5f;

    // Rink background
    C2D_DrawRectSolid(0, 0, 0, SCREEN_W, SCREEN_H, C2D_Color32(0,255,128,255));
    C2D_DrawRectSolid(rink_x, rink_y, 0, RINK_W, RINK_H, C2D_Color32(161,191,200,255));


    // Top/bottom borders
    C2D_DrawRectSolid(rink_x, rink_y, 0, RINK_W, 4, C2D_Color32(255, 255,255,255));
    C2D_DrawRectSolid(rink_x, rink_y + RINK_H - 4, 0, RINK_W, 4, C2D_Color32(255,255,255,255));

    // Goal posts
    float goal_gap = 60;
    float gap_y = (SCREEN_H - goal_gap) * 0.5f;
    C2D_DrawRectSolid(rink_x, rink_y, 0, 4, gap_y - rink_y, C2D_Color32(255,255,255,255));
    C2D_DrawRectSolid(rink_x, gap_y + goal_gap, 0, 4, rink_y + RINK_H - (gap_y + goal_gap), C2D_Color32(255,255,255,255));
    C2D_DrawRectSolid(rink_x + RINK_W - 4, rink_y, 0, 4, gap_y - rink_y, C2D_Color32(255,255,255,255));
    C2D_DrawRectSolid(rink_x + RINK_W - 4, gap_y + goal_gap, 0, 4, rink_y + RINK_H - (gap_y + goal_gap), C2D_Color32(255,255,255,255));

    // Paddles
    drawPaddle(&left_paddle, C2D_Color32(0,0,255,255));
    drawPaddle(&right_paddle, C2D_Color32(0,128,0,255));

    // Goalies
    drawGoalie(&left_goalie, C2D_Color32(0,0,255,255));
    drawGoalie(&right_goalie, C2D_Color32(0,128,0,255));

    // Puck
    C2D_DrawRectSolid(puck.x, puck.y, 0.2f, puck.size, puck.size, C2D_Color32(0,0,0,255));
}

static void reset_game() {
    left_score = 0;
    right_score = 0;
    reset_puck(&puck);
}

static void pauseGame() {
    // Simple pause - wait for B again
    while (1) {
        hidScanInput();
        u32 kDown = hidKeysDown();
        if (kDown & KEY_B) break;
        gspWaitForVBlank();
    }
}

static bool isHost = false;

/* This will not be availble until later.
static void displayText(const char* text) {
    C2D_TextBuf textBuf = C2D_TextBufNew(4096);
    C2D_Text text = {};
}
*/

int main(int argc, char** argv) {
    srand((unsigned int)svcGetSystemTick());
    
    ndspInit();
    gfxInitDefault();
    PrintConsole bottomScreen;
    consoleInit(GFX_BOTTOM, &bottomScreen);

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

        touchPosition touch;
        hidTouchRead(&touch);
        if (kDown & KEY_TOUCH) {
            netSelectModeFromTouch(&touch);
        }

        if (kDown & KEY_START) pauseGame();
        if (kDown & KEY_SELECT) reset_game();
        if (kDown & KEY_B) pauseGame();
        
        // Circle Pad for right paddle
        circlePosition cp;
        hidCircleRead(&cp);
        right_paddle.x += (float)cp.dx / 477.0f * AI_SPEED;
        right_paddle.y -= (float)cp.dy / 477.0f * AI_SPEED;
        clamp_paddle(&right_paddle);

        if(kHeld & KEY_DUP) right_goalie.y -= PADDLE_SPEED * 0.8f;
        if(kHeld & KEY_DDOWN) right_goalie.y += PADDLE_SPEED * 0.8f;
        clamp_goalie(&right_goalie, 1);

        if (kHeld & KEY_L) right_paddle.angle -= ROT_SPEED;
        if (kHeld & KEY_R) right_paddle.angle += ROT_SPEED;

        if(right_paddle.x > SCREEN_W - 40) right_paddle.x = SCREEN_W - 40;
        if(right_paddle.x < SCREEN_W/2) right_paddle.x = SCREEN_W/2;

        if (currentMode == NET_MODE_IR_HOST) {
            // --- HOST LOGIC ---
            update_ai();       // only host runs AI
            update_puck();     // only host runs puck physics + scoring

        // Prepare packet to send to client
            struct {
                float hostPaddleX, hostPaddleY;
                float puckX, puckY;
                int leftScore, rightScore;
            } hostPkt;

            hostPkt.hostPaddleX = right_paddle.x;
            hostPkt.hostPaddleY = right_paddle.y;
            hostPkt.puckX = puck.x;
            hostPkt.puckY = puck.y;
            hostPkt.leftScore = left_score;
            hostPkt.rightScore = right_score;

            netSendPacket(&hostPkt, sizeof(hostPkt));

            // Receive client paddle
            struct {
                float clientPaddleX, clientPaddleY;
            } clientPkt;

            if (netRecvPacket(&clientPkt, sizeof(clientPkt)) > 0) {
                left_paddle.x = clientPkt.clientPaddleX;
                left_paddle.y = clientPkt.clientPaddleY;
            }

        } else if (currentMode == NET_MODE_IR_CLIENT) {
            // --- CLIENT LOGIC ---
            // Send own paddle position to host
            struct {
                float clientPaddleX, clientPaddleY;
            } clientPkt;

            clientPkt.clientPaddleX = right_paddle.x;
            clientPkt.clientPaddleY = right_paddle.y;

            netSendPacket(&clientPkt, sizeof(clientPkt));

            // Receive puck + host paddle + scores
            struct {
                float hostPaddleX, hostPaddleY;
                float puckX, puckY;
                int leftScore, rightScore;
            } hostPkt;

            if (netRecvPacket(&hostPkt, sizeof(hostPkt)) > 0) {
                left_paddle.x = hostPkt.hostPaddleX;
                left_paddle.y = hostPkt.hostPaddleY;
                puck.x = hostPkt.puckX;
                puck.y = hostPkt.puckY;
                left_score = hostPkt.leftScore;
                right_score = hostPkt.rightScore;
            }
        } else {
            // --- SINGLEPLAYER (no IR selected) ---
            update_ai();
            update_puck();
        }

        // Draw top screen
        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
        C2D_TargetClear(top_target, C2D_Color32(0,0,0,255));
        C2D_SceneBegin(top_target);
        draw_scene();
        C3D_FrameEnd(0);

        // Draw bottom screen scores
        consoleSelect(&bottomScreen);
        consoleClear();
        printf("Left Score: %d\n", left_score);
        printf("Right Score: %d\n", right_score);
        
        if(left_score >= 10) {
            printf("\nLeft Player Wins!\nPress SELECT to reset.");
        } else if(right_score >= 10) {
            printf("\nRight Player Wins!\nPress SELECT to reset.");
        } else {
            printf("\nPress B to pause.\nPress Y for Multiplayer Menu.");
        }

        gspWaitForVBlank();
    }

    C2D_Fini();
    C3D_Fini();
    gfxExit();
    return 0;
}
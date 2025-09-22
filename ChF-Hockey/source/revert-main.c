#include <3ds.h>
#include <stdio.h>
#include <citro2d.h>
#include <citro3d.h>

// Function prototypes
void loadAssets(int menu);
void loadMenu(int menu);
void resetScreen(void);
void changeScreen(int menu);

void loadAssets(int menu) {
    // Placeholder for loading assets based on menu
    if(menu == 0){
        // Load assets for welcome screen
        return;
    } else if(menu == 1){
        // Load assets for main menu
        // Console text, options, etc.
        return;
    } else if(menu == 2){
        // Load assets for single-player mode
        // Two paddles, puck, scoreboard, etc.
    } else if(menu == 3){
        // Load assets for multiplayer mode
        // Two paddles, puck, scoreboard, etc.
        return;
    }
}

void loadMenu(int menu) {
    if(menu == 0){
        printf("\nWelcome! Please review the info on this screen:\n\n\nGame: Channel Fun 'Hockey' (3DS Edition)\nVersion: 0.0.1 Alpha\n\nPress A to go to the main menu.\nPress START to exit.\n\n");

        // Main loop
        while (aptMainLoop()) {
            hidScanInput();

            // Exit on START button
            u32 kDown = hidKeysDown();
            if (kDown & KEY_START) break;

            if (kDown & KEY_A) {
                changeScreen(1);
            }

            gfxFlushBuffers();
            gfxSwapBuffers();
            gspWaitForVBlank();
        }
        // Cleanup
        gfxExit();
        return;
    } else if(menu == 1){
        printf("Press A to continue to single-player mode.\nPress B to continue to multiplayer mode.\nPress START to go back.\n");

        // Main loop
        while (aptMainLoop()) {
            hidScanInput();

            // Exit on START button
            u32 kDown = hidKeysDown();
            if (kDown & KEY_START) break;

            if (kDown & KEY_START) {
                changeScreen(0);
            }

            if (kDown & KEY_A) {
                changeScreen(2);
            }

            if(kDown & KEY_B) {
                // Placeholder for multiplayer mode
                changeScreen(3);
            }

            gfxFlushBuffers();
            gfxSwapBuffers();
            gspWaitForVBlank();
        }
        // Cleanup
        gfxExit();
        return;
    } else if(menu == 2){
        printf("\nSingle-player Mode Loaded! Press START to go back.\n");

        // Main loop
        while (aptMainLoop()) {
            hidScanInput();

            // Exit on START button
            u32 kDown = hidKeysDown();
            if (kDown & KEY_START) break;

            if (kDown & KEY_START) {
                changeScreen(0);
            }

            gfxFlushBuffers();
            gfxSwapBuffers();
            gspWaitForVBlank();
        }
        // Cleanup
        gfxExit();
        return;
    }
}

void resetScreen() {
    gfxExit();
    gfxInitDefault();
    consoleInit(GFX_BOTTOM, NULL);
} 

void changeScreen(int menu) {
    resetScreen();
    loadMenu(menu);
    loadAssets(menu);
}

int main(int argc, char** argv) {
    // Initialize services
    gfxInitDefault();

    // Initialize console on top screen
    consoleInit(GFX_BOTTOM, NULL);

    // Print message
    printf("\nWelcome! Please review the info on this screen:\n\n\n\nGame: Channel Fun 'Hockey' (3DS Edition)\nVersion: 0.0.1 Alpha\n\nPress A to start.\nPress START to exit.\n\n");

    // Main loop
    while (aptMainLoop()) {
        hidScanInput();

        // Exit on START button
        u32 kDown = hidKeysDown();
        if (kDown & KEY_START) break;

        if (kDown & KEY_A) {
            changeScreen(1);
        }

        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    }

    // Cleanup
    gfxExit();
    return 0;
}
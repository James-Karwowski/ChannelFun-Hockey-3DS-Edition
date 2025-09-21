#include <3ds.h>
#include <stdio.h>

int main(int argc, char** argv) {
    // Initialize services
    gfxInitDefault();

    // Initialize console on top screen
    consoleInit(GFX_TOP, NULL);

    // Print message
    printf("\nWelcome! Please review the info on this screen:\n\n\n\nGame: Channel Fun 'Hockey' (3DS Edition)\n\nPress A to start.\nPress START to exit.\n\n");

    // Main loop
    while (aptMainLoop()) {
        hidScanInput();

        // Exit on START button
        u32 kDown = hidKeysDown();
        if (kDown & KEY_START) break;

        if (kDown & KEY_A) {
            printf("Starting...\n");
        }

        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    }

    // Cleanup
    gfxExit();
    return 0;
}
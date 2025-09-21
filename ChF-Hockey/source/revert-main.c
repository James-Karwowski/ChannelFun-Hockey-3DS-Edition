#include <3ds.h>
#include <stdio.h>

int main(int argc, char** argv) {
    // Initialize services
    gfxInitDefault();

    // Initialize console on top screen
    consoleInit(GFX_TOP, NULL);

    // Print message
    printf("\nHockey (3DS Edition)\nPress A to start.\n\nPress START to exit.\n");

    // Main loop
    while (aptMainLoop()) {
        hidScanInput();

        // Exit on START button
        u32 kDown = hidKeysDown();
        if (kDown & KEY_START) break;

        if (kDown & KEY_A) {
            printf("Single-player mode selected.\n");
        }

        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    }

    // Cleanup
    gfxExit();
    return 0;
}
#ifndef MENU_H
#define MENU_H

#include <3ds.h>
#include <stdbool.h>

typedef enum {
    MENU_MAIN,
    MENU_ROLE_CONFIRM,
    MENU_GAME
} MenuState;

typedef enum {
    ROLE_NONE,
    ROLE_HOST,
    ROLE_CLIENT
} Role;

void menuInit();
void menuExit();
void menuUpdate();
void menuDrawTop(gfxScreen_t screen);
void menuDrawBottom(gfxScreen_t screen);

MenuState menuGetState();
Role menuGetRole();

#endif

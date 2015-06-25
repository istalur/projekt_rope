
#ifndef _MAIN_H_
#define _MAIN_H_

#include <stdio.h>														
#include <stdlib.h>															
#include <GL/gl.h>															
#include <GL/glu.h>															
#include <SDL/SDL.h>															
#include "Physics2.h"

#define APP_NAME	"Projekt WONSZ"

#define SCREEN_W	800
#define SCREEN_H	600
#define SCREEN_BPP	16

typedef struct
{
    bool Visible;
    bool MouseFocus;
    bool KeyboardFocus;
} S_AppStatus;

int main(int, char **);

bool InitTimers(Uint32 *);
bool InitGL(SDL_Surface *);
bool CreateWindowGL(SDL_Surface *, int, int, int, Uint32);

void ReshapeGL(int, int);
void ToggleFullscreen(void);
void TerminateApplication(void);

bool Initialize(void);
void Deinitialize(void);
void Update(Uint32, Uint8 *);
void Draw(void);

#endif

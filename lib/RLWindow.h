#include <raylib.h>

#ifndef RLWINDOW_H 
#define RLWINDOW_H 

typedef struct {
	int width;
	int height;
	char* title;
} RLWindow;

void RLSetWinSize(RLWindow* win, int w, int h);

#endif



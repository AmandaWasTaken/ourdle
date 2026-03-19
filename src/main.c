#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>
#include <RLWindow.h>
#include <types.h>

#include "../lib/game.h"
#include "../lib/getword.h"


int main(int argc, char** argv){

	RLWindow window = {
		.width = 800,
		.height = 800,
		.title = "Horse"
	};

	char* target_word = get_word();

	InitWindow(window.width, window.height, window.title);
	Wordel(&window, target_word);
	
	return 0;
}

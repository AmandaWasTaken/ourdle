#include <raylib.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <RLWindow.h>
#include <stdio.h>

typedef enum {
	ONGOING = 0,
	WON,
	LOST,
} GameState;

typedef enum {
	INCORRECT = 0,
	WRONG_POS,
	CORRECT,
	NEUTRAL,
} Cell_State;

typedef struct {
	Rectangle rect;
	Cell_State state;
	char current_letter[2];
} Cell;

typedef struct {
	Cell cells[6][5];
	int row;
	int col;
	int gapX;
	int gapY;
} Grid;

void game_over(RLWindow* win, GameState gs){

	const int font_size = 48;
	char text[10];
	int text_w = 0;

	switch(gs){
		case WON:
			strcpy(text, "You Win!");
			text_w = MeasureText(text, font_size);
			DrawText(text,
				(win->width - text_w)/2,
				(win->height - font_size),
				font_size, DARKGREEN);
			break;
		case LOST:
			strcpy(text, "You Lost!");
			text_w = MeasureText(text, font_size);
			DrawText(text,
				(win->width - text_w)/2,
				(win->height - font_size),
				font_size, RED);
			break;
		default:
			if(text)free(text);
			return;
	}
}

void _word_too_short_warning(RLWindow* w){

	const char* warning = "Word too short!";
	const int w_width   = w->width;
	const int w_height  = w->height;
	const int font_size = 32;

	int text_w = MeasureText(warning, font_size);

	DrawText(warning,
			(w_width - text_w)/2,
			(w_height - font_size - 20),
			font_size, MAROON 
			);
}

Grid initialize_grid(RLWindow* w){

	Grid grid = {
		.row  = 6,
		.col  = 5,
		.gapX = 90,
		.gapY = 90,
	};

	const int cell_width  = 50;
	const int cell_height = 60;
	const int padding_x   = 170;
	const int padding_y   = 120;

	for(int row = 0; row < grid.row; row++){
		for(int col = 0; col < grid.col; col++){
			Rectangle rect = {
				.x = col * grid.gapX,
				.y = row * grid.gapY,
				.width  = cell_width,
				.height = cell_height,
			};
			grid.cells[row][col].rect  = rect;
			grid.cells[row][col].state = NEUTRAL;
		}
	}
	return grid;
}

Color get_color(Cell_State state){
	switch(state){
		case CORRECT:   return DARKGREEN;
		case WRONG_POS: return YELLOW;
		case INCORRECT: return RED;
		case NEUTRAL:   return DARKGRAY;
	}
}

void draw_grid(RLWindow* w, Grid* grid){

	const int padding_left = 180;
	const int padding_top = 90;
	Color color;

	for(int i = 0; i < grid->row; i++){
		for(int j = 0; j < grid->col; j++){

			Rectangle rect = (Rectangle) {
				.x = padding_left + grid->cells[i][j].rect.x,
				.y = padding_top + grid->cells[i][j].rect.y,
				.width = grid->cells[i][j].rect.width,
				.height = grid->cells[i][j].rect.height,
			};

			color = get_color(grid->cells[i][j].state); 
			DrawRectangleLinesEx(rect, 4.0f, color);
			DrawText(grid->cells[i][j].current_letter,
				padding_left + 10 +
				grid->cells[i][j].rect.x,
				padding_top +
				grid->cells[i][j].rect.y,
				64, RED);
		}
	}
}

void to_lower(char* s){
	if(s[0] < 97) s[0] -= 32;
}

void check_guess(Cell* cells, char word[6], const char* target, GameState* gs){

	char target_copy[6];
	strcpy(target_copy, target);
	to_lower(word);
	to_lower(target_copy);
	int n_correct = 0;
	int n_incorrect = 0;

	// Check correct
	for(int i = 0; i < 5; i++){
		if(word[i] == target_copy[i]){
			cells[i].state = CORRECT;
			target_copy[i] = '_';
			n_correct++;
		}
	}

	// Check correct but wrong spot
	for(int i = 0; i < 5; i++){
		if(cells[i].state == CORRECT) continue;

		for(int j = 0; j < 5; j++){
			if(word[i] == target_copy[j]){
				cells[i].state = WRONG_POS;
				target_copy[j] = '_';
				break;
			}
		}
	}

	// Check incorrect
	for(int i = 0; i < 5; i++){
		if(cells[i].state != NEUTRAL) continue;

		for(int j = 0; j < 5; j++){
			if(strchr(target, word[j] == NULL)){
				cells[i].state = INCORRECT;
				n_incorrect++;
			}
		}
	}

	if(n_correct == 5) *gs = WON;	
}

void poll_inputs(RLWindow* w, Grid* grid, const char* target, 
		bool* warn, float* warn_timer, GameState* gs){
	
	static int curr_col = 0;
	static int curr_row = 0;
	static char word[6];

	int ch = GetCharPressed();

	if(IsKeyPressed(KEY_BACKSPACE)){
		if(curr_col - 1 < 0) return;
		curr_col--;
		grid->cells[curr_row][curr_col].current_letter[0] = '\0';
		word[curr_col] = '\0';
	}

	if(IsKeyPressed(KEY_ENTER)){
		if(curr_col < 5){
			*warn = true;
			*warn_timer = 2.0f;
		} else {
			check_guess((grid->cells[curr_row]), word, target, gs);
			curr_col = 0;
			curr_row++;
		}
	}

	if(ch != 0 && curr_col <= 4){
		word[curr_col] = (char)ch;
		grid->cells[curr_row][curr_col].current_letter[0] = (char)ch;
		grid->cells[curr_row][curr_col].current_letter[1] = '\0';
		ch = GetCharPressed();
		curr_col++;
	}
	if(curr_row == 6 && *gs != WON) *gs = LOST;
}

void Wordel(RLWindow* window, const char* target_word){

	Grid grid = initialize_grid(window);
	GameState gs = ONGOING;
	bool word_too_short = false;
	float warn_timer = 0.0f;
	printf("TARGET: %s\n", target_word);
	bool running = true;

	while(!WindowShouldClose()){

		BeginDrawing();
		ClearBackground(RAYWHITE);
		DrawFPS(10, 10);

		draw_grid(window, &grid);

		if(gs == ONGOING){
			poll_inputs(window, &grid, target_word, &word_too_short, 
					&warn_timer, &gs);
		}

		if(word_too_short){
			_word_too_short_warning(window);
			warn_timer -= GetFrameTime();
			if(warn_timer <= 0) word_too_short = false;
		}

		if(gs == WON || gs == LOST){
			game_over(window, gs);
		}
				
		EndDrawing();
	}

//	free(&target_word);
	return;
}

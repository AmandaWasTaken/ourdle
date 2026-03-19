#include <raylib.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <RLWindow.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/wait.h>
#include "../lib/wutils.h"
#include "../lib/getword.h"

#define UNUSED __attribute((unused))

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


void game_over(RLWindow* win, GameState gs, Font font, const char* word){

	const int font_size = 48;
	char text[10];
	int text_w = 0;
	Vector2 pos;

	const char* restart_text = "Press R to restart with a new word";
	char sln_text[24];
	snprintf(sln_text, sizeof(sln_text), "The Word Was %s", word);

	switch(gs){
		case WON:
			strcpy(text, "You Win!");
			text_w = MeasureText(text, font_size);
			 pos = _get_text_pos((win->width - text_w)/2, 
						  (win->height - font_size));
			DrawTextEx(font, text, pos, font_size, 0, WINCOLOR);
			break;
		case LOST:
			strcpy(text, "You Lost!");
			text_w = MeasureText(text, font_size);
			pos = _get_text_pos((win->width - text_w)/2, 
						  (win->height - font_size));
			DrawTextEx(font, text, pos, font_size, 0, LOSECOLOR);
			break;
		default:
//			if(text)free(text);
			return;
	}

	const int restart_font_size = 48;
	int restart_text_w = MeasureText(restart_text, restart_font_size);
	Vector2 restart_text_pos = {100, 33};
	DrawTextEx(font, restart_text, restart_text_pos, restart_font_size,
				0, RESCOLOR);

	const int sln_font_size = 36;
	Vector2 sln_text_pos = {strlen(sln_text) + 225,
		win->height - sln_font_size - 50};
	DrawTextEx(font, sln_text, sln_text_pos, sln_font_size, 0, RESCOLOR);
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

Grid initialize_grid(UNUSED RLWindow* w){

	Grid grid = {
		.row  = 6,
		.col  = 5,
		.gapX = 90,
		.gapY = 90,
	};

	const int cell_width  = 50;
	const int cell_height = 60;
/*	const int padding_x   = 170;
	const int padding_y   = 120;
*/
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
		case NEUTRAL:   return CELL_NEUTRAL;
		default: return DARKGRAY;
	}
}

void draw_grid(UNUSED RLWindow* w, Grid* grid, Font font){

	const int padding_left = 180;
	const int padding_top = 85;
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

			Vector2 text_pos = _get_text_pos(padding_left + 7 +
					grid->cells[i][j].rect.x, padding_top - 5  +
					grid->cells[i][j].rect.y);
			DrawTextEx(font, grid->cells[i][j].current_letter, 
					   text_pos, 56, 0, LETTERCOLOR);
		}
	}
}

bool invalid_guess(char word[6]){

	word[strcspn(word, "\n")] = '\0';

	pid_t pid = fork();
	if(pid == 0){
		char* args[] = {
			"grep",
			"-Fx",
			(char *)word,
			"/etc/dictionaries-common/words",
			NULL,
		};

		execvp("grep", args);
		_exit(1);
	}

	int status;
	waitpid(pid, &status, 0);
	if(WIFEXITED(status)){
		int code = WEXITSTATUS(status);
		if(code == 0) return false;
		if(code == 1) return true;
	}
	
	return true;
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

/*	if(invalid_guess(word)){
		printf("invalid word!!!\n");
		return;
	}
	*/

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
			if(strchr(target, word[j]) == NULL){
				cells[i].state = INCORRECT;
				n_incorrect++;
			}
		}
	}

	if(n_correct == 5) *gs = WON;	
}

void poll_inputs(UNUSED RLWindow* w, Grid* grid, const char* target, 
		bool* warn, float* warn_timer, GameState* gs, Font font){
	
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

	if(curr_row == 6 && *gs != WON){
		*gs = LOST;
		curr_row = 0;
		curr_col = 0;
	}
	if(*gs == WON) {
		curr_row = 0;
		curr_col = 0;
	}
}

void Wordel(RLWindow* window, const char* target_word){

	Grid grid = initialize_grid(window);
	Font font = LoadFont("./assets/RobotoSlab-Medium.ttf");
	GameState gs = ONGOING;
	bool word_too_short = false;
	float warn_timer = 0.0f;
	printf("TARGET: %s\n", target_word);

	while(!WindowShouldClose()){

		if((gs == WON || gs == LOST) && IsKeyPressed('R')){
			grid = initialize_grid(window);
			gs = ONGOING;
			word_too_short = false;
			warn_timer = 0.0f;
			target_word = get_word();
		}

		BeginDrawing();
		ClearBackground(BGCOLOR);
		DrawFPS(10, 10);

		draw_grid(window, &grid, font);

		if(gs == ONGOING){
			poll_inputs(window, &grid, target_word, &word_too_short, 
					&warn_timer, &gs, font);
		}

		if(word_too_short){
			_word_too_short_warning(window);
			warn_timer -= GetFrameTime();
			if(warn_timer <= 0) word_too_short = false;
		}

		if(gs == WON || gs == LOST){
			game_over(window, gs, font, target_word);
		}
				
		EndDrawing();
	}
	UnloadFont(font);

//	free(&target_word);
	return;
}

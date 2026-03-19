#include <raylib.h>

// Custom colors 
#define LETTERCOLOR CLITERAL(Color){203, 240, 245, 255} 
#define GUESSES_COLOR CLITERAL(Color){32, 227, 175, 255}
#define WINCOLOR	CLITERAL(Color){10 , 168, 126, 255}
#define RESCOLOR    CLITERAL(Color){3, 168, 56, 255}
#define LOSECOLOR   CLITERAL(Color){240, 57, 41, 255}
#define BGCOLOR		CLITERAL(Color){1, 108, 117, 255}
#define CELL_NEUTRAL CLITERAL(Color){237, 255, 246, 255}

// Helper function to get text position offsets as a Vector2
// (Needed by DrawTextEx)
Vector2 _get_text_pos(float posX_padded, float posY_padded);

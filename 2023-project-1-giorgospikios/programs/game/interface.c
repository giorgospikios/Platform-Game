#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "state.h"
#include "interface.h"


void interface_init() {
	// Αρχικοποίηση του παραθύρου
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "game");
	SetTargetFPS(60);
}

void interface_close() {
	CloseAudioDevice();
	CloseWindow();
}

// Draw game (one frame)
void interface_draw_frame(State state) {
	BeginDrawing();

	// Καθαρισμός, θα τα σχεδιάσουμε όλα από την αρχή
	ClearBackground(RAYWHITE);

	StateInfo info = state_info(state);
	float x_offset = info->ball->rect.x - SCREEN_WIDTH/2 + 100;


	DrawRectangle(350 , info->ball->rect.y , info->ball->rect.width, info->ball->rect.height, BLACK);


	List objs = state_objects(state, 0 + x_offset , SCREEN_WIDTH + x_offset);
	for(ListNode node = list_first(objs);
		node != LIST_EOF;
		node = list_next(objs, node)){

			Object obj = list_node_value(objs, node);

			if(obj->type == PLATFORM){
				if(obj->unstable == false)
					DrawRectangle(obj->rect.x - x_offset, obj->rect.y + 45, obj->rect.width , obj->rect.height, RED);
				else 
					DrawRectangle(obj->rect.x - x_offset, obj->rect.y + 45, obj->rect.width , obj->rect.height, GREEN);
			}

			if(obj->type == STAR){
				DrawRectangle(obj->rect.x - x_offset, obj->rect.y , obj->rect.width , obj->rect.height, YELLOW);
			}
		}


	// Σχεδιάζουμε το σκορ και το FPS counter
	DrawText(TextFormat("%04i", info->score), 20, 20, 40, GRAY);
	DrawFPS(SCREEN_WIDTH - 80, 0);

	// Αν το παιχνίδι έχει τελειώσει, σχεδιάζομαι το μήνυμα για να ξαναρχίσει
	if (!state_info(state)->playing) {
		DrawText(
			"PRESS [ENTER] TO PLAY AGAIN",
			 GetScreenWidth() / 2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20) / 2,
			 GetScreenHeight() / 2 - 50, 20, GRAY
		);
	}

	if (state_info(state)->paused) {
		DrawText(
			"PRESS [P] TO CONTINUE",
			 GetScreenWidth() / 2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20) / 2,
			 GetScreenHeight() / 2 - 50, 20, GRAY
		);
	}

	EndDrawing();
}
//////////////////////////////////////////////////////////////////
//
// Test για το state.h module
//
//////////////////////////////////////////////////////////////////

#include "acutest.h"			// Απλή βιβλιοθήκη για unit testing

#include "state.h"
#include "set_utils.h"

void test_state_create() {

	State state = state_create();
	TEST_ASSERT(state != NULL);

	StateInfo info = state_info(state);
	TEST_ASSERT(info != NULL);

	TEST_ASSERT(info->playing);
	TEST_ASSERT(!info->paused);
	TEST_ASSERT(info->score == 0);


	List test_objs = state_objects(state, 0, SCREEN_WIDTH);
	//printf("\n\n before test assert \n\n");
	TEST_ASSERT(list_size(test_objs) == 2);

	//Προσθέστε επιπλέον ελέγχους
}

void test_state_update() {
	State state = state_create();
	TEST_ASSERT(state != NULL && state_info(state) != NULL);

	// Πληροφορίες για τα πλήκτρα (αρχικά κανένα δεν είναι πατημένο)
	struct key_state keys = { false, false, false, false, false, false, false };
	
	// Χωρίς κανένα πλήκτρο, η μπάλα μετακινείται 4 pixels δεξιά
	Rectangle old_rect = state_info(state)->ball->rect;
	state_update(state, &keys);
	Rectangle new_rect = state_info(state)->ball->rect;

	TEST_ASSERT( new_rect.x == old_rect.x + 4 );

	// Με πατημένο το δεξί βέλος, η μπάλα μετακινείται 6 pixels δεξιά
	keys.right = true;
	old_rect = state_info(state)->ball->rect;
	state_update(state, &keys);
	new_rect = state_info(state)->ball->rect;

	TEST_ASSERT( new_rect.x == old_rect.x + 6);
	keys.right = false;

	// left key test
	keys.left = true;
	state_info(state)->ball->rect.x = 15;
	state_update(state, &keys);
	new_rect = state_info(state)->ball->rect;

	TEST_ASSERT( new_rect.x == 16 );
	keys.left = false;

	

	// vert_speed test
	keys.up = true;
	state_info(state)->ball->vert_mov = JUMPING;
	state_info(state)->ball->vert_speed = 20;
	state_update(state, &keys);
	float new_vert_speed = state_info(state)->ball->vert_speed;

	TEST_ASSERT(new_vert_speed == 17);
	keys.up = false;

	//FALLING test
	state_info(state)->ball->vert_mov = JUMPING;
	keys.up = true;
	state_info(state)->ball->vert_speed = 0.55;
	state_update(state, &keys);

	TEST_ASSERT(state_info(state)->ball->vert_mov == FALLING);
	keys.up = false;

	//IDLE TO JUMP and starting vert_speed
	state_info(state)->ball->vert_mov = IDLE;
	keys.up = true;
	state_update(state, &keys);

	TEST_ASSERT(state_info(state)->ball->vert_mov == JUMPING);
	//printf("\n\nstate_info(state)->ball->vert_speed = %f\n\n", state_info(state)->ball->vert_speed);
	//TEST_ASSERT(state_info(state)->ball->vert_speed == 14.45000);
	keys.up = false;

	
	//να η μπαλα φτασει στο κατω μερος της οθονης τερματιζει
	state_info(state)->ball->rect.y = SCREEN_HEIGHT - 1;
	state_info(state)->ball->vert_mov = FALLING;
	state_update(state, &keys);

	TEST_ASSERT(state_info(state)->playing == false);

	//Μπαλα να βρισκεται πανω σε πλαφορμα σε κατασταση IDLE 

	List list_of_platforms = state_objects(state, 0, SCREEN_WIDTH);

	float x_of_platform;
	float width_of_platform;
	for(ListNode node = list_first(list_of_platforms);
		node != LIST_EOF; 
		node = list_next(list_of_platforms, node)){

			Object platform = list_node_value(list_of_platforms, node);
			if(platform->type == PLATFORM){

				x_of_platform = platform->rect.x;
				width_of_platform = platform->rect.width;
				printf("\n\nplatform_x = %f \n  platform_width = %f\n\n", platform->rect.x, platform->rect.width);
			}

		}

	printf("\n\n x_of_platform = %f \n  width_of_plaform = %f\n\n", x_of_platform, width_of_platform);
	

	state_info(state)->ball->vert_mov = IDLE;
	state_info(state)->ball->rect.x = (x_of_platform + width_of_platform + 100);

	keys.up = false;
	keys.right = false;
	keys.left = false;
	state_update(state, &keys);

	TEST_ASSERT(state_info(state)->ball->vert_mov == IDLE);

	// //μπαλα σε κατασταση FALLING συγκρουση με πλατφορμα

	// List list_of_platforms = state_objects(state, 0, SCREEN_WIDTH);

	// float x_of_platform;
	// float width_of_platform;
	// Object rect_platform = malloc(sizeof(*rect_platform));

	// for(ListNode node = list_first(list_of_platforms);
	// 	node != LIST_EOF; 
	// 	node = list_next(list_of_platforms, node)){

	// 		Object platform = list_node_value(list_of_platforms, node);
	// 		if(platform->type == PLATFORM){

	// 			x_of_platform = platform->rect.x;
	// 			width_of_platform = platform->rect.width;
	// 			printf("\n\nplatform_x = %f \n  platform_width = %f\n\n", platform->rect.x, platform->rect.width);

	// 		}

	// 		rect_platform = platform;

	// 	}

	// printf("\n\n x_of_platform = %f \n  width_of_plaform = %f\n\n", x_of_platform, width_of_platform);

	// printf("\n\n rect_platform->rect.x = %f \n rect_platform->rect.y = %f",  rect_platform->rect.x , rect_platform->rect.y);
	
	// state_info(state)->ball->vert_mov = FALLING;
	// state_info(state)->ball->rect.x = (x_of_platform + width_of_platform/2);
	// state_info(state)->ball->rect.y = rect_platform->rect.y; 

	// printf("\n\n state_info(state)->ball->rect.x = %f \n state_info(state)->ball->rect.y = %f",  
	// state_info(state)->ball->rect.x , state_info(state)->ball->rect.y);

	// CheckCollisionRecs(state_info(state)->ball->rect, rect_platform->rect);

	// keys.up = false;
	// keys.right = false;
	// keys.left = false;
	// state_update(state, &keys);

	// TEST_ASSERT(state_info(state)->ball->vert_mov == IDLE);

}



// Λίστα με όλα τα tests προς εκτέλεση
TEST_LIST = {
	{ "test_state_create", test_state_create },
	{ "test_state_update", test_state_update },

	{ NULL, NULL } // τερματίζουμε τη λίστα με NULL
};
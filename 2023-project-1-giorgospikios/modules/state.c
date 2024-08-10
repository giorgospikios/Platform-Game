#include <stdlib.h>
#include <stdio.h>

#include "ADTVector.h"
#include "ADTList.h"
#include "state.h"

// Οι ολοκληρωμένες πληροφορίες της κατάστασης του παιχνιδιού.
// Ο τύπος State είναι pointer σε αυτό το struct, αλλά το ίδιο το struct
// δεν είναι ορατό στον χρήστη.

struct state {
	Vector objects;			// περιέχει στοιχεία Object (Πλατφόρμες, Αστέρια)
	struct state_info info;	// Γενικές πληροφορίες για την κατάσταση του παιχνιδιού
	float speed_factor;		// Πολλαπλασιαστής ταχύτητς (1 = κανονική ταχύτητα, 2 = διπλάσια, κλπ)
};



void swaping_objects(Object *obj1, Object *obj2){

	Object tmp_obj = *obj1;
	*obj1 = *obj2;
	*obj2 = tmp_obj;
}

void deleting_object_of_collision(Pointer obj_to_delete, Pointer obj, State state)
{
	swaping_objects(obj_to_delete, obj);
	vector_remove_last(state->objects);
}


float x_of_last_platform;



// Δημιουργεί και επιστρέφει ένα αντικείμενο

static Object create_object(ObjectType type, float x, float y, float width, float height, VerticalMovement vert_mov, float speed, bool unstable) {
	Object obj = malloc(sizeof(*obj));
	obj->type = type;
	obj->rect.x = x;
	obj->rect.y = y;
	obj->rect.width = width;
	obj->rect.height = height;
	obj->vert_mov = vert_mov;
	obj->vert_speed = speed;
	obj->unstable = unstable;
	return obj;
}

// Προσθέτει αντικείμενα στην πίστα (η οποία μπορεί να περιέχει ήδη αντικείμενα).
// Τα αντικείμενα ξεκινάνε από x = start_x, και επεκτείνονται προς τα δεξιά.
//
// ΠΡΟΣΟΧΗ: όλα τα αντικείμενα έχουν συντεταγμένες x,y σε ένα
// καρτεσιανό επίπεδο.
// - Στο άξονα x το 0 είναι η αρχή της πίστας και οι συντεταγμένες
//   μεγαλώνουν προς τα δεξιά.
//
// - Στον άξονα y το 0 είναι το πάνω μέρος της πίστας, και οι συντεταγμένες μεγαλώνουν
//   προς τα _κάτω_. Η πίστα έχει σταθερό ύψος, οπότε όλες οι
//   συντεταγμένες y είναι ανάμεσα στο 0 και το SCREEN_HEIGHT.
//
// Πέρα από τις συντεταγμένες, αποθηκεύουμε και τις διαστάσεις width,height
// κάθε αντικειμένου. Τα x,y,width,height ορίζουν ένα παραλληλόγραμμο, οπότε
// μπορούν να αποθηκευτούν όλα μαζί στο obj->rect τύπου Rectangle (ορίζεται
// στο include/raylib.h). Τα x,y αναφέρονται στην πάνω-αριστερά γωνία του Rectangle.

static void add_objects(State state, float start_x) {
	// Προσθέτουμε PLATFORM_NUM πλατφόρμες, με τυχαία χαρακτηριστικά.

	for (int i = 0; i < PLATFORM_NUM; i++) {
		Object platform = create_object(
			PLATFORM,
			start_x + 150 + rand() % 80,						// x με τυχαία απόσταση από το προηγούμενο στο διάστημα [150, 230]
			SCREEN_HEIGHT/4 + rand() % SCREEN_HEIGHT/2,			// y τυχαία στο διάστημα [SCREEN_HEIGHT/4, 3*SCREEN_HEIGHT/4]
			i == 0 ? 250 : 50 + rand()%200,						// πλάτος τυχαία στο διάστημα [50, 250] (η πρώτη πάντα 250)
			20,													// ύψος
			i < 3 || rand() % 2 == 0 ? MOVING_UP : MOVING_DOWN,	// τυχαία αρχική κίνηση (οι πρώτες 3 πάντα πάνω)
			0.6 + 3*(rand()%100)/100,							// ταχύτητα τυχαία στο διάστημα [0.6, 3.6]
			i > 0 && (rand() % 10) == 0							// το 10% (τυχαία) των πλατφορμών είναι ασταθείς (εκτός από την πρώτη)
		);
		vector_insert_last(state->objects, platform);

		// Στο 50% των πλατφορμών (τυχαία), εκτός της πρώτης, προσθέτουμε αστέρι
		if(i != 0 && rand() % 2 == 0) {
			Object star = create_object(
				STAR,
				start_x + 200 + rand() % 60,				// x με τυχαία απόσταση από την προηγούμενη πλατφόρμα στο διάστημα [200,260]
				SCREEN_HEIGHT/8 + rand() % SCREEN_HEIGHT/2,	// y τυχαία στο διάστημα [SCREEN_HEIGHT/8, 5*SCREEN_HEIGHT/8]
				30, 30,										// πλάτος, ύψος
				IDLE,										// χωρίς κίνηση
				0,										 	// ταχύτητα 0
				false										// 'unstable' πάντα false για τα αστέρια
			);
			vector_insert_last(state->objects, star);
		}

		start_x = platform->rect.x + platform->rect.width;	// μετακίνηση των επόμενων αντικειμένων προς τα δεξιά

		if(i == PLATFORM_NUM - 1)
			x_of_last_platform = platform->rect.x;
	}
}

// Δημιουργεί και επιστρέφει την αρχική κατάσταση του παιχνιδιού

State state_create() {
	// Δημιουργία του state
	State state = malloc(sizeof(*state));

	// Γενικές πληροφορίες
	state->info.playing = true;				// Το παιχνίδι ξεκινάει αμέσως
	state->info.paused = false;				// Χωρίς να είναι paused.
	state->speed_factor = 1;				// Κανονική ταχύτητα
	state->info.score = 0;				// Αρχικό σκορ 0

	// Δημιουργούμε το vector των αντικειμένων, και προσθέτουμε αντικείμενα
	// ξεκινώντας από start_x = 0.
	state->objects = vector_create(0, NULL);
	add_objects(state, 0);

	// Δημιουργούμε την μπάλα τοποθετώντας τη πάνω στην πρώτη πλατφόρμα
	Object first_platform = vector_get_at(state->objects, 0);
	state->info.ball = create_object(
		BALL,
		first_platform->rect.x,			// x στην αρχή της πλατφόρμας
		first_platform->rect.y - 40,	// y πάνω στην πλατφόρμα
		45, 45,							// πλάτος, ύψος
		IDLE,							// χωρίς αρχική κατακόρυφη κίνηση
		0,								// αρχική ταχύτητα 0
		false							// "unstable" πάντα false για την μπάλα
	);

	return state;
}

// Επιστρέφει τις βασικές πληροφορίες του παιχνιδιού στην κατάσταση state
StateInfo state_info(State state){

	return &state->info;
}

// Επιστρέφει μια λίστα με όλα τα αντικείμενα του παιχνιδιού στην κατάσταση state,
// των οποίων η συντεταγμένη x είναι ανάμεσα στο x_from και x_to.

List state_objects(State state, float x_from, float x_to) {

	List xfrom_xto_objects_list = list_create(NULL);

	for(VectorNode node = vector_first(state->objects);
		node != VECTOR_EOF;
		node = vector_next(state->objects, node)){

			Object in_for_tmp_obj = vector_node_value(state->objects, node);
			if( (in_for_tmp_obj->rect.x >= x_from) && (in_for_tmp_obj->rect.x <= x_to) )
				list_insert_next(xfrom_xto_objects_list, LIST_BOF, in_for_tmp_obj);
		}
	return xfrom_xto_objects_list;
}


// Ενημερώνει την κατάσταση state του παιχνιδιού μετά την πάροδο 1 frame.
// Το keys περιέχει τα πλήκτρα τα οποία ήταν πατημένα κατά το frame αυτό.

void state_update(State state, KeyState keys) {

	if(state->info.playing){

		//κατακορυφη και οριζοντια κινηση μπαλας
		if(!keys->right && !keys->left)
			state->info.ball->rect.x += 4*state->speed_factor;

		if(keys->right == true)
			state->info.ball->rect.x += 6*state->speed_factor;

		if(keys->left == true)
			state->info.ball->rect.x += 1*state->speed_factor;
		

		// Κατακόρυφη κίνηση μπάλας
		if(state->info.ball->vert_mov == IDLE)
			state->info.ball->vert_speed = 0;

		if(keys->up && state->info.ball->vert_mov == IDLE){
			state->info.ball->vert_speed = 17;
			state->info.ball->vert_mov = JUMPING;
		}

		if(state->info.ball->vert_mov == JUMPING){
			keys->up = false;
			state->info.ball->rect.y -= state->info.ball->vert_speed;
			state->info.ball->vert_speed *= 0.85;

			if(state->info.ball->vert_speed <= 0.5)
			state->info.ball->vert_mov = FALLING;
		}


		if(state->info.ball->vert_mov == FALLING){
			keys->up = false;
			state->info.ball->rect.y += state->info.ball->vert_speed;
			state->info.ball->vert_speed += (state->info.ball->vert_speed*0.1);
			
			if(state->info.ball->vert_speed >= 7)
				state->info.ball->vert_speed = state->info.ball->vert_speed;
		}




		//Κατακόρυφη κίνηση πλατφόρμας
		for(VectorNode node = vector_first(state->objects);
			node != VECTOR_EOF;
			node = vector_next(state->objects, node)){

				Object platform = vector_node_value(state->objects, node);

				if(platform->type == PLATFORM){

					if(platform->vert_mov == MOVING_UP){
						platform->rect.y -= platform->vert_speed;
						if(platform->rect.y < SCREEN_HEIGHT/4)
								platform->vert_mov = MOVING_DOWN;
					}

					if(platform->vert_mov == MOVING_DOWN){
						platform->rect.y += platform->vert_speed;
						if(platform->rect.y > 3*SCREEN_HEIGHT/4)
							platform->vert_mov = MOVING_UP;
					}

					if(platform->vert_mov == FALLING)
						platform->rect.y += 4;
				}
				
		}

		//εκκινηση και διακοπη 

		if(state->info.playing == false){

			vector_destroy(state->objects);

			if(keys->enter == true)
				state_create();
		}

		if(keys->p)
			state->info.paused = true;	

		//Συμπεριφορά μπάλας σε κατακόρυφη ηρεμία (IDLE) ανάλογα με το αν βρίσκεται πάνω σε πλατφόρμα

		for(VectorNode node = vector_first(state->objects);
			node != VECTOR_EOF;
			node = vector_next(state->objects, node)){

				Object platform = vector_node_value(state->objects, node);

				bool ball_still_on_previous_platform = false;
				if(platform->type == PLATFORM && state->info.ball->vert_mov == IDLE){

					if(state->info.ball->rect.x >= platform->rect.x &&
					state->info.ball->rect.x <= (platform->rect.x + platform->rect.width)){

						state->info.ball->rect.y = platform->rect.y;
						
						ball_still_on_previous_platform = true;
						if(ball_still_on_previous_platform)
							break;
					}
					else{
						state->info.ball->vert_mov = FALLING;
						state->info.ball->vert_speed = 1.5;
					}
				}
			}
		

		//Συγκρούσεις
		if(state->info.ball->rect.y >= SCREEN_HEIGHT - state->info.ball->rect.height){
			state->info.playing = false;
			state->info.score = 0;
		}
			
		
		for(VectorNode node = vector_first(state->objects);
			node != VECTOR_EOF;
			node = vector_next(state->objects, node)){

				Object platform_or_star = vector_node_value(state->objects, node);

				if(platform_or_star->type == STAR){

					if(CheckCollisionRecs(state->info.ball->rect , platform_or_star->rect)){

						Object last_obj = vector_node_value(state->objects, vector_last(state->objects));
						deleting_object_of_collision(platform_or_star, last_obj, state);
						state->info.score += 10;
					}
				}

				if(platform_or_star->type == PLATFORM && platform_or_star->vert_mov == FALLING){

					if(platform_or_star->rect.y >= SCREEN_HEIGHT){

						Object last_obj = vector_node_value(state->objects, vector_last(state->objects));
						deleting_object_of_collision(platform_or_star, last_obj, state);

					}
				}

				if(state->info.ball->vert_mov == FALLING && platform_or_star->type == PLATFORM){
					
					if((state->info.ball->rect.y  - state->info.ball->rect.height) <= platform_or_star->rect.y){
						if(CheckCollisionRecs(state->info.ball->rect , platform_or_star->rect)){

							if(platform_or_star->unstable == true)
								platform_or_star->vert_mov = FALLING;

							state->info.ball->vert_mov = IDLE;
							state->info.ball->rect.y = platform_or_star->rect.y;
					
						}
					}
				}
		}
		
		// expanding game
		if(state->info.ball->rect.x == (x_of_last_platform - SCREEN_WIDTH))
			add_objects(state, state->info.ball->rect.x);

		if(state->info.ball->rect.x == x_of_last_platform)
			state->speed_factor += (state->speed_factor*0.1);
	}
}

// Καταστρέφει την κατάσταση state ελευθερώνοντας τη δεσμευμένη μνήμη.

void state_destroy(State state) {
	// Προς υλοποίηση
}
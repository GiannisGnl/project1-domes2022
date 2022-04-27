#include <stdlib.h>
#include "ADTList.h"
#include "state.h"


// Οι ολοκληρωμένες πληροφορίες της κατάστασης του παιχνιδιού.
// Ο τύπος State είναι pointer σε αυτό το struct, αλλά το ίδιο το struct
// δεν είναι ορατό στον χρήστη.

struct state {
	List objects;			// περιέχει στοιχεία Object (Εδαφος / Ελικόπτερα / Πλοία/ Γέφυρες)
	struct state_info info;	// Γενικές πληροφορίες για την κατάσταση του παιχνιδιού
	float speed_factor;		// Πολλαπλασιαστής ταχύτητς (1 = κανονική ταχύτητα, 2 = διπλάσια, κλπ)
};


// Δημιουργεί και επιστρέφει ένα αντικείμενο

static Object create_object(ObjectType type, float x, float y, float width, float height) {
	Object obj = malloc(sizeof(*obj));
	obj->type = type;
	obj->rect.x = x;
	obj->rect.y = y;
	obj->rect.width = width;
	obj->rect.height = height;
	return obj;
}

// Προσθέτει αντικείμενα στην πίστα (η οποία μπορεί να περιέχει ήδη αντικείμενα).
// Τα αντικείμενα ξεκινάνε από y = start_y, και επεκτείνονται προς τα πάνω.
//
// ΠΡΟΣΟΧΗ: όλα τα αντικείμενα έχουν συντεταγμένες x,y σε ένα
// καρτεσιανό επίπεδο.
// - Στο άξονα x το 0 είναι το αριστερό μέρος της πίστας και οι συντεταγμένες
//   μεγαλώνουν προς τα δεξιά. Η πίστα έχει σταθερό πλάτος, οπότε όλες οι
//   συντεταγμένες x είναι ανάμεσα στο 0 και το SCREEN_WIDTH.
//
// - Στον άξονα y το 0 είναι η αρχή της πίστας, και οι συντεταγμένες μεγαλώνουν
//   προς τα _κάτω_. Η πίστα αντιθέτως επεκτείνεται προς τα πάνω, οπότε όλες οι
//   συντεταγμένες των αντικειμένων είναι _αρνητικές_.
//
// Πέρα από τις συντεταγμένες, αποθηκεύουμε και τις διαστάσεις width,height
// κάθε αντικειμένου. Τα x,y,width,height ορίζουν ένα παραλληλόγραμμο, οπότε
// μπορούν να αποθηκευτούν όλα μαζί στο obj->rect τύπου Rectangle (ορίζεται
// στο include/raylib.h). Τα x,y αναφέρονται στην πάνω-αριστερά γωνία του Rectangle.

static void add_objects(State state, float start_y) {
	// Προσθέτουμε BRIDGE_NUM γέφυρες.
	// Στο διάστημα ανάμεσα σε δύο διαδοχικές γέφυρες προσθέτουμε:
	// - Εδαφος, αριστερά και δεξιά της οθόνης (με μεταβαλλόμενο πλάτος).
	// - 3 εχθρούς (ελικόπτερα και πλοία)
	// Τα αντικείμενα έχουν SPACING pixels απόσταση μεταξύ τους.

	for (int i = 0; i < BRIDGE_NUM; i++) {
		// Δημιουργία γέφυρας
		Object bridge = create_object(
			BRIDGE,
			0,								// x στο αριστερό άκρο της οθόνης
			start_y - 4 * (i+1) * SPACING,	// Η γέφυρα i έχει y = 4 * (i+1) * SPACING
			SCREEN_WIDTH,					// Πλάτος ολόκληρη η οθόνη
			20								// Υψος
		);

		// Δημιουργία εδάφους
		Object terrain_left = create_object(
			TERRAIN,
			0,								// Αριστερό έδαφος, x = 0
			bridge->rect.y,					// y ίδιο με την γέφυρα
			rand() % (SCREEN_WIDTH/3),		// Πλάτος επιλεγμένο τυχαία
			4*SPACING						// Υψος καλύπτει το χώρο ανάμεσα σε 2 γέφυρες
		);
		int width = rand() % (SCREEN_WIDTH/2);
		Object terrain_right = create_object(
			TERRAIN,
			SCREEN_WIDTH - width,			// Δεξί έδαφος, x = <οθόνη> - <πλάτος εδάφους>
			bridge->rect.y,					// y ίδιο με τη γέφυρα
			width,							// Πλάτος, επιλεγμένο τυχαία
			4*SPACING						// Υψος καλύπτει το χώρο ανάμεσα σε 2 γέφυρες
		);

		list_insert_next(state->objects, list_last(state->objects), terrain_left);
		list_insert_next(state->objects, list_last(state->objects), terrain_right);
		list_insert_next(state->objects, list_last(state->objects), bridge);

		// Προσθήκη 3 εχθρών πριν από τη γέφυρα.
		for (int j = 0; j < 3; j++) {
			// Ο πρώτος εχθρός βρίσκεται SPACING pixels κάτω από τη γέφυρα, ο δεύτερος 2*SPACING pixels κάτω από τη γέφυρα, κλπ.
			float y = bridge->rect.y + (j+1)*SPACING;

			Object enemy = rand() % 2 == 0		// Τυχαία επιλογή ανάμεσα σε πλοίο και ελικόπτερο
				? create_object(WARSHIP,    (SCREEN_WIDTH - 83)/2, y, 83, 30)		// οριζόντιο κεντράρισμα
				: create_object(HELICOPTER, (SCREEN_WIDTH - 66)/2, y, 66, 25);
			enemy->forward = rand() % 2 == 0;	// Τυχαία αρχική κατεύθυνση

			list_insert_next(state->objects, list_last(state->objects), enemy);
		}
	}
}

// Δημιουργεί και επιστρέφει την αρχική κατάσταση του παιχνιδιού

State state_create() {
	// Δημιουργία του state
	State state = malloc(sizeof(*state));

	// Γενικές πληροφορίες
	state->info.playing = true;				// Το παιχνίδι ξεκινάει αμέσως
	state->info.paused = false;				// Χωρίς να είναι paused.
	state->info.score = 0;					// Αρχικό σκορ 0
	state->info.missile = NULL;				// Αρχικά δεν υπάρχει πύραυλος
	state->speed_factor = 1;				// Κανονική ταχύτητα

	// Δημιουργία του αεροσκάφους, κεντραρισμένο οριζόντια και με y = 0
	state->info.jet = create_object(JET, (SCREEN_WIDTH - 35)/2,  0, 35, 40);

	// Δημιουργούμε τη λίστα των αντικειμένων, και προσθέτουμε αντικείμενα
	// ξεκινώντας από start_y = 0.
	state->objects = list_create(NULL);
	add_objects(state, 0);

	return state;
}

// Επιστρέφει τις βασικές πληροφορίες του παιχνιδιού στην κατάσταση state

StateInfo state_info(State state) {

	return &state->info;
}

// Επιστρέφει μια λίστα με όλα τα αντικείμενα του παιχνιδιού στην κατάσταση state,
// των οποίων η συντεταγμένη y είναι ανάμεσα στο y_from και y_to.

List state_objects(State state, float y_from, float y_to) {

List list = list_create(NULL);

for(ListNode n = list_first(list); n != LIST_EOF; n = list_next(list,n)) {
    Object oo1 = list_node_value(state->objects, n);
    if(oo1-> rect.y <= y_from && oo1-> rect.y >= y_to){
        list_insert_next(list, LIST_BOF, oo1);
    }
	  
}
    return list;
}

void state_update(State state, KeyState keys) {
	
  if(state->info.playing){

	// Λιστα με ολα τα αντικειμενα κατα μηκος της οθονης 	
	List list = state_objects(state,state->info.jet->rect.y, -100*SCREEN_HEIGHT);



		
	for(ListNode n = list_first(list);
		n != LIST_EOF;
		n = list_next(list,n)) {
	 
		
		Object oo1 = list_node_value(list, n);
		 

		// Ελεγχος για συγκορουσεις αεροσκαφους,ελικοπτερων και πλοιων με εδαφος
		if(oo1->type == TERAIN){
			

			for(ListNode n2 = list_first(list);
				n2 != LIST_EOF;
				n2 = list_next(list,n2)){

				Object oo2 = list_node_value(list, n2);
				
				// Συγκρουση ελικοπτερου η πλοιου με εδαφος
				if(oo2->type == HELICOPTER || oo2->type == WARSHIP){	
					if(CheckCollisionRecs(oo1->rect,oo2->rect)){
						if(oo1->rect.x == 0)	// Συγκρουση με αριστερο εδαφος
							oo2->forward = true;
						else 					// Συγκρουση με δεξι εδαφος
							oo2->forward = false;
					}
				}
			}
		}

		if(oo1->type == TERAIN || oo1->type == HELICOPTER || oo1->type == WARSHIP || oo1->type == BRIDGE)
			if(CheckCollisionRecs(state->info.jet->rect, oo1->rect)){
				state->info.playing = false;
				return;
			}
		
		
		
		if( state->info.missile != NULL ){
			if(CheckCollisionRecs(state->info.missile->rect, oo1->rect )){
				
				state->info.missile = NULL;
				state->info.score += 10;
				list_remove_next(list, n);
				
			}
		}		
	}


	 						
	for(ListNode n = list_first(list);
		n != LIST_EOF;
		n = list_next(list,n)) {
			Object oo2 = list_node_value(list, n);
			if(oo2->type == WARSHIP){
				if(oo2->forward)
					oo2->rect.x += 3;
				else 
					oo2->rect.x -= 3;
			
	
			}
			if(oo2->type == HELICOPTER){
				if(oo2->forward) 
					oo2->rect.x += 4;
				if(!oo2->forward) 
					oo2->rect.x -= 4;
			}
						
	}
    if(keys->space && state->info.missile == NULL)
		state->info.missile = create_object(MISSLE, state->info.jet-> rect.x, state->info.jet-> rect.y + 2, 10,	20);
	// Λειτουργιες Πυραυλου
	if(state->info.missile != NULL){
		state->info.missile->rect.y -= 10 * state->speed_factor;
		if(state->info.missile->rect.y < -100*SCREEN_HEIGHT)
			state->info.missile = NULL;	
	}		

	// Ελεγχος για τερματισμο παιχνιδιου και pause 
	if(!state->info.playing || state->info.paused){
		if(keys->enter){
			state->info.playing = true;
			state->info.paused = false;
		}
		else return;	 
	}

	// Κινηση αεροσκαφους προς τα πανω 
	if (keys->up)
		state->info.jet->rect.y -= 6 * state->speed_factor; 
	if (keys->down)
		state->info.jet->rect.y -= 2 * state->speed_factor; 
	if (keys->left)
		state->info.jet->rect.x -= 3 * state->speed_factor;
	if (keys->right)
		state->info.jet->rect.x += 3 * state->speed_factor;
	if(!keys->down && !keys->up)
		state->info.jet->rect.y -= 3 * state->speed_factor; 
	
	
	// Ελεγχος για pause
	if(keys->p){
		state->info.paused = true;
		return;
	}
  }	
  else 
	if(keys->enter){
		state_destroy(state);
		state_create();
	}
}


void state_destroy(State state){
    list_destroy(state->objects);
    free(state);
}
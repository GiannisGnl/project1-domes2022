#include <stdlib.h>
#include "raylib.h"
#include "state.h"
#include "interface.h"
State state;

void update_end_draw(){
    struct key_state keys = {
        .up = IsKeyDown(KEY_UP),
        .down = IsKeyDown(KEY_DOWN),
	    .left = IsKeyDown(KEY_LEFT),
	    .right = IsKeyDown(KEY_RIGHT),
	    .space = IsKeyDown(KEY_SPACE),
        .enter = IsKeyPressed(KEY_ENTER),
        .n = IsKeyDown(KEY_N),
	    .p = IsKeyPressed(KEY_P)
    };
    state_update(state, &keys);
    interface_draw_frame(state);
}


void main(){

state = state_create();
interface_init();
start_main_loop(update_end_draw);
CloseWindow();

}
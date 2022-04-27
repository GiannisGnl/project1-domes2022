#include <stdio.h>
#include <stdlib.h>
#include "state.h"
#include "interface.h"
#include "raylib.h"
#include <stddef.h>


// Αρχικοποιει το interface του παιχνιδιου 
void interface_init(){
    // Αρχικοποιει το παραθυρο
    InitWindow(SCREEN_HEIGHT,SCREEN_WIDTH,"Riddle Rain");
    SetTargetFPS(60);
}

// Φτιαχνει ενα frame με την κατασταση του παιχνιδιου
void interface_draw_frame(State state){
    BeginDrawing();

    // ολικο wipe
    ClearBackground(GREEN);

    StateInfo info = state_info(state);

    
    int x_offset = SCREEN_WIDTH - 450;
    int y_offset = info->jet->rect.y - 500;

    List list = list_create(NULL);
    list = state_objects(state, -y_offset, info->jet->rect.y -5*SCREEN_HEIGHT);

    for(ListNode n = list_first(list); 
        n != LIST_EOF;
        n = list_next(list,n)) {

        Object oo2 = list_node_value(list,n);
        if(oo2->type == TERAIN){
            // Terain
            DrawRectangle(oo2->rect.x - x_offset, oo2->rect.y - y_offset, oo2->rect.width, oo2->rect.height, GRAY);
            
        }
        else if(oo2->type == BRIDGE){
            // Bridge
            DrawRectangle(oo2->rect.x - x_offset, oo2->rect.y - y_offset, oo2->rect.width, oo2->rect.height, BROWN);
        }

        else{ 
            // Helicopter
            if(oo2->type == HELICOPTER)
                DrawRectangle(oo2->rect.x - x_offset, oo2->rect.y - y_offset, oo2->rect.width, oo2->rect.height, RED);

        
            // warship
            if(oo2->type == WARSHIP)                            
               DrawRectangle(oo2->rect.x - x_offset, oo2->rect.y - y_offset, oo2->rect.width, oo2->rect.height, BLACK);

        }
    }

    //jet
    DrawRectangle(oo2->rect.x - x_offset, oo2->rect.y - y_offset, oo2->rect.width, oo2->rect.height, BLUE);
    //missile
    if(info->missile != NULL)
        DrawRectangle(oo2->rect.x - x_offset, oo2->rect.y - y_offset, oo2->rect.width, oo2->rect.height, WHITE);

    DrawText(TextFormat("%04i", info->score),24,14,24,YELLOW);
    DrawFPS(SCREEN_WIDTH - 80, 0);

    if(!info->playing || info->paused){
        DrawText("PRESS [ENTER] TO KEEP PLAYING",             
        GetScreenWidth()/2 - MeasureText("PRESS ENTER TO PLAY AGAIN",20)/2,
        GetScreenHeight()/2 - 50,20, RAYWHITE);
    }

    EndDrawing();
}
void interface_close(){
    CloseWindow();
}
#ifndef RAYCASTER_H
#define RAYCASTER_H

#include <SDL2/SDL.h>

#include "map.h"
#include "vectors.h"

typedef struct
{
    // sdl
    SDL_Window* sdl_window;
    SDL_Renderer* sdl_renderer;
    SDL_Event* sdl_event;
    SDL_Surface* texture1;
    SDL_Surface* texture2;
    // fps
    Uint64 frame_start_time;
    double min_time_per_frame;
    // window info
    int window_width;
    int window_height;
    // player info
    Vector camera_plane;
    Vector position;
    Vector direction;
    double rotation_speed; // radians per frame
    double movement_speed; // squares per frame
    int up; // currently moving up
    int left; // currently rotating left
    int down; // currently moving up
    int right; // currently rotating right
    
    // game info
    int** map; // does not need the entire MapInfo struct
}
RaycasterData;

/* 
Initializes the raycaster
Returns the pointer to RaycasterData needed for the other functions in this file
Returns NULL on failure
player_start_rotation is in radians compared to facing in the direction of the y axis
In the map: 0 represents an empty square, 1 represents a wall, 2 represents a wall with a different texture and without collision
destroy_raycaster should be called once the raycaster isn't needed anymore
*/
RaycasterData* initialize_raycaster(int fps_cap, int window_width, int window_height, int fov, double player_start_x, double player_start_y, double player_start_rotation, double rotation_speed, double movement_speed, MapInfo* map, char* texture1_file, char* texture2_file);

/*
Ends the raycaster cleanly
*/
void destroy_raycaster(RaycasterData* raycaster_data);

/*
Handles player input (moving and exiting)
Returns 2 if the player is exiting the app
Returns 1 if the player has moved
Returns 0 otherwise (you can use this to avoid useless redrawing)
*/
int handle_input(RaycasterData* raycaster_data);

/*
Renders the frame and sleeps until it's time for the next frame
*/
void render_and_sleep(RaycasterData* raycaster_data);

/*
Returns the value of the square the player is standing on
*/
int get_current_square_value(RaycasterData* raycaster_data);

/*
Cover the entire screen with an image and wait for a key press
Returns 1 if the player quits the app
Returns 0 if the player presses a key
Returns -1 if there is an error loading the image
*/
int screen(RaycasterData* raycaster_data, char* path);

#endif

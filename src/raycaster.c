#include <math.h>
#include <stdio.h>

#include "raycaster.h"



// Uses the DDA algorithm
RaycasterData* initialize_raycaster(int fps_cap, int window_width, int window_height, int fov, double player_start_x, double player_start_y, double player_start_rotation, double rotation_speed, double movement_speed, MapInfo* map)
{
    // INITIALIZE RETURN POINTER

    RaycasterData* raycaster_data = malloc(sizeof(RaycasterData));
    if (!raycaster_data)
    {
        printf("error attempting to allocate memory for RaycasterData\n");
        return NULL;
    }

    // INITIALIZE SDL
    
    // init sdl
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
    {
        printf("sdl init error: %s\n", SDL_GetError());
        free(raycaster_data);
        return NULL;
    }

    // create window
    raycaster_data->sdl_window = SDL_CreateWindow("Maze", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, window_width, window_height, 0);
    if (!raycaster_data->sdl_window)
    {
        printf("sdl window creation error: %s\n", SDL_GetError());
        SDL_Quit();
        free(raycaster_data);
        return NULL;
    }
    
    // create renderer
    raycaster_data->sdl_renderer = SDL_CreateRenderer(raycaster_data->sdl_window, -1,  SDL_RENDERER_ACCELERATED);
    if (!raycaster_data->sdl_renderer)
    {
        printf("sdl renderer creation error: %s\n", SDL_GetError());
        SDL_DestroyWindow(raycaster_data->sdl_window);
        SDL_Quit();
        free(raycaster_data);
        return NULL;
    }

    // create event handler
    raycaster_data->sdl_event = malloc(sizeof(SDL_Event));
    if (!raycaster_data->sdl_event)
    {
        printf("error attempting to allocate memory for sdl event in RaycasterData\n");
        return NULL;
    }
    SDL_Event event;
    memcpy(raycaster_data->sdl_event, &event, sizeof(SDL_Event));

    // INITIALIZE FRAME TIME INFO

    raycaster_data->min_time_per_frame = 1000.0 / fps_cap;
    raycaster_data->frame_start_time = SDL_GetTicks64(); // render_and_sleep needs this value to be initialized, it does not matter if it isn't the correct start time this time

    // INITIALIZE WINDOW INFO

    raycaster_data->window_width = window_width;
    raycaster_data->window_height = window_height;

    // INITIALIZE PLAYER INFO

    // camera plane (fov)
    raycaster_data->camera_plane = (Vector) {tan((M_PI*fov)/360), 0};

    // direction
    raycaster_data->direction = (Vector) {0, 1};
    rotate_vector(&raycaster_data->direction, player_start_rotation);
    rotate_vector(&raycaster_data->camera_plane, player_start_rotation);
    
    // position
    raycaster_data->position = (Vector) {player_start_x, player_start_y};

    // rotation speed
    raycaster_data->rotation_speed = rotation_speed; // radians per frame

    // movement speed
    raycaster_data->movement_speed = movement_speed; // squares per frame

    // INITIALIZE GAME INFO

    raycaster_data->map = map->map;

    return raycaster_data;
}


void destroy_raycaster(RaycasterData* raycaster_data)
{
        SDL_DestroyRenderer(raycaster_data->sdl_renderer);
        SDL_DestroyWindow(raycaster_data->sdl_window);
        SDL_Quit();
        free(raycaster_data->sdl_event);
        free(raycaster_data);
}


int handle_input(RaycasterData* raycaster_data)
{
    // GET INPUT

    while (SDL_PollEvent(raycaster_data->sdl_event) != 0)
    {
        switch (raycaster_data->sdl_event->type)
        {
            // quit
            case SDL_QUIT: return 2;
            // move
            case SDL_KEYDOWN:
                switch (raycaster_data->sdl_event->key.keysym.scancode)
                {
                    case SDL_SCANCODE_W: raycaster_data->up = 1; break;
                    case SDL_SCANCODE_A: raycaster_data->left = 1; break;
                    case SDL_SCANCODE_S: raycaster_data->down = 1; break;
                    case SDL_SCANCODE_D: raycaster_data->right = 1; break;
                }
                break;
            // stop moving
            case SDL_KEYUP:
                switch (raycaster_data->sdl_event->key.keysym.scancode)
                {
                    case SDL_SCANCODE_W: raycaster_data->up = 0; break;
                    case SDL_SCANCODE_A: raycaster_data->left = 0; break;
                    case SDL_SCANCODE_S: raycaster_data->down = 0; break;
                    case SDL_SCANCODE_D: raycaster_data->right = 0; break;
                }
                break;
        }
    }

    // HANDLE MOVEMENT

    Vector new = {raycaster_data->position.x, raycaster_data->position.y}; // player x and y after moving
    if (raycaster_data->up)
    {
        new.x += raycaster_data->movement_speed * raycaster_data->direction.x;
        new.y += raycaster_data->movement_speed * raycaster_data->direction.y;
    }
    if (raycaster_data->down)
    {
        new.x -= raycaster_data->movement_speed * raycaster_data->direction.x;
        new.y -= raycaster_data->movement_speed * raycaster_data->direction.y;
    }
    if (raycaster_data->right)
    {
        rotate_vector(&(raycaster_data->direction), -raycaster_data->rotation_speed);
        rotate_vector(&(raycaster_data->camera_plane), -raycaster_data->rotation_speed);
    }
    if (raycaster_data->left)
    {
        rotate_vector(&(raycaster_data->direction), raycaster_data->rotation_speed);
        rotate_vector(&(raycaster_data->camera_plane), raycaster_data->rotation_speed);
    }
    if (raycaster_data->up || raycaster_data->down || raycaster_data->right || raycaster_data->left)
    {
        if (!(raycaster_data->map[(int) new.x][(int) raycaster_data->position.y] == 1)) raycaster_data->position.x = new.x;
        if (!(raycaster_data->map[(int) raycaster_data->position.x][(int) new.y] == 1)) raycaster_data->position.y = new.y;
        return 1;
    }

    return 0;
}


static double cast(RaycasterData* raycaster_data, int x)
{
    // set the direction of the ray
    // -1 to 1, representing where the screen x is in the camera plane
    double camera_fraction = -1 + 2 * x / (double) raycaster_data->window_width;
    Vector ray =
    {
        raycaster_data->direction.x + camera_fraction * raycaster_data->camera_plane.x,
        raycaster_data->direction.y + camera_fraction * raycaster_data->camera_plane.y
    };

    // dda
    double number_for_one_x = (ray.x != 0) ? fabs(1 / ray.x) : 1e10; // number of ray vectors to cross one x
    double number_for_one_y = (ray.y != 0) ? fabs(1 / ray.y) : 1e10; // number of ray vectors to cross one y
    double number_to_next_x; // number of ray vectors to go from the player to the next integer x to check
    double number_to_next_y; // number of ray vectors to go from the player to the next integer y to check
    int ray_x_increment; // amount to increment x by, 1 if ray x is above 0, -1 if below 0 (will not be needed if ray x is 0)
    int ray_y_increment; // amount 1 increment y by, if ray y is above 0, -1 if below 0 (will not be needed if ray y is 0)
    if (ray.x > 0)
    {
        ray_x_increment = 1;
        number_to_next_x = (1 - raycaster_data->position.x + (int) raycaster_data->position.x) * number_for_one_x;
    }
    else
    {
        ray_x_increment = -1;
        number_to_next_x = (raycaster_data->position.x - (int) raycaster_data->position.x) * number_for_one_x;
    }
    if (ray.y > 0)
    {
        ray_y_increment = 1;
        number_to_next_y = (1 - raycaster_data->position.y + (int) raycaster_data->position.y) * number_for_one_y;
    }
    else
    {
        ray_y_increment = -1;
        number_to_next_y = (raycaster_data->position.y - (int) raycaster_data->position.y) * number_for_one_y;
    }
    int found_wall = 0;
    int x_check = raycaster_data->position.x; // x that is currently being checked for a square
    int y_check = raycaster_data->position.y; // y that is currently being checked for a square
    int last_incremented; // 0 if x was incremented last, 1 if y was incremented last
    while (!found_wall)
    {
        if (number_to_next_x < number_to_next_y)
        {
            number_to_next_x += number_for_one_x;
            x_check += ray_x_increment;
            if (raycaster_data->map[x_check][y_check])
            {
                last_incremented = 0;
                found_wall = 1;
            }
        }
        else
        {
            number_to_next_y += number_for_one_y;
            y_check += ray_y_increment;
            if (raycaster_data->map[x_check][y_check])
            {
                last_incremented = 1;
                found_wall = 1;
            }
        }
    }

    // find the distance
    double distance = (last_incremented) ? number_to_next_y - number_for_one_y : number_to_next_x - number_for_one_x;

    return (raycaster_data->map[x_check][y_check] == 1) ? distance : -distance;
}


int render_and_sleep(RaycasterData* raycaster_data)
{
    // render
    SDL_SetRenderDrawColor(raycaster_data->sdl_renderer, 0, 0, 0, 255);
    SDL_RenderClear(raycaster_data->sdl_renderer);
    SDL_SetRenderDrawColor(raycaster_data->sdl_renderer, 100, 100, 100, 255);
    for (int x = 0; x < raycaster_data->window_width; x++) // loop over all window columns
    {
        double distance = cast(raycaster_data, x);
        if (distance < 0)
        {
            distance = fabs(distance);
            SDL_SetRenderDrawColor(raycaster_data->sdl_renderer, 0, 100, 0, 255);
        }
        if (distance > 1)
        {
            int height = raycaster_data->window_height / distance; // height of the line
            int startY = (raycaster_data->window_height - height) / 2; // start y of the line
            SDL_RenderDrawLine(raycaster_data->sdl_renderer, x, startY, x, startY + height);
        }
        else SDL_RenderDrawLine(raycaster_data->sdl_renderer, x, 0, x, raycaster_data->window_height);
        SDL_SetRenderDrawColor(raycaster_data->sdl_renderer, 100, 100, 100, 255);
    }
    SDL_RenderPresent(raycaster_data->sdl_renderer);

    // sleep
    Uint64 time = SDL_GetTicks64() - raycaster_data->frame_start_time;
    if (time < raycaster_data->min_time_per_frame) SDL_Delay(raycaster_data->min_time_per_frame - time);
    raycaster_data->frame_start_time = SDL_GetTicks64();
}



int get_current_square_value(RaycasterData* raycaster_data)
{
    return raycaster_data->map[(int) raycaster_data->position.x][(int) raycaster_data->position.y];
}

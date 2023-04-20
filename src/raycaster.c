#include <math.h>
#include <stdio.h>
#include <SDL2/SDL_image.h>

#include "raycaster.h"


typedef struct
{
    double distance; // distance traveled by the ray
    int hit_texture_id; // id of the texture of the block that was hit
    double hit_x; // x on the texture of the block
}
RayResults;



// Uses the DDA algorithm
RaycasterData* initialize_raycaster(int fps_cap, int window_width, int window_height, int fov, double player_start_x, double player_start_y, double player_start_rotation, double rotation_speed, double movement_speed, MapInfo* map, char* texture1_file, char* texture2_file)
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

    // load textures
    raycaster_data->texture1 = IMG_Load(texture1_file);
    if (!raycaster_data->texture1)
    {
        printf("error attempting to load texture 1 (%s)\n", texture1_file);
        return NULL;
    }
    raycaster_data->texture2 = IMG_Load(texture2_file);
    if (!raycaster_data->texture2)
    {
        printf("error attempting to load texture 2 (%s)\n", texture2_file);
        return NULL;
    }

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
        SDL_FreeSurface(raycaster_data->texture1);
        SDL_FreeSurface(raycaster_data->texture2);
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


static void cast(RaycasterData* raycaster_data, int x, RayResults* ray_results)
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
    while (!found_wall)
    {
        if (number_to_next_x < number_to_next_y)
        {
            x_check += ray_x_increment;
            if (raycaster_data->map[x_check][y_check])
            {
                found_wall = 1;
                ray_results->distance = number_to_next_x;
                ray_results->hit_x = raycaster_data->position.y + number_to_next_x * ray.y;
            }
            else number_to_next_x += number_for_one_x;
        }
        else
        {
            y_check += ray_y_increment;
            if (raycaster_data->map[x_check][y_check])
            {
                found_wall = 1;
                ray_results->distance = number_to_next_y;
                ray_results->hit_x = raycaster_data->position.x + number_to_next_y * ray.x;
            }
            else number_to_next_y += number_for_one_y;
        }
    }
    ray_results->hit_x -= floor(ray_results->hit_x);
    ray_results->hit_texture_id = raycaster_data->map[x_check][y_check];
}


int render_and_sleep(RaycasterData* raycaster_data)
{
    // render
    // clear the background, make it floor colored
    SDL_SetRenderDrawColor(raycaster_data->sdl_renderer, 75, 0, 0, 255);
    SDL_RenderClear(raycaster_data->sdl_renderer);
    // draw the roof
    SDL_SetRenderDrawColor(raycaster_data->sdl_renderer, 22, 9, 0, 255);
    SDL_Rect rect;
    rect.x = 0;
    rect.y = 0;
    rect.w = raycaster_data->window_width;
    rect.h = raycaster_data->window_height / 2;
    SDL_RenderFillRect(raycaster_data->sdl_renderer, &rect);
    // loop over all window columns
    for (int x = 0; x < raycaster_data->window_width; x++)
    {
        RayResults ray_results;
        cast(raycaster_data, x, &ray_results);
        SDL_Surface* texture = (ray_results.hit_texture_id - 1) ? raycaster_data->texture2 : raycaster_data->texture1;
        int height = raycaster_data->window_height / ray_results.distance; // perceived height of the wall
        int texture_x = ray_results.hit_x * texture->w;
        double texture_y; // double because increments will be smaller than 1
        double texture_y_increment = (double) texture->h / height;
        int start_y; // start y of the wall on the screen
        int end_y; // end y of the wall on the screen
        if (raycaster_data->window_height > height)
        {
            start_y = (raycaster_data->window_height - height) / 2;
            end_y = start_y + height;
            texture_y = 0;
        }
        else
        {
            start_y = 0;
            end_y = raycaster_data->window_height;
            texture_y = (height / 2 - raycaster_data->window_height / 2) * texture_y_increment;
        }
        // loop over all rows that have to be drawn
        for (int y = start_y; y < end_y; y++)
        {
            Uint8 r, g, b;
            Uint8* pixel = (Uint8*) texture->pixels + (int) texture_y * texture->pitch + texture_x * texture->format->BytesPerPixel;
            SDL_GetRGB(*(Uint32*)pixel, texture->format, &r, &g, &b);
            SDL_SetRenderDrawColor(raycaster_data->sdl_renderer, r, g, b, 255);
            SDL_RenderDrawPoint(raycaster_data->sdl_renderer, x, y);
            texture_y += texture_y_increment;
        }
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

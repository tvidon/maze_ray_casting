#include <time.h>

#include "map.h"
#include "raycaster.h"



int main(int argc, char **argv)
{
    // parameters
    int fps_cap = 60;
    int window_width = 640;
    int window_height = 480;
    int fov = 60;
    double rotation_speed = 3.0 / fps_cap;
    double movement_speed = 3.0 / fps_cap;
    int map_size = 21; // will be increased to 5 if under 5, will be made odd by subtracting 1 if even
    unsigned int seed = (argc == 2) ? atoi(argv[1]) : time(0);
    srand(seed);

    // generate map
    MapInfo *map = generate_map(map_size);
    if (!map) return 1;

    // print map info
    printf("XX is your starting location, the exit is in the bottom right corner\n");
    for (int x = 0; x < map->size; x++)
    {
        for (int y = 0; y < map->size; y++)
        {
            if (map->map[x][y] == 1) printf("||");
            else if (x == 1 && y == 1) printf("XX");
            else printf("  ");
        }
        printf("\n");
    }
    printf("seed: %d\nYou can use this seed again by starting the game with `./main %d`\n", seed, seed);

    // initialize raycaster
    RaycasterData *raycaster = initialize_raycaster(fps_cap, window_width, window_height, fov, 1.5, 1.5, -0.785, rotation_speed, movement_speed, map, "./res/wall.png", "./res/door.png");
    if (!raycaster) return 1;

    // start screen
    if (screen(raycaster, "./res/start_screen.png") == 1)
    {
        destroy_raycaster(raycaster);
        destroy_map(map);
        return 0;
    }

    // main loop
    render_and_sleep(raycaster);
    while (1)
    {
        int received_input = handle_input(raycaster);
        // only update if player moves
        if (received_input == 1)
        {
            // if victory
            if (get_current_square_value(raycaster) == 2)
            {
                printf("Exit\n");
                screen(raycaster, "./res/victory_screen.png");
                break;
            }
            // else render and wait until end of frame
            render_and_sleep(raycaster);
        }
        // player quits
        else if (received_input == 2) break;
    }

    // cleanup
    destroy_raycaster(raycaster);
    destroy_map(map);
    return 0;
}

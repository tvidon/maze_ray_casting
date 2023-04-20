#include "map.h"
#include "raycaster.h"
#include <time.h>



int main(int argc, char** argv)
{
    int fps_cap = 60;
    int window_width = 640;
    int window_height = 480;
    int fov = 60;
    double rotation_speed = 3.0 / fps_cap;
    double movement_speed = 3.0 / fps_cap;
    int map_size = 21; // will be increased to 5 if under 5, will be made odd by subtracting 1 if even
    int print_map = 1;
    unsigned int seed = (argc == 2) ? atoi(argv[1]) : time(0);
    srand(seed);

    MapInfo* map = generate_map(map_size);
    if (!map) return 1;
    if (print_map)
    {
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
        printf("seed: %d\n", seed);
    }

    RaycasterData* raycaster = initialize_raycaster(fps_cap, window_width, window_height, fov, 1.5, 1.5, -0.785, rotation_speed, movement_speed, map, "./res/wall.png", "./res/door.png");
    if (!raycaster) return 1;

    render_and_sleep(raycaster);
    while (1)
    {
        int received_input = handle_input(raycaster);
        if (received_input == 1)
        {
            if (get_current_square_value(raycaster) == 2)
            {
                printf("Exit\n");
                break;
            }
            render_and_sleep(raycaster);
        }
        else if (received_input == 2) break;
    }

    destroy_raycaster(raycaster);
    destroy_map(map);
}

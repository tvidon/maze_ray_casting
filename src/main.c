#include "map.h"
#include "raycaster.h"
#include <time.h>



int main(void)
{
    int fps_cap = 60;
    int window_width = 640;
    int window_height = 480;
    int fov = 60;
    double player_start_x = 1;
    double player_start_y = 1;
    double player_start_rotation = -0.785; // ~= 45 degrees
    double rotation_speed = 3.0 / fps_cap;
    double movement_speed = 3.0 / fps_cap;
    int map_size = 15;
    int print_map = 1;
    unsigned int seed= time(0);
    srand(seed);

    MapInfo* map = generate_map(map_size);
    if (!map) return 1;
    if (print_map)
    {
        for (int x = 0; x < map->size; x++)
        {
            for (int y = 0; y < map->size; y++)
            {
                if (map->map[x][y] == 1) printf("xx");
                else printf("  ");
            }
            printf("\n");
        }
        printf("seed: %d\n", seed);
    }

    RaycasterData* raycaster = initialize_raycaster(fps_cap, window_width, window_height, fov, player_start_x, player_start_y, player_start_rotation, rotation_speed, movement_speed, map);
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

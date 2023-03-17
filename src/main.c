#include "map.h"
#include "raycaster.h"



int main(void)
{
    int fps_cap = 60;
    int window_width = 640;
    int window_height = 480;
    int fov = 60;
    double player_start_x = 1;
    double player_start_y = 1;
    double player_start_rotation = 0;
    double rotation_speed = 3.0 / fps_cap;
    double movement_speed = 3.0 / fps_cap;

    int** map = generate_map(10);
    if (!map) return 1;

    RaycasterData* raycaster = initialize_raycaster(fps_cap, window_width, window_height, fov, player_start_x, player_start_y, player_start_rotation, rotation_speed, movement_speed, map);
    if (!raycaster) return 1;

    int run = 1;
    render_and_sleep(raycaster);
    while (run)
    {
        int received_input = handle_input(raycaster);
        if (received_input == 1)
        {
            render_and_sleep(raycaster);
            if (get_current_square_value(raycaster) == 2)
            {
                run = 0;
                printf("Exit\n");
            }
        }
        else if (received_input == 2) run = 0;
    }

    destroy_raycaster(raycaster);
    destroy_map(map);
}

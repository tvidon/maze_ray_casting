#include <SDL2/SDL.h>
#include <stdio.h>



#define WINDOW_WIDTH (640)
#define WINDOW_HEIGHT (480)
#define TIME_PER_FRAME (1000/60)



int main(){

    // INITIALISATION
    
    // init sdl
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
    {
        printf("sdl init error: %s\n", SDL_GetError());
        return 1;
    }

    // create window
    SDL_Window* win = SDL_CreateWindow("Maze", SDL_WINDOWPOS_UNDEFINED,
                                       SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT,
                                       0);
    if (!win)
    {
        printf("sdl window creation error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    
    // create renderer
    SDL_Renderer* rend = SDL_CreateRenderer(win, -1,  SDL_RENDERER_ACCELERATED);
    if (!rend)
    {
        printf("sdl renderer creation error: %s\n", SDL_GetError());
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }


    // GAME
    
    // exit
    int exit = 0;

    // input
    int up = 0;
    int down = 0;
    int left = 0;
    int right = 0;

    // game loop
    while (!exit)
    {
        int startTime = SDL_GetTicks();

        // input
        SDL_Event event;
        int update = 0;
        while (SDL_PollEvent(&event) != 0)
        {
            switch (event.type)
            {
                // quit
                case SDL_QUIT:
                    exit = 1;
                    break;
                // move
                case SDL_KEYDOWN:
                    switch (event.key.keysym.scancode)
                    {
                        case SDL_SCANCODE_W:
                            up = 1;
                            update = 1;
                            break;
                        case SDL_SCANCODE_A:
                            left = 1;
                            update = 1;
                            break;
                        case SDL_SCANCODE_S:
                            down = 1;
                            update = 1;
                            break;
                        case SDL_SCANCODE_D:
                            right = 1;
                            update = 1;
                            break;
                    }
                    break;
                // stop moving
                case SDL_KEYUP:
                    switch (event.key.keysym.scancode)
                    {
                        case SDL_SCANCODE_W:
                            up = 0;
                            break;
                        case SDL_SCANCODE_A:
                            left = 0;
                            break;
                        case SDL_SCANCODE_S:
                            down = 0;
                            break;
                        case SDL_SCANCODE_D:
                            right = 0;
                            break;
                    }
                    break;
            }
        }

        if (update)
        {
            // move
            update = 0;
            // TODO
            printf("up: %i down: %i left: %i right: %i\n", up, down, left, right);
           
            // draw
            SDL_RenderClear(rend);
            // TODO
            SDL_RenderPresent(rend);
        }

        // wait for the end of the frame
        int time = SDL_GetTicks() - startTime;
        if (time < TIME_PER_FRAME){
            SDL_Delay(TIME_PER_FRAME - time);
        }
    }


    // CLEAN UP
    
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();
}

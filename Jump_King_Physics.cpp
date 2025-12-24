#include <iostream>
#include <SDL3/SDL.h>

const int sdl_width{ 900 };
const int sdl_height{ 600 };

const float hitbox_width{ 50 };
const float hitbox_height{ 75 };

float player_x{ sdl_width / 2 };
float player_y{ sdl_height - hitbox_height };

const float moveSpeed{ 3 };

const float groundHeight{ sdl_height - hitbox_height };
bool isGrounded{ true };

float charge{ 0 };
const float maxCharge{ 35 };
const float minCharge{ maxCharge / 10 };

float velocity_x{ 0 };
float velocity_y{ 0 };


float gravity{ 2 };

void move()
{
    const bool* key_states = SDL_GetKeyboardState(NULL);

    if (!isGrounded || player_y < groundHeight) velocity_y += gravity;

    // Y bounds check
    if (player_y >= groundHeight)
    {
        isGrounded = true;
        velocity_y = 0;
        if (velocity_x != 0) velocity_x = 0;
        player_y = groundHeight;
    }
    if (player_y <= 0) player_y = 0;
    
    // X bounds check
    if (player_x < 0) player_x = 0;
    if (player_x > sdl_width - hitbox_width) player_x = sdl_width - hitbox_width;

    // walk
    if (isGrounded && player_x <= sdl_width - hitbox_width && key_states[SDL_SCANCODE_RIGHT]) player_x += moveSpeed;
    if (isGrounded && player_x >= 0 && key_states[SDL_SCANCODE_LEFT]) player_x -= moveSpeed;

    // jump
    while (isGrounded && key_states[SDL_SCANCODE_SPACE])
    {
        charge += 0.000005; // this is so dumb but it works

        if (charge >= maxCharge)
        {
            isGrounded = false;
            velocity_y = -maxCharge;

            if (key_states[SDL_SCANCODE_RIGHT])
            {
                velocity_x = maxCharge / 2;
            }
            else if (key_states[SDL_SCANCODE_LEFT])
            {
                velocity_x = -maxCharge / 2;
            }

            charge = 0;
            break;
        }
        //SDL_Delay(1);
        SDL_PumpEvents();
    }

    if (isGrounded && charge > minCharge)
    {
        isGrounded = false;
        velocity_y = -charge;

        if (key_states[SDL_SCANCODE_RIGHT])
        {
            velocity_x = charge / 2;
        }
        else if (key_states[SDL_SCANCODE_LEFT])
        {
            velocity_x = -charge / 2;
        }
    }
    charge = 0;

    // apply velocities
    if (player_x >= 0 && player_x <= sdl_width - hitbox_width)
    {
        player_x += velocity_x;
    }

    if (player_y <= groundHeight && player_y >= 0)
    {
        player_y += velocity_y;
    }
}

int main()
{
    bool done = false;

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = nullptr;

    SDL_Renderer* renderer = nullptr;

    SDL_Event event;

    SDL_CreateWindowAndRenderer("JK Physics", sdl_width, sdl_height, 0, &window, &renderer);
    
    Uint64 lastTime{ 0 };
    Uint64 currentTime{ 0 };

    // Game loop
    while (true)
    {
        SDL_PollEvent(&event);
        if (event.type == SDL_EVENT_QUIT)
        {
            break;
        }

        // Clear renderer
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
        SDL_RenderClear(renderer);

        SDL_FRect player{ player_x, player_y, hitbox_width, hitbox_height };

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &player);

        currentTime = SDL_GetTicks();
        if (currentTime > lastTime + 16) // ~60 fps
        {
            move();
            lastTime = currentTime;
        }


        SDL_RenderPresent(renderer); // LAST

        
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
#include "chip8.h"
#include <stdio.h>
#include <string.h>
#include <SDL2/SDL.h>

void update_texture(Chip8 *chip8, SDL_Texture *texture){
    uint32_t pixels[DISPLAY_WIDTH * DISPLAY_HEIGHT];

    uint32_t fg_color = 0xFFFFFFFF;  // white (foreground - pixel ON)
    uint32_t bg_color = 0x00000000;  // black (background - pixel OFF)

    for (int i = 0; i < DISPLAY_WIDTH * DISPLAY_HEIGHT; i++) {
        pixels[i] = chip8->display[i] ? fg_color : bg_color;
    }

    SDL_UpdateTexture(texture, NULL, pixels, DISPLAY_WIDTH * sizeof(uint32_t));
}

void handle_input(Chip8 *chip8, SDL_Event *event){
    int i;

SDL_Scancode keymap[16] = {
    SDL_SCANCODE_X,    // 0x0 → X
    SDL_SCANCODE_1,    // 0x1 → 1
    SDL_SCANCODE_2,    // 0x2 → 2
    SDL_SCANCODE_3,    // 0x3 → 3
    SDL_SCANCODE_Q,    // 0x4 → Q
    SDL_SCANCODE_W,    // 0x5 → W
    SDL_SCANCODE_E,    // 0x6 → E
    SDL_SCANCODE_A,    // 0x7 → A
    SDL_SCANCODE_S,    // 0x8 → S
    SDL_SCANCODE_D,    // 0x9 → D
    SDL_SCANCODE_Z,    // 0xA → Z
    SDL_SCANCODE_C,    // 0xB → C
    SDL_SCANCODE_4,    // 0xC → 4
    SDL_SCANCODE_R,    // 0xD → R
    SDL_SCANCODE_F,    // 0xE → F
    SDL_SCANCODE_V     // 0xF → V
};

    if(event->type == SDL_KEYDOWN || event->type == SDL_KEYUP){
        for (i = 0; i < 16; i++){
            if(event->key.keysym.scancode == keymap[i]){
                chip8->keypad[i] = (event->type == SDL_KEYDOWN) ? 1 : 0;
                //break for useless continuation of the loop
                break;
            }
        }
    }
}

int main(){
    int i;
    struct timespec start_time, current_time;
    long elapsed_ms;

    static int frame = 0;
    


    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window = SDL_CreateWindow(
        "Test Window",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        640, 480,
        SDL_WINDOW_SHOWN
    );

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, 
                                         SDL_TEXTUREACCESS_STREAMING, 
                                         DISPLAY_WIDTH, DISPLAY_HEIGHT);

    Chip8 chip8;

    chip8_init(&chip8);

    chip8_load_rom(&chip8, "Pong (1 player).ch8");

    SDL_Event event;
    int running = 1;
    while (running) {
        frame++;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
            handle_input(&chip8, &event);
        }

        if (frame % 60 == 0) {
            printf("Frame %d - PC: 0x%03X\n", frame, chip8.pc);
        fflush(stdout);
}

        //run the loop 10 times to get 600MHz cpu
        for(i = 0; i < 10; i++){
            chip8_execute_cycle(&chip8);
        }

        if (chip8.delay_timer > 0) chip8.delay_timer--;
        if (chip8.sound_timer > 0) {
            chip8.sound_timer--;
        if (chip8.sound_timer == 0) printf("\a");
        }

        update_texture(&chip8, texture);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);

        SDL_Delay(16); // ~60 FPS
    }
    
    // Cleanup
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
#include "chip8.h"
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <SDL2/SDL.h>

void draw_display(Chip8 *chip8){
    int y, x;

    printf("\033[H\033[J");
    for (y = 0; y < DISPLAY_HEIGHT; y++){
        for (x = 0; x < DISPLAY_WIDTH; x++){
            if (chip8->display[x + (y * DISPLAY_WIDTH)] == 1){
                printf("█");
            }else{
                printf(" ");
            }
        }
        printf("\n");
    }
    fflush(stdout);
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
    struct timespec start_time, current_time;
    long elapsed_ms;

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

    chip8_load_rom(&chip8, "test_opcode.ch8");

    // clock_gettime(CLOCK_MONOTONIC, &start_time);
    // while(1){
    //     chip8_execute_cycle(&chip8);

    //     clock_gettime(CLOCK_MONOTONIC, &current_time);

    //     long elapsed_ms = ((current_time.tv_sec - start_time.tv_sec) * 1000)
    //                     + ((current_time.tv_nsec - start_time.tv_nsec) / 1000000);
        
    //     if(elapsed_ms >= 16){
    //         if(chip8.delay_timer > 0){
    //             chip8.delay_timer--;
    //         }

    //         if(chip8.sound_timer > 0){
    //             chip8.sound_timer--;
    //             if(chip8.sound_timer == 0){
    //                 printf("\a");
    //             }
    //         }
    //     }
    // }

    SDL_Event event;
    int running = 1;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
            handle_input(&chip8, &event);
        }
        SDL_Delay(16); // ~60 FPS
    }
    
    // Cleanup
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
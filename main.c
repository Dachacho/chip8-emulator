#include "chip8.h"
#include <stdio.h>
#include <time.h>
#include <string.h>

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

void handle_input(Chip8 *chip8){
    int i;
    char c;

    char keymap[16] = {
        '1', '2', '3', '4',
        'q', 'w', 'e', 'r',
        'a', 's', 'd', 'f',
        'z', 'x', 'c', 'v'
    };

    memset(chip8->keypad, 0, sizeof(chip8->keypad));

    c = getchar();
    for (i = 0; i < 16; i++){
        if (c == keymap[i]){
            chip8->keypad[i] = 1;
        }
    }
}

int main(){
    struct timespec start_time, current_time;
    long elapsed_ms;

    Chip8 chip8;

    chip8_init(&chip8);

    chip8_load_rom(&chip8, "test_opcode.ch8");

    clock_gettime(CLOCK_MONOTONIC, &start_time);
    while(1){
        chip8_execute_cycle(&chip8);

        clock_gettime(CLOCK_MONOTONIC, &current_time);

        long elapsed_ms = ((current_time.tv_sec - start_time.tv_sec) * 1000)
                        + ((current_time.tv_nsec - start_time.tv_nsec) / 1000000);
        
        if(elapsed_ms >= 16){
            if(chip8.delay_timer > 0){
                chip8.delay_timer--;
            }

            if(chip8.sound_timer > 0){
                chip8.sound_timer--;
                if(chip8.sound_timer == 0){
                    printf("\a");
                }
            }
        }

        handle_input(&chip8);
        draw_display(&chip8);
    }
}
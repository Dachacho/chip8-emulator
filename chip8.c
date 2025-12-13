#include "chip8.h"
#include <string.h>
#include <stdio.h>

const unsigned char fontset[FONTSET_SIZE] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

void chip8_init(Chip8 *chip8){
    memset(chip8, 0, sizeof(Chip8)); //clear all memory on init
    chip8->pc = START_ADDR;
    chip8->sp = 0;
    chip8->I = 0;
    chip8->delay_timer = 0;
    chip8->sound_timer = 0;

    //fontset starts from the memory address 0x50
    //we copy fontset into the memory of our chip
    memcpy(&chip8->memory[0x50], fontset, FONTSET_SIZE);
}

void chip8_load_rom(Chip8 *chip8, const char *filename){
    FILE *fp = fopen(filename, "rb");
    if(!fp){
        printf("FAILED TO LOAD ROM %s\n", filename);
        return;
    }

    //load rom into the memory starting from START_ADDR(0x200)
    size_t bytes_read = fread(&chip8->memory[START_ADDR], 1, MEM_SIZE - START_ADDR, fp);

    if(bytes_read == 0){
        printf("FAILED TO READ ROM %s\n", filename);
    }else{
        printf("LOADED ROM %s (%zu bytes)\n", filename, bytes_read);
    }

    fclose(fp);
}

int main(){
    Chip8 chip8;

    chip8_init(&chip8);
    chip8_load_rom(&chip8, "dummy.rom");

    printf("memory at 0x50 (fontset):\n");
    for(int i = 0x50; i < 0x50 + FONTSET_SIZE; i++){
        printf("0x%02X", chip8.memory[i]);
        if ((i - 0x50 + 1) % 5 == 0){
            printf("\n");   
        } 
    }

    printf("memory at START_ADDR (first 8 bytes):\n");
    for(int i = START_ADDR; i < START_ADDR + 8; i++){
        printf("0x%02X", chip8.memory[i]);
    }
    printf("\n");
}

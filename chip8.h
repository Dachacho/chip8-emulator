//this ifndef could be changed with pragma once 
//but is modern and not standard
#ifndef CHIP8_H
#define CHIP8_H

#define MEM_SIZE 4096
#define STACK_SIZE 16
#define NUM_REGISTERS 16
#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32
#define FONTSET_SIZE 80
#define KEYPAD_SIZE 16
//starting address (512 bits) before this is where the interpreter lives
#define START_ADDR 0x200

typedef struct{
    //is the memory of the chip
    //unsigned char because each memory location is 1 byte
    unsigned char memory[MEM_SIZE];
    //represents registers of the chip
    //same with the unsigned char each register is 8 bytes long
    unsigned char V[NUM_REGISTERS]; 
    //index register used to store memory addresses
    //unsigned short because memory adresses cant exceed 16bits
    unsigned short I;
    //program counter which tracks current instruction being executed
    //program counter holds memory addresses which are mainly 12bits (same reason as above)
    unsigned short pc;
    //timer that counts down from 60hz
    unsigned char delay_timer;
    //timer that counts down at 60hz emits sound when reaches 0
    unsigned char sound_timer;
    //stack is used for storing return addresses when calling subrountines
    //short because memory addresses as explained above are 12bits
    unsigned short stack[STACK_SIZE];
    //pointer to the stack which points to the top of the stack
    unsigned short sp;
    //chip8 display
    //unsigned char array becasuse each pixel is either on(1) or off(2) so 1 byte is enough
    unsigned char display[DISPLAY_WIDTH * DISPLAY_HEIGHT];
    //chip8 keyboard
    //char because its either pressed(1) or not(2)
    unsigned char keypad[KEYPAD_SIZE];
} Chip8;

//initialize the chip, clear memory, set pc...
void chip8_init(Chip8 *chip8);
//load rom file into memory array
void chip8_load_rom(Chip8 *chip8, const char *filename);
//execute single cycle (fetch, decode, execute)
void chip8_execute_cycle(Chip8 *chip8);

#endif
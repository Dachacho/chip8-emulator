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

    // Clear everything one by one 
    memset(chip8->display, 0, DISPLAY_HEIGHT*DISPLAY_WIDTH);
    memset(chip8->keypad, 0, KEYPAD_SIZE);
    memset(chip8->stack, 0, STACK_SIZE);

    /*
    fontset starts from the memory address 0x50
    we copy fontset into the memory of our chip
    */
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

void chip8_execute_cycle(Chip8 *chip8){
    unsigned char X, Y, kk, n;
	unsigned short nnn;
    //load high bits from pc and low bits from pc+1
    unsigned short opcode = chip8->memory[chip8->pc] << 8 | chip8->memory[chip8->pc+1];
    //plus 2 every cycle (because we use +1 too)
    chip8->pc += 2;
    //extract out all the opcode operations to not have to repeat it every time
    X = (opcode & 0x0F00) >> 8;
	Y = (opcode & 0x00F0) >> 4;
	nnn = (opcode & 0x0FFF);
	kk = (opcode & 0x00FF);
	n = (opcode & 0x000F);

    //all comments for functions and what they do come from cowgods manual
    //literal copy paste from the docs

    //check what the first 4bits are
    switch (opcode & 0xF000)
    {
    case 0x0000:
        //if its 0 there could be 2 calls
        switch(opcode & 0x00FF){
            //00E0 - CLS (clear screen)
            case 0x00E0:
                memset(chip8->display, 0, DISPLAY_HEIGHT*DISPLAY_WIDTH);
                break;
            //00EE - RET (return from a subroutine)
            case 0x00EE:
                chip8->sp--;
                chip8->pc = chip8->stack[chip8->sp];
                break;
        }
        break;
    //1nnn - JP addr (jump to nnn - last 12 bits)
    case 0x1000:
        chip8->pc = nnn;
        break;
    //2nnn - CALL addr (call subroutine at nnn)
    case 0x2000:
        chip8->stack[chip8->sp] = chip8->pc;
        chip8->sp++;
        chip8->pc = nnn;
        break;
    //3xkk - SE Vx, byte (skip next instruction if Vx == kk)
    case 0x3000:
        if(chip8->V[X] == kk) chip8->pc += 2;
        break;
    //4xkk - SNE Vx, byte (skip next instruction if Vx != kk)
    case 0x4000:
        if(chip8->V[X] != kk) chip8->pc += 2;
        break;
    //5xy0 - SE Vx, Vy (skip next instruction if Vx == Vy)
    case 0x5000:
        if(chip8->V[X] == chip8->V[Y]) chip8->pc += 2;
        break;
    //6xkk - LD Vx, kk (set Vx = kk)
    case 0x6000:
        chip8->V[X] = kk;
        break;
    
    default:
        break;
    }
}

int main() {
    Chip8 chip8;

    chip8_init(&chip8);
    
}

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
    //load high bits from pc and low bits from pc+1
    unsigned short opcode = chip8->memory[chip8->pc] << 8 | chip8->memory[chip8->pc+1];
    //plus 2 every cycle (because we use +1 too)
    chip8->pc += 2;

    //all comments for functions and what they do come from cowgods manual

    //check what the first 4bits are
    switch (opcode & 0xF000)
    {
    case 0x0000:
        //if its 0 there could be 2 calls
        switch(opcode & 0x00FF){
            //CLS (clear screen)
            case 0x00E0:
                memset(chip8->display, 0, DISPLAY_HEIGHT*DISPLAY_WIDTH);
                break;
            //RET (return from a subroutine)
            case 0x00EE:
                chip8->sp--;
                chip8->pc = chip8->stack[chip8->sp];
                break;
        }
        break;
    //JP addr (jump to nnn - last 12 bits)
    case 0x1000:
        chip8->pc = opcode & 0x0FFF;
        break;
    //CALL addr (call subroutine at nnn)
    case 0x2000:
        chip8->stack[chip8->sp] = chip8->pc;
        chip8->sp++;
        chip8->pc = opcode & 0x0FFF;
        break;
    
    default:
        break;
    }
}

int main() {
    Chip8 chip8;

    chip8_init(&chip8);

    chip8.memory[0x200] = 0x21;
    chip8.memory[0x201] = 0x00;
    chip8.memory[0x100] = 0x00;
    chip8.memory[0x101] = 0xEE;

    chip8.pc = START_ADDR;

    // execute the CALL instruction
    chip8_execute_cycle(&chip8);

    // verify
    if (chip8.pc == 0x100 && chip8.sp == 1 && chip8.stack[0] == 0x202) {
        printf("Test Case 1 Passed: CALL pushed pc to stack and jumped to 0x100.\n");
    } else {
        printf("Test Case 1 Failed: pc = 0x%03X, sp = %d, stack[0] = 0x%03X (expected pc = 0x100, sp = 1, stack[0] = 0x202).\n",
               chip8.pc, chip8.sp, chip8.stack[0]);
    }

    // execute the RET instruction
    chip8_execute_cycle(&chip8);

    // verify
    if (chip8.pc == 0x202 && chip8.sp == 0) {
        printf("Test Case 2 Passed: RET restored pc to 0x202 and decremented sp.\n");
    } else {
        printf("Test Case 2 Failed: pc = 0x%03X, sp = %d (expected pc = 0x202, sp = 0).\n",
               chip8.pc, chip8.sp);
    }
}

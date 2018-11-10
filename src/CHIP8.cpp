//
// Created by sam on 09/11/18.
//

#include "CHIP8.h"

int CHIP8::getWidth() {
    return SCR_WIDTH;
}

int CHIP8::getHeight() {
    return SCR_HEIGHT;
}

void CHIP8::memory_init() {
    for (int i = 0; i < MEM_LENGTH; i++) {
        mem[i] = 0;
    }
    for (int i = 0; i < V_LENGTH; i++) {
        v[i] = 0;
    }
    for (int i = 0; i < STACK_LENGTH; i++) {
        stack[i] = 0;
    }
    for (int i = 0; i < SCR_WIDTH * SCR_HEIGHT; i++) {
        screen[i] = false;
    }

    delayTimer = 0;
    soundTimer = 0;
    pc = 0;
    sp = 0;
}

void CHIP8::process_instruction() {
    short int instruction = mem[pc];
    // Note: we increment the PC by 2 because the memory is an array of chars (1 byte),
    // and each instruction is 2 bytes

    // CLS - Clear Display
    if (instruction == 0x00E0) {
        for (int i = 0; i < SCR_WIDTH * SCR_HEIGHT; i++) {
            screen[i] = false;
        }
    }

    // RET - Return from current subroutine
    else if (instruction == 0x00EE) {
        pc = stack[sp];
        sp--;
    }

    // JMP - Jump to instruction in last three bits
    else if ((instruction & 0xF000) == 0x1000) {
        pc = instruction & 0x0FFF;
    }

    // CALL - Call subroutine in last three bits
    else if ((instruction & 0xF000) == 0x2000) {
        sp++;
        stack[sp] = pc;
        pc = instruction & 0x0FFF;
    }

    // SE - Skip next instruction if register in second bit is equal to third and fourth bits
    else if ((instruction & 0xF000) == 0x3000) {
        if (v[instruction & 0x0F00] == (instruction & 0x00FF)) {
            pc += 4;
        }
    }

    // SNE - Skip next instruction if register in second bit is not equal to third and fourth bits
    else if ((instruction & 0xF000) == 0x4000) {
        if (v[instruction & 0x0F00] != (instruction & 0x00FF)) {
            pc += 4;
        }
    }

    // SE - Skip next instruction if register in second bit is equal to register in third bit
    else if ((instruction & 0xF00F) == 0x5000) {
        if (v[instruction & 0x0F00] == v[instruction & 0x00F0]) {
            pc += 4;
        }
    }


}
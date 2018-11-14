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
    reg_i = 0;
}

void CHIP8::process_instruction() {
    short int inst = mem[pc];
    // Note: we increment the PC by 2 because the memory is an array of chars (1 byte),
    // and each instruction is 2 bytes

    // CLS - Clear Display
    if (inst == 0x00E0) {
        for (int i = 0; i < SCR_WIDTH * SCR_HEIGHT; i++) {
            screen[i] = false;
        }
    }

    // RET - Return from current subroutine
    else if (inst == 0x00EE) {
        pc = stack[sp];
        sp--;
    }

    // JMP - Jump to instruction in last three nibbles
    else if ((inst & 0xF000) == 0x1000) {
        pc = inst & 0x0FFF;
    }

    // CALL - Call subroutine in last three nibbles
    else if ((inst & 0xF000) == 0x2000) {
        sp++;
        stack[sp] = pc;
        pc = inst & 0x0FFF;
    }

    // SE - Skip next instruction if register in second nibble is equal to third and fourth nibbles
    else if ((inst & 0xF000) == 0x3000) {
        if (v[(inst & 0x0F00) >> 8] == (inst & 0x00FF)) {
            pc += 4;
        }
    }

    // SNE - Skip next instruction if register in second nibble is not equal to third and fourth nibbles
    else if ((inst & 0xF000) == 0x4000) {
        if (v[(inst & 0x0F00) >> 8] != (inst & 0x00FF)) {
            pc += 4;
        }
    }

    // SE - Skip next instruction if register in second nibble is equal to register in third nibble
    else if ((inst & 0xF00F) == 0x5000) {
        if (v[(inst & 0x0F00) >> 8] == v[inst & 0x00F0 >> 4]) {
            pc += 4;
        }
    }

    // LD - Load value in third and fourth nibbles into register in second nibble
    else if ((inst & 0xF000) == 0x6000) {
        v[(inst & 0x0F00) >> 8] = inst & 0x00FF;
        pc += 2;
    }

    // ADD - Add value in third and fourth nibbles to register in second nibble
    else if ((inst & 0xF000) == 0x7000) {
        v[(inst & 0x0F00) >> 8] += inst & 0x00FF;
        pc += 2;
    }

    // LD - Load value of register in third nibble into register in second nibble
    else if ((inst & 0xF00F) == 0x8000) {
        v[(inst & 0x0F00) >> 8] = v [inst & 0x00F0 >> 4];
        pc += 2;
    }

    // OR - Bitwise OR of register in second and register in third nibbles, store result in second-nibble register
    else if ((inst & 0xF00F) == 0x8001) {
        v[(inst & 0x0F00) >> 8] |= v[inst & 0x00F0 >> 4];
        pc += 2;
    }

    // AND - Bitwise AND of register in second and register in third nibbles, store result in second-nibble register
    else if ((inst & 0xF00F) == 0x8002) {
        v[(inst & 0x0F00) >> 8] &= v[inst & 0x00F0 >> 4];
        pc += 2;
    }

    // XOR - Bitwise XOR of register in second and register in third nibbles, store result in second-nibble register
    else if ((inst & 0xF00F) == 0x8003) {
        v[(inst & 0x0F00) >> 8] ^= v[inst & 0x00F0 >> 4];
        pc += 2;
    }

    // ADD - Addition of register in second and register in third nibbles, store result in second-nibble register
    // V[F] is used as the carry bit register
    else if ((inst & 0xF00F) == 0x8004) {
        int temp = v[inst & 0x0F00 >> 8] + v[inst & 0x00F0 >> 4];
        v[0xF] = (temp > 0xFF) ? 1 : 0;
        v[(inst & 0x0F00) >> 8] = temp & 0xFF;
        pc += 2;
    }

    // SUB - Subtraction of register in third from register in second nibbles, store result in second-nibble register
    // V[F] is used as the carry bit register
    else if ((inst & 0xF00F) == 0x8005) {
        int temp = v[inst & 0x0F00] - v[inst & 0x00F0];
        v[0xF] = (temp > 0) ? 1 : 0;
        v[(inst & 0x0F00) >> 8] = temp & 0xFF;
        pc += 2;
    }

    // SHR - Shift register in second nibble one bit to the right, store least significant bit in V[F]
    else if ((inst & 0xF00F) == 0x8005) {
        v[0xF] = v[inst & 0x0001];
        v[(inst & 0x0F00) >> 8] /= 2;
    }


    // SUBN - Subtraction of register in second from register in third nibbles, store result in second-nibble register
    // V[F] is used as the carry bit register
    else if ((inst & 0xF00F) == 0x8006) {
        int temp = v[inst & 0x00F0] - v[inst & 0x0F00];
        v[0xF] = (temp > 0) ? 1 : 0;
        v[(inst & 0x0F00) >> 8] = temp & 0xFF;
        pc += 2;
    }

    // SHL - Shift register in second nibble one bit to the left, store most significant bit in V[F]
    else if ((inst & 0xF00F) == 0x800E) {
        v[0xF] = v[(inst & 0x8000) >> 15];
        v[(inst & 0x0F00) >> 8] *= 2;
    }
}
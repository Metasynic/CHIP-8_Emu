//
// Created by sam on 09/11/18.
//

#include <cstdlib>
#include "CHIP8.h"

int CHIP8::getWidth() {
    return SCR_WIDTH;
}

int CHIP8::getHeight() {
    return SCR_HEIGHT;
}

void CHIP8::character_init() {
    mem[0] = (char)0xF0;
    mem[1] = (char)0x90;
    mem[2] = (char)0x90;
    mem[3] = (char)0x90;
    mem[4] = (char)0xF0;

    mem[5] = (char)0x20;
    mem[6] = (char)0x60;
    mem[7] = (char)0x20;
    mem[8] = (char)0x20;
    mem[9] = (char)0x70;

    mem[10] = (char)0xF0;
    mem[11] = (char)0x10;
    mem[12] = (char)0xF0;
    mem[13] = (char)0x80;
    mem[14] = (char)0xF0;

    mem[15] = (char)0xF0;
    mem[16] = (char)0x10;
    mem[17] = (char)0xF0;
    mem[18] = (char)0x10;
    mem[19] = (char)0xF0;

    mem[20] = (char)0x90;
    mem[21] = (char)0x90;
    mem[22] = (char)0xF0;
    mem[23] = (char)0x10;
    mem[24] = (char)0x10;

    mem[25] = (char)0xF0;
    mem[26] = (char)0x80;
    mem[27] = (char)0xF0;
    mem[28] = (char)0x10;
    mem[29] = (char)0xF0;

    mem[30] = (char)0xF0;
    mem[31] = (char)0x80;
    mem[32] = (char)0xF0;
    mem[33] = (char)0x90;
    mem[34] = (char)0xF0;

    mem[35] = (char)0xF0;
    mem[36] = (char)0x10;
    mem[37] = (char)0x20;
    mem[38] = (char)0x40;
    mem[39] = (char)0x40;

    mem[40] = (char)0xF0;
    mem[41] = (char)0x90;
    mem[42] = (char)0xF0;
    mem[43] = (char)0x90;
    mem[44] = (char)0xF0;

    mem[45] = (char)0xF0;
    mem[46] = (char)0x90;
    mem[47] = (char)0xF0;
    mem[48] = (char)0x10;
    mem[49] = (char)0xF0;

    mem[50] = (char)0xF0;
    mem[51] = (char)0x90;
    mem[52] = (char)0xF0;
    mem[53] = (char)0x90;
    mem[54] = (char)0x90;

    mem[55] = (char)0xE0;
    mem[56] = (char)0x90;
    mem[57] = (char)0xE0;
    mem[58] = (char)0x90;
    mem[59] = (char)0xE0;

    mem[60] = (char)0xF0;
    mem[61] = (char)0x80;
    mem[62] = (char)0x80;
    mem[63] = (char)0x80;
    mem[64] = (char)0xF0;

    mem[65] = (char)0xE0;
    mem[66] = (char)0x90;
    mem[67] = (char)0x90;
    mem[68] = (char)0x90;
    mem[69] = (char)0xE0;

    mem[70] = (char)0xF0;
    mem[71] = (char)0x80;
    mem[72] = (char)0xF0;
    mem[73] = (char)0x80;
    mem[74] = (char)0xF0;

    mem[75] = (char)0xF0;
    mem[76] = (char)0x80;
    mem[77] = (char)0xF0;
    mem[78] = (char)0x80;
    mem[79] = (char)0x80;
}

void CHIP8::memory_init() {
    character_init();

    for (int i = 80; i < MEM_LENGTH; i++) {
        mem[i] = 0;
    }
    for (int i = 0; i < V_LENGTH; i++) {
        v[i] = 0;
    }
    for (int i = 0; i < STACK_LENGTH; i++) {
        stack[i] = 0;
    }
    for (int i = 0; i < KEYS_LENGTH; i++) {
        keys[i] = false;
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

// The function wraps the pixel around the screen, and returns true if a pixel was erased via XOR
bool CHIP8::draw_xor(int x, int y, bool pixel) {
    x = x % SCR_WIDTH;
    y = y % SCR_HEIGHT;
    bool current = screen[y * SCR_WIDTH + x];
    screen[y * SCR_WIDTH + x] = pixel ^ current;
    return (current && !(pixel ^ current));
}

void CHIP8::process_instruction() {
    short int inst = mem[pc];
    // Note: we increment the PC by 2 because the memory is an array of chars (1 byte),
    // and each instruction is 2 bytes

    // CLS - Clear Display
    if (inst == 0x00E0) {
        for (int i = 0; i < SCR_WIDTH * SCR_HEIGHT; i++)
            screen[i] = false;

        pc += 2;
    }

    // RET - Return from current subroutine
    else if (inst == 0x00EE) {
        pc = stack[sp];
        sp--;
    }

    // JMP - Jump to instruction in last three nibbles
    else if ((inst & 0xF000) == 0x1000) {
        pc = (short int)(inst & 0x0FFF);
    }

    // CALL - Call subroutine in last three nibbles
    else if ((inst & 0xF000) == 0x2000) {
        sp++;
        stack[sp] = pc;
        pc = (short int)(inst & 0x0FFF);
    }

    // SE - Skip next instruction if register in second nibble is equal to third and fourth nibbles
    else if ((inst & 0xF000) == 0x3000) {
        if (v[(inst & 0x0F00) >> 8] == (inst & 0x00FF))
            pc += 4;
        else
            pc += 2;
    }

    // SNE - Skip next instruction if register in second nibble is not equal to third and fourth nibbles
    else if ((inst & 0xF000) == 0x4000) {
        if (v[(inst & 0x0F00) >> 8] != (inst & 0x00FF))
            pc += 4;
        else
            pc += 2;
    }

    // SE - Skip next instruction if register in second nibble is equal to register in third nibble
    else if ((inst & 0xF00F) == 0x5000) {
        if (v[(inst & 0x0F00) >> 8] == v[inst & 0x00F0 >> 4])
            pc += 4;
        else
            pc += 2;
    }

    // LD - Load value in third and fourth nibbles into register in second nibble
    else if ((inst & 0xF000) == 0x6000) {
        v[(inst & 0x0F00) >> 8] = (char)(inst & 0x00FF);
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
        v[0xF] = (temp > 0xFF) ? (char)1 : (char)0;
        v[(inst & 0x0F00) >> 8] = (char)(temp & 0xFF);
        pc += 2;
    }

    // SUB - Subtraction of register in third from register in second nibbles, store result in second-nibble register
    // V[F] is used as the carry bit register
    else if ((inst & 0xF00F) == 0x8005) {
        int temp = v[inst & 0x0F00] - v[inst & 0x00F0];
        v[0xF] = (temp > 0) ? (char)1 : (char)0;
        v[(inst & 0x0F00) >> 8] = (char)(temp & 0xFF);
        pc += 2;
    }

    // SHR - Shift register in second nibble one bit to the right, store least significant bit in V[F]
    else if ((inst & 0xF00F) == 0x8005) {
        v[0xF] = v[inst & 0x0001];
        v[(inst & 0x0F00) >> 8] /= 2;
        pc += 2;
    }


    // SUBN - Subtraction of register in second from register in third nibbles, store result in second-nibble register
    // V[F] is used as the carry bit register
    else if ((inst & 0xF00F) == 0x8006) {
        int temp = v[inst & 0x00F0] - v[inst & 0x0F00];
        v[0xF] = (temp > 0) ? (char)1 : (char)0;
        v[(inst & 0x0F00) >> 8] = (char)(temp & 0xFF);
        pc += 2;
    }

    // SHL - Shift register in second nibble one bit to the left, store most significant bit in V[F]
    else if ((inst & 0xF00F) == 0x800E) {
        v[0xF] = v[(inst & 0x8000) >> 15];
        v[(inst & 0x0F00) >> 8] *= 2;
        pc += 2;
    }

    // SNE - Skip next instruction if second nibble register is not equal to third nibble register
    else if ((inst & 0xF00F) == 0x9000) {
        if (v[(inst & 0x0F00) >> 8] != v[(inst & 0x00F0) >> 4])
            pc += 4;
        else
            pc += 2;
    }

    // LD I - Load the value represented by the last three nibbles into register I
    else if ((inst & 0xF000) == 0xA000) {
        reg_i = (short int)(inst & 0x0FFF);
        pc += 2;
    }

    // JMP V0 - Jump to location in last three nibbles plus the value of register V0
    else if ((inst & 0xF000) == 0xB000) {
        pc = (short int)((inst & 0x0FFF) + v[0x0]);
    }

    // RND - Generate a random byte, AND it with the last two nibbles of the instruction,
    // and store in the second-nibble register
    else if ((inst & 0xF000) == 0xC000) {
        char random = (char)(rand() % 0xFF);
        v[(inst & 0x0F00) >> 8] = char(random & (inst & 0x00FF));
        pc += 2;
    }

    // DRW - Display a sprite of length fourth-nibble bytes from I, at coordinates (second-nibble register,
    // third-nibble register) by XORing it with the existing screen. VF is set to 1 if this causes
    // any pixels to be erased, otherwise 0. Sprites wrap around the screen
    else if ((inst & 0xF000) == 0xD000) {
        // Load sprite data into a temporary array
        char n = (char)(inst & 0x000F);
        char spriteArray[n];
        for (int i = 0; i < n; i++) {
            spriteArray[i] = mem[reg_i + i];
        }
        v[0xF] = 0;
        int x = (inst & 0x0F00) >> 8;
        int base_y = (inst & 0x00F0) >> 4;

        // One iteration of this loop draws an 8-bit row of pixels to the screen
        for (int y = base_y; y < base_y + n; y++) {
            char spriteRow = spriteArray[y - base_y];

            if (draw_xor(x, y, ((spriteRow & 0x80) >> 7) == 1))
                v[0xF] = 1;
            if (draw_xor(x + 1, y, ((spriteRow & 0x40) >> 6) == 1))
                v[0xF] = 1;
            if (draw_xor(x + 2, y, ((spriteRow & 0x20) >> 5) == 1))
                v[0xF] = 1;
            if (draw_xor(x + 3, y, ((spriteRow & 0x10) >> 4) == 1))
                v[0xF] = 1;
            if (draw_xor(x + 4, y, ((spriteRow & 0x08) >> 3) == 1))
                v[0xF] = 1;
            if (draw_xor(x + 5, y, ((spriteRow & 0x04) >> 2) == 1))
                v[0xF] = 1;
            if (draw_xor(x + 6, y, ((spriteRow & 0x02) >> 1) == 1))
                v[0xF] = 1;
            if (draw_xor(x + 7, y, (spriteRow & 0x01) == 1))
                v[0xF] = 1;
        }

        pc += 2;
    }

    // SKP - Skip the next instruction if the second-nibble key is currently pressed
    else if ((inst & 0xF0FF) == 0xE09E) {
        if (keys[(inst & 0x0F00) >> 8])
            pc += 4;
        else
            pc += 2;
    }

    // SKNP - Skip the next instruction if the second-nibble key is not pressed
    else if ((inst & 0xF0FF) == 0xE0A1) {
        if (!keys[(inst & 0x0F00) >> 8])
            pc += 4;
        else
            pc += 2;
    }

    // LD DT - Set the second-nibble register to the value in the delay timer
    else if ((inst & 0xF0FF) == 0xF007) {
        v[(inst & 0x0F00) >> 8] = delayTimer;
        pc += 2;
    }

    // LD K - Wait for any key press then store the value of the key in second-nibble register
    else if ((inst & 0xF0FF) == 0xF00A) {
        for (char i = 0; i < KEYS_LENGTH; i++) {
            if (keys[i]) {
                v[(inst & 0x0F00) >> 8] = i;
                pc += 2;
                break;
            }
            // If there is no key down we don't increment the PC so it stays on this instruction
        }
    }

    // LD DT - Set the delay timer to the value in second-nibble register
    else if ((inst & 0xF0FF) == 0xF015) {
        delayTimer = v[(inst & 0x0F00) >> 8];
        pc += 2;
    }

    // LD ST - Set the sound timer to the value in second-nibble register
    else if ((inst & 0xF0FF) == 0xF018) {
        soundTimer = v[(inst & 0x0F00) >> 8];
        pc += 2;
    }

    // ADD I - Add the second-nibble register to I
    else if ((inst & 0xF0FF) == 0xF01E) {
        reg_i += v[(inst & 0x0F00) >> 8];
        pc += 2;
    }

    // LD F - Set I to the location of the sprite representing character in second-nibble register
    else if ((inst & 0xF0FF) == 0xF029) {
        reg_i = (short)(((inst & 0x0F00) >> 8) * 5);
        pc += 2;
    }

    // LD B - Store the BCD representation of second-nibble register in location I onwards
    else if ((inst & 0xF0FF) == 0xF033) {
        char number = (char)((inst & 0x0F00) >> 8);
        mem[reg_i] = (char)(number / 100);
        mem[reg_i + 1] = (char)((number % 100) / 10);
        mem[reg_i + 2] = (char)(number % 10);
        pc += 2;
    }

    // LD [I] - Store registers 0 to second nibble in memory starting at location I
    else if ((inst & 0xF0FF) == 0xF055) {
        for (int i = 0; i <= ((inst & 0x0F00) >> 8); i++) {
            mem[reg_i + i] = v[i];
        }
        pc += 2;
    }

    // LD [I] - Store memory from location I in registers 0 to second nibble
    else if ((inst & 0xF0FF) == 0xF065) {
        for (int i = 0; i <= ((inst & 0x0F00) >> 8); i++) {
            v[i] = mem[reg_i + i];
        }
        pc += 2;
    }
}
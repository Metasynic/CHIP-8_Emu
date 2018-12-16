//
// Created by sam on 09/11/18.
//

#include <cstdlib>
#include <iostream>
#include <fstream>
#include "CHIP8.h"

using namespace std;

int CHIP8::getWidth() {
    return SCR_WIDTH;
}

int CHIP8::getHeight() {
    return SCR_HEIGHT;
}


/* Initialize the interpreter-allocated memory with the data for each character. */
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

/* Initialize the different parts of memory. PC begins at 0x200. */
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
    pc = 0x200;
    sp = 0;
    reg_i = 0;
}

/* The function wraps the pixel around the screen, and returns true if a pixel was erased via XOR */
bool CHIP8::draw_xor(int x, int y, bool pixel) {
    x = x % SCR_WIDTH;
    y = y % SCR_HEIGHT;
    bool current = screen[y * SCR_WIDTH + x];
    screen[y * SCR_WIDTH + x] = pixel ^ current;
    return (current && !(pixel ^ current));
}

/* Load program from stream into memory. */
void CHIP8::load_program(ifstream &inbuffer) {
    short pointer = 0x200;
    char buffer = 0;
    while (inbuffer.read(&buffer, 1)) {
        mem[pointer] = buffer;
        pointer++;
    }

    cout << "Loaded " << pointer - 0x200 << " bytes of program." << endl;
    if (pointer > 0xFFF) {
        cerr << "WARNING: Program exceeds space allocated (past 0xFFF)." << endl;
    }
}

void CHIP8::process_instruction() {
    short int inst = (mem[pc] << 8) | mem[pc + 1];
    // Note: we increment the PC by 2 because the memory is an array of chars (1 byte),
    // and each instruction is 2 bytes

    cout << "Executing instruction " << inst << " from " << pc << "." << endl;

    // CLS - Clear Display
    if (inst == 0x00E0) {
        for (int i = 0; i < SCR_WIDTH * SCR_HEIGHT; i++)
            screen[i] = false;

        pc += 2;

        cout << "Clear Display" << endl;
    }

    // RET - Return from current subroutine
    else if (inst == 0x00EE) {
        pc = stack[sp];
        sp--;

        cout << "Return from subroutine" << endl;
    }

    // JMP - Jump to instruction in last three nibbles
    else if ((inst & 0xF000) == 0x1000) {
        pc = (short int)(inst & 0x0FFF);

        cout << "Jump to " << (inst & 0x0FFF) << endl;
    }

    // CALL - Call subroutine in last three nibbles
    else if ((inst & 0xF000) == 0x2000) {
        sp++;
        stack[sp] = pc;
        pc = (short int)(inst & 0x0FFF);

        cout << "Call subroutine at " << (inst & 0x0FFF) << endl;
    }

    // SE - Skip next instruction if register in second nibble is equal to third and fourth nibbles
    else if ((inst & 0xF000) == 0x3000) {
        if (v[(inst & 0x0F00) >> 8] == (inst & 0x00FF))
            pc += 4;
        else
            pc += 2;

        cout << "Skip if register " << ((inst & 0x0F00) >> 8) << " is " << (inst & 0x00FF) << endl;
    }

    // SNE - Skip next instruction if register in second nibble is not equal to third and fourth nibbles
    else if ((inst & 0xF000) == 0x4000) {
        if (v[(inst & 0x0F00) >> 8] != (inst & 0x00FF))
            pc += 4;
        else
            pc += 2;

        cout << "Skip if register " << ((inst & 0x0F00) >> 8) << " is not " << (inst & 0x00FF) << endl;
    }

    // SE - Skip next instruction if register in second nibble is equal to register in third nibble
    else if ((inst & 0xF00F) == 0x5000) {
        if (v[(inst & 0x0F00) >> 8] == v[(inst & 0x00F0) >> 4])
            pc += 4;
        else
            pc += 2;

        cout << "Skip if register " << ((inst & 0x0F00) >> 8) << " is register " << ((inst & 0x00F0) >> 4) << endl;
    }

    // LD - Load value in third and fourth nibbles into register in second nibble
    else if ((inst & 0xF000) == 0x6000) {
        v[(inst & 0x0F00) >> 8] = (char)(inst & 0x00FF);
        pc += 2;

        cout << "Load " << ((char)(inst & 0x00FF)) << " into register " << ((inst & 0x0F00) >> 8) << endl;
    }

    // ADD - Add value in third and fourth nibbles to register in second nibble
    else if ((inst & 0xF000) == 0x7000) {
        v[(inst & 0x0F00) >> 8] += inst & 0x00FF;
        pc += 2;

        cout << "Add " << ((char)(inst & 0x00FF)) << " into register " << ((inst & 0x0F00) >> 8) << endl;
    }

    // LD - Load value of register in third nibble into register in second nibble
    else if ((inst & 0xF00F) == 0x8000) {
        v[(inst & 0x0F00) >> 8] = v[(inst & 0x00F0) >> 4];
        pc += 2;

        cout << "Load value from register " << ((inst & 0x00F0) >> 4) << " into register " << ((inst & 0x0F00) >> 8) << endl;
    }

    // OR - Bitwise OR of register in second and register in third nibbles, store result in second-nibble register
    else if ((inst & 0xF00F) == 0x8001) {
        v[(inst & 0x0F00) >> 8] |= v[(inst & 0x00F0) >> 4];
        pc += 2;

        cout << "Bitwise OR registers" << ((inst & 0x0F00) >> 8) << " and " << ((inst & 0x00F0) >> 4) << ", store in first" << endl;
    }

    // AND - Bitwise AND of register in second and register in third nibbles, store result in second-nibble register
    else if ((inst & 0xF00F) == 0x8002) {
        v[(inst & 0x0F00) >> 8] &= v[(inst & 0x00F0) >> 4];
        pc += 2;

        cout << "Bitwise AND registers" << ((inst & 0x0F00) >> 8) << " and " << ((inst & 0x00F0) >> 4) << ", store in first" << endl;
    }

    // XOR - Bitwise XOR of register in second and register in third nibbles, store result in second-nibble register
    else if ((inst & 0xF00F) == 0x8003) {
        v[(inst & 0x0F00) >> 8] ^= v[(inst & 0x00F0) >> 4];
        pc += 2;

        cout << "Bitwise XOR registers" << ((inst & 0x0F00) >> 8) << " and " << ((inst & 0x00F0) >> 4) << ", store in first" << endl;
    }

    // ADD - Addition of register in second and register in third nibbles, store result in second-nibble register
    // V[F] is used as the carry bit register
    else if ((inst & 0xF00F) == 0x8004) {
        int temp = v[(inst & 0x0F00) >> 8] + v[(inst & 0x00F0) >> 4];
        v[0xF] = (temp > 0xFF) ? (char)1 : (char)0;
        v[(inst & 0x0F00) >> 8] = (char)(temp & 0xFF);
        pc += 2;

        cout << "Add registers" << ((inst & 0x0F00) >> 8) << " and " << ((inst & 0x00F0) >> 4) << ", store in first, V[F] carry" << endl;
    }

    // SUB - Subtraction of register in third from register in second nibbles, store result in second-nibble register
    // V[F] is used as the carry bit register
    else if ((inst & 0xF00F) == 0x8005) {
        int temp = v[(inst & 0x0F00) >> 8] - v[(inst & 0x00F0) >> 4];
        v[0xF] = (temp > 0) ? (char)1 : (char)0;
        v[(inst & 0x0F00) >> 8] = (char)(temp & 0xFF);
        pc += 2;

        cout << "Sub registers" << ((inst & 0x0F00) >> 8) << " and " << ((inst & 0x00F0) >> 4) << ", store in first, V[F] carry" << endl;
    }

    // SHR - Shift register in second nibble one bit to the right, store least significant bit in V[F]
    else if ((inst & 0xF00F) == 0x8006) {
        v[0xF] = v[inst & 0x0001];
        v[(inst & 0x0F00) >> 8] /= 2;
        pc += 2;

        cout << "Right shift by one register " << ((inst & 0x0F00) >> 8) << ", V[F] stores LSB" << endl;
    }


    // SUBN - Subtraction of register in second from register in third nibbles, store result in second-nibble register
    // V[F] is used as the carry bit register
    else if ((inst & 0xF00F) == 0x8007) {
        int temp = v[(inst & 0x00F0) >> 4] - v[(inst & 0x0F00) >> 8];
        v[0xF] = (temp > 0) ? (char)1 : (char)0;
        v[(inst & 0x0F00) >> 8] = (char)(temp & 0xFF);
        pc += 2;

        cout << "Sub registers" << ((inst & 0x00F0) >> 4) << " and " << ((inst & 0x0F00) >> 8) << ", store in last, V[F] carry" << endl;
    }

    // SHL - Shift register in second nibble one bit to the left, store most significant bit in V[F]
    else if ((inst & 0xF00F) == 0x800E) {
        v[0xF] = v[(inst & 0x8000) >> 15];
        v[(inst & 0x0F00) >> 8] *= 2;
        pc += 2;

        cout << "Left shift by one register " << ((inst & 0x0F00) >> 8) << ", V[F] stores MSB" << endl;
    }

    // SNE - Skip next instruction if second nibble register is not equal to third nibble register
    else if ((inst & 0xF00F) == 0x9000) {
        if (v[(inst & 0x0F00) >> 8] != v[(inst & 0x00F0) >> 4])
            pc += 4;
        else
            pc += 2;

        cout << "Skip if register " << ((inst & 0x0F00) >> 8) << " not equal to register " << ((inst & 0x00F0) >> 4) << endl;
    }

    // LD I - Load the value represented by the last three nibbles into register I
    else if ((inst & 0xF000) == 0xA000) {
        reg_i = (short int)(inst & 0x0FFF);
        pc += 2;

        cout << "Load I with " << (inst & 0x0FFF) << endl;
    }

    // JMP V0 - Jump to location in last three nibbles plus the value of register V0
    else if ((inst & 0xF000) == 0xB000) {
        pc = (short int)((inst & 0x0FFF) + v[0x0]);

        cout << "Jump to " << (inst & 0x0FFF) << " plus V[0]" << endl;
    }

    // RND - Generate a random byte, AND it with the last two nibbles of the instruction,
    // and store in the second-nibble register
    else if ((inst & 0xF000) == 0xC000) {
        char random = (char)(rand() % 0xFF);
        v[(inst & 0x0F00) >> 8] = (char)(random & (inst & 0x00FF));
        pc += 2;

        cout << "Generate random byte AND " << (inst & 0x00FF) << " in register " << ((inst & 0x0F00) >> 8) << endl;
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

        cout << "Draw" << endl;
    }

    // SKP - Skip the next instruction if the second-nibble key is currently pressed
    else if ((inst & 0xF0FF) == 0xE09E) {
        if (keys[(inst & 0x0F00) >> 8])
            pc += 4;
        else
            pc += 2;

        cout << "Skip if key " << ((inst & 0x0F00) >> 8) << "is down" << endl;
    }

    // SKNP - Skip the next instruction if the second-nibble key is not pressed
    else if ((inst & 0xF0FF) == 0xE0A1) {
        if (!keys[(inst & 0x0F00) >> 8])
            pc += 4;
        else
            pc += 2;

        cout << "Skip if key " << ((inst & 0x0F00) >> 8) << "is up" << endl;
    }

    // LD DT - Set the second-nibble register to the value in the delay timer
    else if ((inst & 0xF0FF) == 0xF007) {
        v[(inst & 0x0F00) >> 8] = delayTimer;
        pc += 2;

        cout << "Set register " << ((inst & 0x0F00) >> 8) << " to DT" << endl;
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

        cout << "Wait for key press and store in register " << ((inst & 0x0F00) >> 8) << endl;
    }

    // LD DT - Set the delay timer to the value in second-nibble register
    else if ((inst & 0xF0FF) == 0xF015) {
        delayTimer = v[(inst & 0x0F00) >> 8];
        pc += 2;

        cout << "Set DT to register " << ((inst & 0x0F00) >> 8) << endl;
    }

    // LD ST - Set the sound timer to the value in second-nibble register
    else if ((inst & 0xF0FF) == 0xF018) {
        soundTimer = v[(inst & 0x0F00) >> 8];
        pc += 2;

        cout << "Set ST to register " << ((inst & 0x0F00) >> 8) << endl;
    }

    // ADD I - Add the second-nibble register to I
    else if ((inst & 0xF0FF) == 0xF01E) {
        reg_i += v[(inst & 0x0F00) >> 8];
        pc += 2;

        cout << "Add register " << ((inst & 0x0F00) >> 8) << " to I" << endl;
    }

    // LD F - Set I to the location of the sprite representing character in second-nibble register
    else if ((inst & 0xF0FF) == 0xF029) {
        reg_i = (short)(((inst & 0x0F00) >> 8) * 5);
        pc += 2;

        cout << "Set I to location of character " << ((inst & 0x0F00) >> 8) << endl;
    }

    // LD B - Store the BCD representation of second-nibble register in location I onwards
    else if ((inst & 0xF0FF) == 0xF033) {
        char number = (char)((inst & 0x0F00) >> 8);
        mem[reg_i] = (char)(number / 100);
        mem[reg_i + 1] = (char)((number % 100) / 10);
        mem[reg_i + 2] = (char)(number % 10);
        pc += 2;

        cout << "Store BCD for register " << ((inst & 0x0F00) >> 8) << "at location in I" << endl;
    }

    // LD [I] - Store registers 0 to second nibble in memory starting at location I
    else if ((inst & 0xF0FF) == 0xF055) {
        for (int i = 0; i <= ((inst & 0x0F00) >> 8); i++) {
            mem[reg_i + i] = v[i];
        }
        pc += 2;

        cout << "Copy registers 0 to " << ((inst & 0x0F00) >> 8) << " to memory starting at value in I" << endl;
    }

    // LD [I] - Store memory from location I in registers 0 to second nibble
    else if ((inst & 0xF0FF) == 0xF065) {
        for (int i = 0; i <= ((inst & 0x0F00) >> 8); i++) {
            v[i] = mem[reg_i + i];
        }
        pc += 2;

        cout << "Copy registers 0 to " << ((inst & 0x0F00) >> 8) << " from memory starting at value in I" << endl;
    }

    else {
        cerr << "Instruction " << inst << " not recognized." << endl;
        exit(-1);
    }
}

/* The CHIP-8 decrements both its timers 60 times a second. */
void CHIP8::decrement_timers() {
    if (delayTimer > 0)
        delayTimer--;

    if (soundTimer > 0)
        soundTimer--;
}

/* Return whether a pixel on the screen is filled in or not. */
bool CHIP8::get_pixel(int x, int y) {
    return screen[y * SCR_WIDTH + x];
}
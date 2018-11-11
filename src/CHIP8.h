//
// Created by sam on 09/11/18.
//

#ifndef CHIP_8_EMU_CHIP8_H
#define CHIP_8_EMU_CHIP8_H

class CHIP8 {
public:
    int getWidth();
    int getHeight();
    void memory_init();
    void process_instruction();

private:
    static const int SCR_WIDTH = 64;
    static const int SCR_HEIGHT = 32;
    static const int MEM_LENGTH = 4096;
    static const int V_LENGTH = 16;
    static const int STACK_LENGTH = 16;
    char delayTimer;
    char soundTimer;
    short int pc;
    char sp;
    short int reg_i;
    char mem[MEM_LENGTH];
    char v[V_LENGTH];
    short int stack[STACK_LENGTH];
    bool screen[SCR_WIDTH * SCR_HEIGHT];
};

#endif //CHIP_8_EMU_CHIP8_H

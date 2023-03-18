#include "include/include.h"
#include "Vtop.h"
#include <verilated.h>
#include <verilated_vcd_c.h>

void step_and_dump_wave(VerilatedContext* contextp, VerilatedVcdC* tfp, Vtop* dut){
  dut->eval();
  contextp->timeInc(1);
  tfp->dump(contextp->time());
}

void sim(VerilatedContext* contextp, VerilatedVcdC* tfp, Vtop* dut){
    dut->x=0b11111111; dut->en=0; step_and_dump_wave(contextp, tfp, dut);
        Assert(dut->y == 0b000 && dut->vaild == 0b0 && dut->h ==0b1000000  , "error in test1");
    dut->x=0b00000000; dut->en=1; step_and_dump_wave(contextp, tfp, dut);
        Assert(dut->y == 0b000 && dut->vaild == 0b0 && dut->h ==0b1000000  , "error in test2");
    dut->x=0b00000001; dut->en=1; step_and_dump_wave(contextp, tfp, dut);
        Assert(dut->y == 0b000 && dut->vaild == 0b1 && dut->h ==0b1000000  , "error in test3");
    dut->x=0b00000011; dut->en=1; step_and_dump_wave(contextp, tfp, dut);
        Assert(dut->y == 0b001 && dut->vaild == 0b1 && dut->h ==0b1111001  , "error in test4");
    dut->x=0b00000111; dut->en=1; step_and_dump_wave(contextp, tfp, dut);
        Assert(dut->y == 0b010 && dut->vaild == 0b1 && dut->h ==0b0100100  , "error in test5");
    dut->x=0b00001111; dut->en=1; step_and_dump_wave(contextp, tfp, dut);
        Assert(dut->y == 0b011 && dut->vaild == 0b1 && dut->h ==0b0110000  , "error in test6");
    dut->x=0b00011111; dut->en=1; step_and_dump_wave(contextp, tfp, dut);
        Assert(dut->y == 0b100 && dut->vaild == 0b1 && dut->h ==0b0011001  , "error in test7");
    dut->x=0b00111111; dut->en=1; step_and_dump_wave(contextp, tfp, dut);
        Assert(dut->y == 0b101 && dut->vaild == 0b1 && dut->h ==0b0010010  , "error in test8");
    dut->x=0b01111111; dut->en=1; step_and_dump_wave(contextp, tfp, dut);
        Assert(dut->y == 0b110 && dut->vaild == 0b1 && dut->h ==0b0000010   , "error in test9");
    dut->x=0b11111111; dut->en=1; step_and_dump_wave(contextp, tfp, dut);
        Assert(dut->y == 0b111 && dut->vaild == 0b1 && dut->h ==0b1111000   , "error in test10");
}


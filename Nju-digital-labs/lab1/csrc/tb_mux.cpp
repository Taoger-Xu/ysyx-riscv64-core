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
  dut->s=0b00;  dut->a=0b10011100;  step_and_dump_wave(contextp, tfp, dut);
                Assert(dut->y == 0b00, "y is 00");
  dut->s=0b01;  dut->a=0b10011100;  step_and_dump_wave(contextp, tfp, dut);
                Assert(dut->y == 0b11, "y is 11");
  dut->s=0b10;  dut->a=0b10011100;  step_and_dump_wave(contextp, tfp, dut);
                Assert(dut->y == 0b01, "y is 01");
  dut->s=0b11;  dut->a=0b10011100;  step_and_dump_wave(contextp, tfp, dut);
                Assert(dut->y == 0b10, "y is 10");
}


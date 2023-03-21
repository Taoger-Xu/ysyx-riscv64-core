#include "include/include.h"
#include "Vtop.h"
#include <verilated.h>
#include <verilated_vcd_c.h>
#ifdef DUMPWAVE_ON
void step_and_dump_wave(VerilatedContext* contextp, VerilatedVcdC* tfp, Vtop* dut){
  dut->eval();
  contextp->timeInc(1);
  tfp->dump(contextp->time());
}

void sim(VerilatedContext* contextp, VerilatedVcdC* tfp, Vtop* dut){
//算术左移移动1位
//   dut->din=0b11011011;  dut->shamt=0b001; dut->LR=0b1; dut->AL=0b1; step_and_dump_wave(contextp, tfp, dut);
//           Assert(dut->dout == 0b10110110, "test1");
//         //   printf("dut-dout is %b\n", dut->dout);

 //算术左移移动7位
  dut->din=0b11011011;  dut->shamt=0b111; dut->LR=0b0; dut->AL=0b1; step_and_dump_wave(contextp, tfp, dut);
        Assert(dut->dout == 0b11111111, "test2");
}

#endif
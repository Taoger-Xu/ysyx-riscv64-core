#ifndef _INCLUDE_H_
#define _INCLUDE_H_

#include "Vtop.h"
#include <verilated.h>
#include <verilated_vcd_c.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define Assert(cond, format, ...) \
  do { \
    if (!(cond)) { \
      fprintf(stderr, format "\n", ## __VA_ARGS__); \
      assert(cond); \
    } \
  } while (0)

/*开启仿真*/
#define DUMPWAVE_ON  1 
/*接入NVBOARD*/
// #define NVBOARD_ON  1
#ifdef DUMPWAVE_ON 1
void step_and_dump_wave(VerilatedContext* contextp, VerilatedVcdC* tfp, Vtop* dut);
void sim(VerilatedContext* contextp, VerilatedVcdC* tfp, Vtop* dut);
#endif

#endif
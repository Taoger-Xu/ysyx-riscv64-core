#include <nvboard.h>
#include "Vtop.h"
#include "include/include.h"
#include "verilated.h"
#include "verilated_vcd_c.h"
static TOP_NAME top;

#ifdef NVBOARD_ON
void nvboard_bind_all_pins(Vtop *top);
#endif

#ifdef DUMPWAVE_ON
    void step_and_dump_wave(VerilatedContext* contextp, VerilatedVcdC* tfp, Vtop* dut);
    void sim(VerilatedContext* contextp, VerilatedVcdC* tfp, Vtop* dut);
#endif


int main() {
    VerilatedContext*  contextp = new VerilatedContext;
    Vtop* dut = new Vtop;     
    /*开启仿真*/
    #ifdef DUMPWAVE_ON
    VerilatedVcdC* tfp = new VerilatedVcdC;
    contextp->traceEverOn(true);
    dut->trace(tfp, 0);
    tfp->open("waveform.vcd");

    sim(contextp, tfp, dut);

    step_and_dump_wave(contextp, tfp, dut);
    tfp->close();
    #endif

    /*接入nvboard*/
    #ifdef NVBOARD_ON
    nvboard_bind_all_pins(dut);
    nvboard_init();

    while (1) {
        nvboard_update();
        dut->eval();
    }
    nvboard_quit();
    #endif
    return 0;
}


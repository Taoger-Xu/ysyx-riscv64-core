#include "include/include.h"
#include "include/tb_alu.h"
#include "Vtop.h"
#include <verilated.h>
#include <verilated_vcd_c.h>

#ifdef DUMPWAVE_ON 
// 定义补码加法函数
ComplementCode* Add(ComplementCode *code1, ComplementCode *code2){
    ComplementCode *output = (ComplementCode *)malloc(sizeof (ComplementCode)); // 输出
    int carry = 0; //初始化进位标志位为0
    for(int i = 0; i <=3; i++){
        int sum = code1->bits[i] + code2->bits[i] + carry;
        output->bits[i] = sum % 2; // 计算该位上的和
        carry = sum / 2; // 更新进位标志位
    }
    return output;
}

// 将有符号数转换为补码
ComplementCode* SignedToComplement(int num){
    ComplementCode *complment = NULL;
    if(num >= 0){ // 非负数直接转化为二进制
        complment = (ComplementCode *)malloc(sizeof (ComplementCode)); // 输出
        complment->bits = std::bitset<4>(num);
    } else{ // 对于负数，先计算它的绝对值的二进制补码，然后取反加1
        int abs_num = -num;
        ComplementCode *abs_complment = SignedToComplement(abs_num);
        abs_complment->bits = ~abs_complment->bits;
        ComplementCode *one = (ComplementCode *)malloc(sizeof (ComplementCode));
        one->bits = std::bitset<4>(1);
        complment = Add(abs_complment, one);
        free(abs_complment);
        free(one);
    }
    return complment;
}

// 将4位二进制补码转换为无符号数用于判断相等
int ComplementToSigned(ComplementCode *code){
        return code->bits[3] * (8) + code->bits[2] * 4 + code->bits[1] *2 + code->bits[0];
}
// 加法运算
Result* ALU_ADD(int a, int b){
    ComplementCode *code1 = SignedToComplement(a);
    ComplementCode *code2 = SignedToComplement(b);
    Result *result = (Result *)malloc(sizeof (Result)); // 输出
    ComplementCode *output = (ComplementCode *)malloc(sizeof (ComplementCode)); // 输出
    int carry = 0; //初始化进位标志位为0
    for(int i = 0; i <=3; i++){
        int sum = code1->bits[i] + code2->bits[i] + carry;
        output->bits[i] = sum % 2; // 计算该位上的和
        carry = sum / 2; // 更新进位标志位
    }
    result->output = ComplementToSigned(output);
    result->CF = carry;

    if(result->output) result->ZF = 1;
    else result->ZF = 0;

    if((code1->bits[3] == code2->bits[3]) && (output->bits[3] != code1->bits[3])) result->OF = 1;
    else result->OF = 0;
    free(code1);
    free(code2);
    free(output);
    return result;
}

// 减法运算
Result* ALU_SUB(int a, int b){
    ComplementCode *code1 = SignedToComplement(a);
    int abs_num = -b;
    // 求b的相反数的补码
    ComplementCode *abs_complment = SignedToComplement(abs_num);
    abs_complment->bits = ~abs_complment->bits;
    ComplementCode *one = (ComplementCode *)malloc(sizeof (ComplementCode));
    one->bits = std::bitset<4>(1);
    ComplementCode *code2 = Add(abs_complment, one); 

    Result *result = (Result *)malloc(sizeof (Result)); // 输出
    ComplementCode *output = (ComplementCode *)malloc(sizeof (ComplementCode)); // 输出
    int carry = 0; //初始化进位标志位为0
    for(int i = 0; i <=3; i++){
        int sum = code1->bits[i] + code2->bits[i] + carry;
        output->bits[i] = sum % 2; // 计算该位上的和
        carry = sum / 2; // 更新进位标志位
    }
    result->output = ComplementToSigned(output);
    result->CF = carry;

    if(result->output) result->ZF = 1;
    else result->ZF = 0;

    if((code1->bits[3] == code2->bits[3]) && (output->bits[3] != code1->bits[3])) result->OF = 1;
    else result->OF = 0;
    free(code1);
    free(code2);
    free(abs_complment);
    free(one);
    free(output);
    return result;
}

// 取反运算
Result* ALU_NOT(int a, int b){
    ComplementCode *code1 = SignedToComplement(a);
    code1->bits = ~code1->bits;
    Result *result = (Result *)malloc(sizeof (Result)); // 输出
    result->output = ComplementToSigned(code1);
    result->ZF = 0;
    result->OF = 0;
    result->CF = 0;
    free(code1);
    return result;
}

// 与运算
Result* ALU_AND(int a, int b){
    ComplementCode *code1 = SignedToComplement(a);
    ComplementCode *code2 = SignedToComplement(b);
    Result *result = (Result *)malloc(sizeof (Result)); // 输出
    ComplementCode *output = (ComplementCode *)malloc(sizeof (ComplementCode)); // 输出
    for(int i = 0; i <=3; i++){
        output->bits[i] = code1->bits[i] & code2->bits[i] + carry;
    }
    result->output = ComplementToSigned(output);
    result->ZF = 0;
    result->OF = 0;
    result->CF = 0;
    free(code1);
    free(code2);
    free(output);
    return result;
}

// 或运算
Result* ALU_OR(int a, int b){
    ComplementCode *code1 = SignedToComplement(a);
    ComplementCode *code2 = SignedToComplement(b);
    Result *result = (Result *)malloc(sizeof (Result)); // 输出
    ComplementCode *output = (ComplementCode *)malloc(sizeof (ComplementCode)); // 输出
    for(int i = 0; i <=3; i++){
        output->bits[i] = code1->bits[i] | code2->bits[i] + carry;
    }
    result->output = ComplementToSigned(output);
    result->ZF = 0;
    result->OF = 0;
    result->CF = 0;
    free(code1);
    free(code2);
    free(output);
    return result;
}

// 异或运算
Result* ALU_XOR(int a, int b){
    ComplementCode *code1 = SignedToComplement(a);
    ComplementCode *code2 = SignedToComplement(b);
    Result *result = (Result *)malloc(sizeof (Result)); // 输出
    ComplementCode *output = (ComplementCode *)malloc(sizeof (ComplementCode)); // 输出
    for(int i = 0; i <=3; i++){
        output->bits[i] = code1->bits[i] ^ code2->bits[i] + carry;
    }
    result->output = ComplementToSigned(output);
    result->ZF = 0;
    result->OF = 0;
    result->CF = 0;
    free(code1);
    free(code2);
    free(output);
    return result;
}

// 比较大小
Result* ALU_COMPARE(int a, int b){

}



void step_and_dump_wave(VerilatedContext* contextp, VerilatedVcdC* tfp, Vtop* dut){
  dut->eval();
  contextp->timeInc(1);
  tfp->dump(contextp->time());
}

void sim(VerilatedContext* contextp, VerilatedVcdC* tfp, Vtop* dut){
    /*加法测试*/
    dut->in_a=1;dut->in_b=1;dut->op=0b000;step_and_dump_wave(contextp, tfp, dut);
        Result *result = AlU_ADD(dut->in_a,dut->in_b);
        Assert(dut->out_c == result->output && dut->ZF == result->ZF && dut->OF == result->OF && dut->CF == result->CF, "test 1 + 1");
        free(result);
    dut->in_a=4;dut->in_b=5;dut->op=0b000;step_and_dump_wave(contextp, tfp, dut);
        Result *result = AlU_ADD(dut->in_a,dut->in_b);
        Assert(dut->out_c == result->output && dut->ZF == result->ZF && dut->OF == result->OF && dut->CF == result->CF, "test 4 + 5");
        free(result);
    dut->in_a=1;dut->in_b=-1;dut->op=0b000;step_and_dump_wave(contextp, tfp, dut);
        Result *result = AlU_ADD(dut->in_a,dut->in_b);
        Assert(dut->out_c == result->output && dut->ZF == result->ZF && dut->OF == result->OF && dut->CF == result->CF, "test 1 + (-1)");
        free(result);
    
    /*减法测试*/
    dut->in_a=4;dut->in_b=5;dut->op=0b001;step_and_dump_wave(contextp, tfp, dut);
        Result *result = ALU_SUB(dut->in_a,dut->in_b);
        Assert(dut->out_c == result->output && dut->ZF == result->ZF && dut->OF == result->OF && dut->CF == result->CF, "test 4 - 5");
        free(result);

    dut->in_a=7;dut->in_b=6;dut->op=0b001;step_and_dump_wave(contextp, tfp, dut);
        Result *result = ALU_SUB(dut->in_a,dut->in_b);
        Assert(dut->out_c == result->output && dut->ZF == result->ZF && dut->OF == result->OF && dut->CF == result->CF, "test 7 - 6");
        free(result);

    dut->in_a=6;dut->in_b=-4;dut->op=0b001;step_and_dump_wave(contextp, tfp, dut);
        Result *result = ALU_SUB(dut->in_a,dut->in_b);
        Assert(dut->out_c == result->output && dut->ZF == result->ZF && dut->OF == result->OF && dut->CF == result->CF, "test 6 - (-4)");
        free(result);
    
    /*取反测试*/
    dut->in_a=6;dut->in_b=-4;dut->op=0b001;step_and_dump_wave(contextp, tfp, dut);
        Result *result = ALU_SUB(dut->in_a,dut->in_b);
        Assert(dut->out_c == result->output && dut->ZF == result->ZF && dut->OF == result->OF && dut->CF == result->CF, "test 6 - (-4)");
        free(result);
}

#endif
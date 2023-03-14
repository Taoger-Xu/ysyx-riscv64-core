#include<bitset>

#ifndef _TBALU_H_
#define _TBALU_H_

// 定义4位补码结构体
struct ComplementCode
{
    std::bitset<4> bits;
};

// 定义定义计算器结构体
struct Result {
    int output;  // 输出结果
    int CF;  // 进位标志位
    int OF; //溢出标志位
    int ZF; // 结果为零标志位
};

// 定义补码加法函数
ComplementCode* Add(ComplementCode *code1, ComplementCode *code2);

// 将有符号数转换为补码
ComplementCode* SignedToComplement(int num);

// 将4位二进制补码转换为有符号数
int ComplementToSigned(ComplementCode *code);

// 加法运算
Result* ALU_ADD(int a, int b);

// 减法运算
Result* ALU_SUB(int a, int b);

// 取反运算
Result* ALU_NOT(int a, int b);

// 与运算
Result* ALU_AND(int a, int b);

// 或运算
Result* ALU_OR(int a, int b);

// 异或运算
Result* ALU_XOR(int a, int b);

// 比较大小
Result* ALU_COMPARE(int a, int b);

//判断相等
Result* ALU_EQUAL(int a, int b);
#endif
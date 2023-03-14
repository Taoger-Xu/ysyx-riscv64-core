module top (in_a,in_b,op,out_c,ZF,OF,CF);
    input [3:0] in_a; // 操作数a
    input [3:0] in_b; // 操作数b
    input [2:0] op; // 操作码
    output reg [3:0] out_c; // 运算结果
    output reg ZF; // 结果是否为0
    output reg OF; // 是否溢出
    output reg CF; // 是否有进位
    alu inst_alu(
      .in_a(in_a),
      .in_b(in_b),
      .op(op),
      .out_c(out_c),
      .ZF(ZF),
      .OF(OF),
      .CF(CF)
    );
endmodule


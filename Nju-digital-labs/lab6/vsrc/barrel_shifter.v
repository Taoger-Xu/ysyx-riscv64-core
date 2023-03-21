module barrel_shifter (
    input [7:0] din, // 输入
    input [2:0] shamt, // 移位位数
    input LR, // 左移右移选择,为1为左移
    input AL, // 算术逻辑选择,为1为算术移位
    output [7:0] dout // 输出
);
    wire [7:0] x, y;
    wire [1:0] sel_1, sel_2, sel_3;
    wire sign;

    assign sign = AL ? din[7] : 0;

    //第一级
    assign sel_1 = {LR, shamt[0]};
    mux41 inst_1_0(din[0], din[1], din[0], 1'b0,   sel_1, x[0]);
    mux41 inst_1_1(din[1], din[2], din[1], din[0], sel_1, x[1]);
    mux41 inst_1_2(din[2], din[3], din[2], din[1], sel_1, x[2]);
    mux41 inst_1_3(din[3], din[4], din[3], din[2], sel_1, x[3]);
    mux41 inst_1_4(din[4], din[5], din[4], din[3], sel_1, x[4]);
    mux41 inst_1_5(din[5], din[6], din[5], din[4], sel_1, x[5]);
    mux41 inst_1_6(din[6], din[7], din[6], din[5], sel_1, x[6]);
    mux41 inst_1_7(din[7], sign,   din[7], din[6], sel_1, x[7]);
    
    //第二级
    assign sel_2 = {LR, shamt[1]};
    mux41 inst_2_0(x[0], x[2], x[0], 1'b0, sel_2, y[0]);
    mux41 inst_2_1(x[1], x[3], x[1], 1'b0, sel_2, y[1]);
    mux41 inst_2_2(x[2], x[4], x[2], x[0], sel_2, y[2]);
    mux41 inst_2_3(x[3], x[5], x[3], x[1], sel_2, y[3]);
    mux41 inst_2_4(x[4], x[6], x[4], x[2], sel_2, y[4]);
    mux41 inst_2_5(x[5], x[7], x[5], x[3], sel_2, y[5]);
    mux41 inst_2_6(x[6], sign, x[6], x[4], sel_2, y[6]);
    mux41 inst_2_7(x[7], sign, x[7], x[5], sel_2, y[7]);

    //第三级
    assign sel_3 = {LR, shamt[2]};
    mux41 inst_3_0(y[0], y[4], y[0], 1'b0, sel_3, dout[0]);
    mux41 inst_3_1(y[1], y[5], y[1], 1'b0, sel_3, dout[1]);
    mux41 inst_3_2(y[2], y[6], y[2], 1'b0, sel_3, dout[2]);
    mux41 inst_3_3(y[3], y[7], y[3], 1'b0, sel_3, dout[3]);
    mux41 inst_3_4(y[4], sign, y[4], y[0], sel_3, dout[4]);
    mux41 inst_3_5(y[5], sign, y[5], y[1], sel_3, dout[5]);
    mux41 inst_3_6(y[6], sign, y[6], y[2], sel_3, dout[6]);
    mux41 inst_3_7(y[7], sign, y[7], y[3], sel_3, dout[7]);
    
endmodule
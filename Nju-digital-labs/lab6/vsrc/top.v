module top (
    input [7:0] din, // 输入
    input [2:0] shamt, // 移位位数
    input LR, // 左移右移选择,为1为左移
    input AL, // 算术逻辑选择,为1为算术移位
    output [7:0] dout // 输出
);
   barrel_shifter demo (din, shamt, LR, AL, dout);
endmodule
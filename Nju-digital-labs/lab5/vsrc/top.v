module top (
    input clk, // 时钟信号
    input we,  // 写使能信号
    input [7:0] din, //写入的数据
    input [3:0] inaddr, //写数据需要的地址
    input [3:0] outaddr,//读数据需要的地址
    output [7:0] dout //读出的数据
);
    
    /*实列化*/
    ram #(8, 4) inst_ram(
      .clk(clk),
      .we(we),
      .din(din),
      .inaddr(inaddr),
      .outaddr(outaddr),
      .dout(dout)
    );

endmodule


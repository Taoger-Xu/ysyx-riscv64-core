module ram #(
    parameter RAM_WIDTH = 8,
    parameter RAM_ADDR_WIDTH = 4
)(
    input clk, // 时钟信号
    input we,  // 写使能信号
    input [RAM_WIDTH-1:0] din, //写入的数据
    input [RAM_ADDR_WIDTH-1:0] inaddr, //写数据需要的地址
    input [RAM_ADDR_WIDTH-1:0] outaddr,//读数据需要的地址
    output [RAM_WIDTH-1:0] dout //读出的数据
);
    reg [RAM_WIDTH-1:0] ram [(2**RAM_ADDR_WIDTH)-1:0];
    initial begin
       $readmemh("mem.txt", ram);
    end
    always @(posedge clk) begin
        if(we) begin
            ram[outaddr] <= din;
        end
    end

    assign dout = ram[outaddr];
    
endmodule
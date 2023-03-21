module shift_reg (
    input clk,
    input [2:0] ctrl, // 控制位
    input [7:0] data_in,
    output reg [7:0] q
);
    always @(posedge clk) begin
        case (ctrl)
            3'b000 : q <= 8'b0; // 清0
            3'b001 : q <= data_in; //置数
            3'b010 : q <= {1'b0, q[7:1]}; //逻辑右移
            3'b011 : q <= {q[6:0], 1'b0}; //逻辑左移
            3'b100 : q <= {q[7], q[7:1]}; //算术右移
            3'b101 : q <= {data_in[0], q[7:1]}; //左端串行输入1位值，并行输出8位值
            3'b110 : q <= {q[0], q[7:1]}; //循环右移
            3'b111 : q <= {q[6:0], q[7]}; //循环左移
            default:  
                     q <= q;
        endcase
        
    end
endmodule
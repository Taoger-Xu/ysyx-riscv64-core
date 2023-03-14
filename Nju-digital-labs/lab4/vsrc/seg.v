/*seg.v: 七段码模块*/

module seg(
  input  wire [3:0] in_num, // 输入的需要显示的数
  output reg [6:0] out_seg //输出的显示结果
);

  /*需要显示0-9*/
  always @(*) begin
    case (in_num)
      // 注意约束文件中链接顺序要与编码匹配
      4'b0000: out_seg = 7'b1000000; // 显示0
      4'b0001: out_seg = 7'b1111001; // 显示1
      4'b0010: out_seg = 7'b0100100; // 显示2
      4'b0011: out_seg = 7'b0110000; // 显示3
      4'b0100: out_seg = 7'b0011001; // 显示4
      4'b0101: out_seg = 7'b0010010; // 显示5
      4'b0110: out_seg = 7'b0000010; // 显示6
      4'b0111: out_seg = 7'b1111000; // 显示7
      4'b1000: out_seg = 7'b0000000; // 显示8
      4'b1001: out_seg = 7'b0010000; // 显示9
      4'b1010: out_seg = 7'b0001000; // 显示10
      4'b1011: out_seg = 7'b0000011; // 显示11
      4'b1100: out_seg = 7'b1000110; // 显示12
      4'b1101: out_seg = 7'b0100001; // 显示13
      4'b1110: out_seg = 7'b0000110; // 显示14
      4'b1111: out_seg = 7'b0001110; // 显示15
      default: 
              out_seg = 7'b0000000;
    endcase    
  end
endmodule
module top (x,en,y,vaild,h);
    input  [7:0] x; // 输入的8位二进制数
    input        en;// 输入的使能端
    output reg [2:0] y; // 二进制的优先编码
    output reg vaild; //输入是否有效
    output reg [6:0] h; //七段数码管的控制位
    integer i;
    wire [3:0] in_num;
    always @(*) begin
      if(en)
        begin
          y = 0; vaild = 0;
          for(i = 0; i < 8; i = i+1)
            begin
              if(x[i] == 1) begin
                y = i[2:0];
                vaild = 1'b1;
              end
            end
        end
      else begin
        y = 0; vaild = 0;
      end
    end

    /*l是编码的总结果,包含符号位和数据位*/
    assign in_num = {1'b0, y};

    /*显示结果*/
    seg inst_seg(
      .in_num(in_num),
      .out_seg(h)
    );
endmodule


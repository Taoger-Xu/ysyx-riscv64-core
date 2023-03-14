module top (clk, en, clr_p, LEDR, HEX0, HEX1);
    input clk; 
    input en; // 当en为1时正常工作，否则暂停计数,同步
    input clr_p; //异步清零信号,高电平有效
    output		     [1:0]		LEDR; //相关指示信号
    output		     [6:0]		HEX0; //七段管0
	  output		     [6:0]		HEX1; //七段管1

    wire [3:0] num1, num0;
    
    /*实列化分频器*/
    clock_1 inst_clock_1(
      .clk_50M(clk),
      .clk_1s(LEDR[0])
    );

    counter_mod100 inst_counter_mod100(
      .clk(LEDR[0]),
      .en(en),
      .clr_p(clr_p),
      .num1(num1),
      .num0(num0),
      .finshed(LEDR[1])
    );

    seg inst_seg0(
      .in_num(num0),
      .out_seg(HEX0)
    );

    seg inst_seg1(
      .in_num(num1),
      .out_seg(HEX1)
    );

endmodule


/*counter_mod100.v : 模100计数器*/
module counter_mod100 (clk, en, clr_p,num1,num0,finshed);
    input clk; // 与分频器的时钟信号相连
    input en; // 当en为1时正常工作，否则暂停计数,同步
    input clr_p; //异步清零信号,高电平有效
    output reg[3:0] num1;//计数的十位
    output reg[3:0] num0;//技术的个位
    /* verilator lint_off UNOPTFLAT */
    output reg finshed; // 完成一个技术周期的提示信号
    /* verilator lint_off UNOPTFLAT */
    integer cnt; // 当前计数值
    initial begin
        cnt = 0;
    end
    always @(posedge clk or negedge clr_p) begin
        if(clr_p == 1)  begin
            cnt <= 0;
            finshed <= 0;
        end// 异步清零
        else begin
            if(en == 1) // en信号有效，计数
                begin
                    if(cnt == 99) begin //归零
                        cnt <= 0;
                        finshed <= 1;
                    end
                    else begin
                        cnt <= cnt + 1;
                        finshed <= 0;
                    end
                end
            else // 暂停计数
                cnt <= cnt;
        end
        /* verilator lint_off WIDTH */
        num1 <= cnt / 10;
        num0 <= cnt % 10;
        /* verilator lint_off WIDTH */
    end
endmodule
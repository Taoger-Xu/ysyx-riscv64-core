/*clock_1.v 分频器,输入为50MHz的时钟，输出为一个频率为1Hz，周期为1秒的时钟信号*/
module clock_1 (clk_50M, clk_1s);
    input clk_50M;
    output reg clk_1s;
    integer cnt; // 计数器
    always @(posedge clk_50M) begin
        if(cnt == 24999999) begin
            cnt <= 0;
            clk_1s <= ~clk_1s;
        end
        else
            cnt <= cnt + 1;
    end
endmodule
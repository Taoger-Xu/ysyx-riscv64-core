module lsfr (clk, rs_n, load, data_in, q);
    input clk;
    input rs_n;
    input load;
    input [7:0] data_in;
    output reg [7:0] q;
    always @(posedge clk) begin
        if(!rs_n) begin// 清零
            q <= 8'b0;
        end
        else begin
            if(load)
                q <= data_in;
            else begin
                // 注意全零状态
                q <= {(q[4] ^ q[3] ^ q[2] ^ q[0]) ^ (~(|q)), q[7:1]};
            end
        end
    end
endmodule
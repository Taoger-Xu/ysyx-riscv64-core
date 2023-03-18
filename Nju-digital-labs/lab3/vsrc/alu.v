/*alu.v*/
module alu (in_a,in_b,op,out_c,ZF,CF,OF);
    input [3:0] in_a; // 操作数a
    input [3:0] in_b; // 操作数b
    input [2:0] op; // 操作码
    output reg [3:0] out_c; // 运算结果
    output reg ZF; // 结果是否为0
    output reg OF; // 是否溢出
    output reg CF; // 是否有进位
    reg [3:0] in_b_com; // b的补码
    integer a;
    integer b;
    always @(*) begin
        ZF = 1'b0;
        OF = 1'b0;
        CF = 1'b0;
        case (op)
            /*执行加法运算*/
            3'b000: begin
                /*得到进位信号和结果*/
                {CF, out_c} = in_a + in_b;
                /*溢出判断*/
                OF = (in_a[3] == in_b[3]) && (out_c[3] != in_a[3]);
                /*结果是否为0*/
                ZF = ~(|out_c);
            end

            /*执行减法运算*/
            3'b001: begin   
                /*得到b的补码*/
                in_b_com = ~in_b + 1;
                /*得到进位信号和结果*/
                {CF, out_c} = in_a + in_b_com;
                /*溢出判断*/
                OF = (in_a[3] == in_b_com[3]) && (out_c[3] != in_a[3]);
                /*结果是否为0*/
                ZF = ~(|out_c);
            end
            /*逻辑运算*/
            3'b010: out_c = ~in_a;
            3'b011: out_c = in_a & in_b;
            3'b110: out_c = in_a | in_b;
            3'b101: out_c = in_a ^ in_b;
            /* 比较大小，注意in_a和in_b都是有符号的补码，reg 型变量为无符号数，而 integer 型变量为有符号数 */
            3'b110: begin
                /* verilator lint_off WIDTH */
                a = -in_a[3]*8 + in_a[2]*4 + in_a[1]*2 + in_a[0]*1;
                b = -in_b[3]*8 + in_b[2]*4 + in_b[1]*2 + in_b[0]*1;
                out_c = (a < b) ? 4'b1 : 4'b0;
                /* verilator lint_off WIDTH */
            end
            3'b111: begin
                out_c = (in_a == in_b) ? 4'b1 : 4'b0;
            end
            default: 
                    out_c = 4'b0;
        endcase
    end
endmodule
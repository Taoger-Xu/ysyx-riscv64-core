module mux41 (d0,d1,d2,d3,sel,out);
    input d0, d1, d2, d3;
    input [1:0] sel;
    output out;
    //当sel == 00或01的时候选择d0
    assign out = sel[0] ? (sel[1]? d3 : d1) : d0;
endmodule
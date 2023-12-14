module TM_test(
    input [0:0] clk1,
    input [8:0] features,
    input [17:0] ex_datapath,
    output [1:0] final_class
);

reg [8:0] features_reg;
reg [17:0] ex1_reg, ex2_reg, ex3_reg, ex4_reg, ex5_reg, ex6_reg, ex7_reg, ex8_reg, ex9_reg, ex10_reg, ex11_reg, ex12_reg;
wire [1:0] class_wire;
reg [3:0] read_counter; 

always @(posedge clk1) begin
    features_reg <= features;

    case(read_counter)
        4'b0000: ex1_reg <= ex_datapath[17:0];
        4'b0001: ex2_reg <= ex_datapath[17:0];
        4'b0010: ex3_reg <= ex_datapath[17:0];
        4'b0011: ex4_reg <= ex_datapath[17:0];
        4'b0100: ex5_reg <= ex_datapath[17:0];
        4'b0101: ex6_reg <= ex_datapath[17:0];
        4'b0110: ex7_reg <= ex_datapath[17:0];
        4'b0111: ex8_reg <= ex_datapath[17:0];
        4'b1000: ex9_reg <= ex_datapath[17:0];
        4'b1001: ex10_reg <= ex_datapath[17:0];
        4'b1010: ex11_reg <= ex_datapath[17:0];
        4'b1011: ex12_reg <= ex_datapath[17:0];
        default: // 
    endcase

    if (read_counter < 12) begin
        read_counter <= read_counter + 1;
    end
end

// Instantisalize testing block 
Inference DUT(
    features_reg,
    ex1_reg,
    ex2_reg,
    ex3_reg,
    ex4_reg,
    ex5_reg,
    ex6_reg,
    ex7_reg,
    ex8_reg,
    ex9_reg,
    ex10_reg,
    ex11_reg,
    ex12_reg,   
    class_wire
);

assign final_class = class_wire;

endmodule

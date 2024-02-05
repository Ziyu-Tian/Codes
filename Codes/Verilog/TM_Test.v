
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 30.11.2023 07:05:36
// Design Name: 
// Module Name: TM_test
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module TM_test(

    input      [0:0] clk,
    output reg [1:0] final_class
);

reg [1:0]      features_reg;
reg [3:0]      ex1_reg;
reg [3:0]      ex2_reg;
reg [3:0]      ex3_reg;
reg [3:0]      ex4_reg;
reg [3:0]      ex5_reg;
reg [3:0]      ex6_reg;
reg [3:0]      ex7_reg;
reg [3:0]      ex8_reg;
wire [1:0]     class_wire;

always @(posedge clk) begin
        features_reg  <= 2'b11 ;  
        ex1_reg  <= 4'b1100 ;
        ex2_reg  <= 4'b1001 ;
        ex3_reg  <= 4'b1100 ;
        ex4_reg  <= 4'b0110 ;
        ex5_reg  <= 4'b0101 ;
        ex6_reg  <= 4'b1100 ;
        ex7_reg  <= 4'b0001 ;
        ex8_reg  <= 4'b0011 ;
        final_class <= class_wire;
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
    class_wire
);
endmodule


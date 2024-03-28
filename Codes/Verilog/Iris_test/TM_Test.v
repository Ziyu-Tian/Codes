
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

    input      [0:0] clk1,
    input      [0:0] rst,
    input      [8:0] features,
    output reg [31:0] final_class
);

reg [8:0]      features_reg;
reg [17:0]      ex1_reg;
reg [17:0]      ex2_reg;
reg [17:0]      ex3_reg;
reg [17:0]      ex4_reg;
reg [17:0]      ex5_reg;
reg [17:0]      ex6_reg;
reg [17:0]      ex7_reg;
reg [17:0]      ex8_reg;
reg [17:0]      ex9_reg;
reg [17:0]      ex10_reg;
reg [17:0]      ex11_reg;
reg [17:0]      ex12_reg;
wire [1:0]     class_wire;


always @(posedge clk1) 
    begin
        if (rst == 1'b0) 
        begin
        features_reg  <= features;  
        ex1_reg  <= 18'b011001011100111100;
        ex2_reg  <= 18'b001011011110101111;
        ex3_reg  <= 18'b011001011100110100;
        ex4_reg  <= 18'b001001001111110111;
        ex5_reg  <= 18'b011101001111111110;
        ex6_reg  <= 18'b111111110111111111;
        ex7_reg  <= 18'b001011001110100110;
        ex8_reg  <= 18'b011001111100111100;
        ex9_reg  <= 18'b111111100111110111;
        ex10_reg <= 18'b111111111111111101;
        ex11_reg <= 18'b001011000111111111;
        ex12_reg <= 18'b011111111100111101;

        features_reg <= 9'b0;    
        end
        
        else begin
           features_reg <= features; 
           final_class <= {30'b0,class_wire};
        end
    end

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
endmodule


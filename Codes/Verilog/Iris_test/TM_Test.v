
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
    input      [8:0] features,
    input      [17:0] ex1,
    input      [17:0] ex2,
    input      [17:0] ex3,
    input      [17:0] ex4,
    input      [17:0] ex5,
    input      [17:0] ex6,
    input      [17:0] ex7,
    input      [17:0] ex8,
    input      [17:0] ex9,
    input      [17:0] ex10,
    input      [17:0] ex11,
    input      [17:0] ex12,
    output     [1:0] final_class
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


always @(posedge clk1) begin
        features_reg  <= features;  
        ex1_reg  <= ex1 ;
        ex2_reg  <= ex2 ;
        ex3_reg  <= ex3 ;
        ex4_reg  <= ex4 ;
        ex5_reg  <= ex5 ;
        ex6_reg  <= ex6 ;
        ex7_reg  <= ex7 ;
        ex8_reg  <= ex8 ;
        ex9_reg  <= ex9 ;
        ex10_reg  <= ex10 ;
        ex11_reg  <= ex11 ;
        ex12_reg  <= ex12 ;

end

assign class_wire = final_class;
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
endmodule


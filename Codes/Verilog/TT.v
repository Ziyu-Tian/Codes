
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


module TT(

    input      [0:0] clk,
    input      [0:0] rst,
    input        
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

always @(posedge clk)
  begin
    if ( rst == 1'b0 )
    begin
      ex1_reg  <= 4'b1100 ;
      ex2_reg  <= 4'b1001 ;
      ex3_reg  <= 4'b1100 ;
      ex4_reg  <= 4'b0110 ;
      ex5_reg  <= 4'b0101 ;
      ex6_reg  <= 4'b1100 ;
      ex7_reg  <= 4'b0001 ;
      ex8_reg  <= 4'b0011 ;
       
    end  

    else begin
        features_reg  <= 2'b11 ;  
        end
            
         
    end

always @( posedge S_AXI_ACLK )
    begin
	  if ( S_AXI_ARESETN == 1'b0 )
	    begin
	      sum <= 0;
	      slv_reg2 <= 0;
	    end 
	  else begin
	    sum         <= slv_reg0 + slv_reg1;
	    slv_reg2    <= sum[$high(sum)-1:0]; // lower 32 bits 
	    slv_reg3[0] <= sum[$high(sum)];     // top carry bit
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
    class_wire
);

endmodule


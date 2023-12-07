
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 27.12.2021 15:13:16
// Design Name: 
// Module Name: ClauseCalculation for XOR
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



module ClauseCalculation(
    input [2-1:0] features,
    input [4-1:0] exclude_state,
    output wire clause
    );
    
    wire [4-1:0] literals;
    reg [4-1:0] in_and;


    integer i;
    
    assign literals={features,~features};
    
    always @(literals,exclude_state)
    begin  
        for (i=0; i<4; i=i+1)
        begin
            
            if (exclude_state[i]==1)
            in_and[i]=1;
            else
            in_and[i]=literals[i]; 
            
        end
        
        
    end
    
    assign clause = &in_and;
    
endmodule

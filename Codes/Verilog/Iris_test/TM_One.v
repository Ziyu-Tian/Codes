
// Main inference Module

module Inference(
    input [2-1:0] features,
    output wire [1:0] final_class
    //input [4-1:0] clause_outputs,
    //input [4-1:0] clause_outputs_2
    );

wire [4-1:0] clause_outputs;

wire [4-1:0] clause_outputs_2;

wire [1:0] class;

reg [3:0]      exclude_state1;
reg [3:0]      exclude_state2;
reg [3:0]      exclude_state3;
reg [3:0]      exclude_state4;
reg [3:0]      exclude_state5;
reg [3:0]      exclude_state6;
reg [3:0]      exclude_state7;
reg [3:0]      exclude_state8;

exclude_state1  <= 4'b1100 ;
exclude_state2  <= 4'b1001 ;
exclude_state3  <= 4'b1100 ;
exclude_state4  <= 4'b0110 ;
exclude_state5  <= 4'b0101 ;
exclude_state6  <= 4'b1100 ;
exclude_state7  <= 4'b0001 ;
exclude_state8  <= 4'b0011 ;


//wire [3:0] clause_1;

//wire [3:0] clause_2;


//Class 0
ClauseCalculation CC1 (features,exclude_state1,clause_outputs[3]);
ClauseCalculation CC2 (features,exclude_state2,clause_outputs[1]);
ClauseCalculation CC3 (features,exclude_state3,clause_outputs[2]);
ClauseCalculation CC4 (features,exclude_state4,clause_outputs[0]);
// Class 1
ClauseCalculation CC5 (features,exclude_state5,clause_outputs_2[3]);
ClauseCalculation CC6 (features,exclude_state6,clause_outputs_2[1]);
ClauseCalculation CC7 (features,exclude_state7,clause_outputs_2[2]);
ClauseCalculation CC8 (features,exclude_state8,clause_outputs_2[0]);

//assign clause_1 = 4'b0100;
//assign clause_2 = 4'b1100;

Sum_And_Threshold SAT(clause_outputs[3:2],clause_outputs[1:0],clause_outputs_2[3:2],clause_outputs_2[1:0],class);

assign final_class = class;


   
endmodule

//------------------------------------------------------------------------------
// CC Module 

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

//----------------------------------------
// Sum and Threshold Module 

module Sum_And_Threshold(
    input wire [1:0] pos_clause_1,
    input wire [1:0] neg_clause_1,
    input wire [1:0] pos_clause_2,
    input wire [1:0] neg_clause_2,
    //output wire [2:0] votes1,
    //output wire [2:0] votes2,
    output reg [1:0] class 
);

integer i, m;
integer countp_1, countn_1;
integer countp_2, countn_2;

always @(pos_clause_1, neg_clause_1) begin
    countp_1 = 0;
    countn_1 = 0;

    for (i = 0; i < 2; i = i + 1) begin
        if (pos_clause_1[i] == 1)
            countp_1 = countp_1 + 1;
    end

    for (i = 0; i < 2; i = i + 1) begin
        if (neg_clause_1[i] == 1)
            countn_1 = countn_1 + 1;
    end
end

always @(pos_clause_2, neg_clause_2) begin
    countp_2 = 0;
    countn_2 = 0;

    for (m = 0; m < 2; m = m + 1) begin
        if (pos_clause_2[m] == 1)
            countp_2 = countp_2 + 1;
    end

    for (m = 0; m < 2; m = m + 1) begin
        if (neg_clause_2[m] == 1)
            countn_2 = countn_2 + 1;
    end
end


//assign votes1 = countp_1 - countn_1;
//assign votes2 = countp_2 - countn_2;

always @* begin
    class = ((countp_1 - countn_1) >= (countp_2 - countn_2)) ? 2'b00 : 2'b01; 
end

endmodule

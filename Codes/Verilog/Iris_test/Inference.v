module Inference(
    input [9-1:0] features,
    input [18-1:0] exclude_state1,
    input [18-1:0] exclude_state2,
    input [18-1:0] exclude_state3,
    input [18-1:0] exclude_state4,
    input [18-1:0] exclude_state5,
    input [18-1:0] exclude_state6,
    input [18-1:0] exclude_state7,
    input [18-1:0] exclude_state8,
    input [18-1:0] exclude_state9,
    input [18-1:0] exclude_state10,
    input [18-1:0] exclude_state11,
    input [18-1:0] exclude_state12,
    output wire [1:0] final_class
    //input [4-1:0] clause_outputs,
    //input [4-1:0] clause_outputs_2
    );

wire [4-1:0] clause_outputs;

wire [4-1:0] clause_outputs_2;

wire [4-1:0] clause_outputs_3;

wire [1:0] class;


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
// Class 2
ClauseCalculation CC9 (features,exclude_state9,clause_outputs_3[3]);
ClauseCalculation CC10 (features,exclude_state10,clause_outputs_3[1]);
ClauseCalculation CC11 (features,exclude_state11,clause_outputs_3[2]);
ClauseCalculation CC12 (features,exclude_state12,clause_outputs_3[0]);

//assign clause_1 = 4'b0100;
//assign clause_2 = 4'b1100;

Sum_And_Threshold SAT(clause_outputs[3:2],clause_outputs[1:0],clause_outputs_2[3:2],clause_outputs_2[1:0],clause_outputs_3[3:2],clause_outputs_3[1:0],class);

assign final_class = class;


   
endmodule

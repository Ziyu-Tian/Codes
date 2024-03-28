
module ClauseCalculation(
    input [9-1:0] features,
    input [18-1:0] exclude_state,
    output wire clause
    );
    
    wire [18-1:0] literals;
    reg [18-1:0] in_and;


    integer i;
    
    assign literals={features,~features};
    
    always @(literals,exclude_state)
    begin  
        for (i=0; i<18; i=i+1)
        begin
            
            if (exclude_state[i]==1)
            in_and[i]=1;
            else
            in_and[i]=literals[i]; 
            
        end
        
        
    end
    
    assign clause = &in_and;
    
endmodule

//1,0,0,1,1,0,1,0,0,0,1,1,0,0,1,0,1,1 ->0
//1,1,1,1,1,0,1,1,0                   ->1
//1,1,1,1,0,0,1,1,1                   ->2
/*
//0 1 1 1 0 1 0 1 1 1 0 0 1 1 0 1 0 0
//0 0 1 0 0 1 1 0 1 1 1 1 1 1 0 1 1 0
//0 1 1 0 0 1 0 1 1 1 0 0 1 1 0 1 0 0
//0 0 1 0 1 1 0 0 1 1 1 1 1 0 0 1 1 1
//0 0 1 0 0 1 1 0 1 1 1 1 1 1 0 1 1 0
//1 1 1 1 1 0 1 1 1 1 1 1 1 1 1 1 1 1
//0 0 1 0 1 1 1 1 1 1 1 0 1 0 0 1 1 1
//1 1 1 0 1 1 1 1 0 1 1 1 1 1 1 1 1 1
//0 1 1 1 1 1 1 1 0 1 1 1 1 1 0 1 1 1
//1 1 1 0 1 1 1 1 1 1 1 1 1 1 1 1 0 1
//1 1 1 0 1 1 0 0 0 1 1 1 1 1 1 1 1 1
//0 1 1 1 0 1 0 1 1 1 1 0 1 1 1 1 0 0
*/
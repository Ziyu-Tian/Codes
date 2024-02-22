module ST(
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

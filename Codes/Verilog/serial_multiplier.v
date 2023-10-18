module multiplyer(input [7:0]a, input [7:0]b, output reg [15:0]p, input clk);
reg [7:0]multiplier;
integer i;

always @(posedge clk)
begin
    p = 0;
    multiplier = b;
    for(i = 0; i < 8; i = i + 1)
    begin
        if(a[i])
            p = p + (multiplier << i);
    end
end

endmodule


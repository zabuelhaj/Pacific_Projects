module MUX(input logic control, input logic [5:0] sum, input logic [5:0] diff, output logic [5:0] outVal);

always_comb begin
	if(control==1'b1)
		outVal<=sum;
	else if(control==1'b0)
		outVal<=diff;
	else
		outVal<=diff;
end

endmodule

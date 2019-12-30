module SUB(input logic [5:0] inA, input logic [5:0] inB, output logic OF_D, output logic [5:0] diff);

always_comb begin
	diff=(inA+(~inB+6'b1));
		
	if(inA[5]==~inB[5]&&inA[5]!=diff[5])
		OF_D<=1'b1;
	else
		OF_D<=1'b0;
	
end

endmodule

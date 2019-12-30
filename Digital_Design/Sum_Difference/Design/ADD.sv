module ADD(input logic [5:0] inA, input logic [5:0] inB, output logic OF_S, output logic [5:0] sum);

always_comb begin
	sum=(inA+inB);
		
	if(inA[5]==inB[5]&&inA[5]!=sum[5])
		OF_S<=1'b1;
	else
		OF_S<=1'b0;
	
end

endmodule

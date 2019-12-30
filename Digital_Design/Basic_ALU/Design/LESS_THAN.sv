module LESS_THAN( input logic [5:0] inA,
						input logic [5:0] inB,
						output logic [5:0] less);
							
always_comb begin
	if((inA[5]<inB[5])) begin
		less<=6'd0;
	end else if((inA[5]==inB[5])&&(inA[5]==1'b1)&&((~inB+1'b1)>(~inA+1'b1))) begin
		less<=6'd0;
	end else if((inA[5]==inB[5])&&(inA[5]==1'b0)&&(inB<inA)) begin
		less<=6'd0;
	end else begin
		less<=6'd1;
	end
end

endmodule 
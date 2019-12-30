module GREATER_THAN( input logic [5:0] inA,
							input logic [5:0] inB,
							output logic [5:0] great);
							
always_comb begin
	if((inA[5]<inB[5])) begin
		great<=6'd1;
	end else if((inA[5]==inB[5])&&(inA[5]==1'b1)&&((~inB+1'b1)>(~inA+1'b1))) begin
		great<=6'd1;
	end else if((inA[5]==inB[5])&&(inA[5]==1'b0)&&(inB<inA)) begin
		great<=6'd1;
	end else begin
		great<=6'd0;
	end
end

endmodule 

//(((inA[5]<inB[5])||((inA[5]>inB[5])&&((~inA+1'b1)<inB))))
module EQUAL_ZERO( input logic [5:0] inA,
						input logic [5:0] inB,
						output logic [5:0] zip);
							
always_comb begin
	if(inA==6'd0||(~inA+1'b1)==6'd0) begin
		zip<=6'd1;
	end else begin
		zip<=6'd0;
	end
end

endmodule 
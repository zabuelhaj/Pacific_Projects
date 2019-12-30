module EQUALS( input logic [5:0] inA,
					input logic [5:0] inB,
					output logic [5:0] eq);
							
always_comb begin
	if(inA==inB) begin
		eq<=6'd1;
	end else begin
		eq<=6'd0;
	end
end

endmodule 
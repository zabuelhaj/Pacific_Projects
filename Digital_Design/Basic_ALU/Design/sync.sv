module sync (input logic clk, button, reset, 
				 output logic fall_edge);

logic in_ff;
logic in_sync2;
logic in_sync1;

always_ff @(posedge clk or negedge reset) begin
	if(!reset) begin
		in_ff<=1'b1;
		in_sync1<=1'b1;
		in_sync2<=1'b1;
	end else begin
		in_ff<=button;
		in_sync1<=in_ff;
		in_sync2<=in_sync1;
	end
end

always_comb begin
	fall_edge<=~(~in_sync1&&in_sync2);
end

endmodule 
module sync (input logic clk, input logic button, output logic in_sync1, fall_edge);

logic in_ff;
logic in_sync2;

always_ff @(posedge clk) begin
	in_ff<=button;
	in_sync1<=in_ff;
	in_sync2<=in_sync1;
end

always_comb begin
	fall_edge<=~(~in_sync1&&in_sync2);
end

endmodule
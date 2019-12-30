module resetSync(input logic clk, input logic reset, output logic resetsync);

logic intreset;

always_ff@(posedge clk or negedge reset) begin
	if(!reset)
		begin
			intreset<=1'b0;
			resetsync<=1'b0;
		end
	else
		begin
			intreset<=1'b1;
			resetsync<=intreset;
		end
end

endmodule 
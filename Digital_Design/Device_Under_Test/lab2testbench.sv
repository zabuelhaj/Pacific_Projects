`timescale 1ns/1ns
module lab2testbench();

	// DUT signals
	logic clk = 1'b0;
	logic w, z;
	logic [3:0]val[0:15]={4'd0,4'd1,4'd2,4'd3,4'd4,4'd5,4'd6,4'd7,4'd8,4'd9,4'd10,4'd11,4'd12,4'd13,4'd14,4'd15};
	
	// Connect device to test
	lab2design dutLab2(.clk(clk),.w(w),.z(z));
	
	// Generate clock
	always #10 clk<=~clk;

	// Generate inputs
	/*initial
	repeat(3) begin
		@(negedge clk);
		w<=1'b1;
		@(negedge clk);
		w<=1'b1;
		@(negedge clk);
		w<=1'b1;
	end*/

	task testCase(input logic [3:0] inval);
		@(negedge clk);
		w<=inval[0];
		@(negedge clk);
		w<=inval[1];
		@(negedge clk);
		w<=inval[2];
		@(negedge clk);
		w<=inval[3];
	endtask

	initial
	begin
		for(int i=0;i<16;i++) begin
			testCase(val[i]);
			@(posedge clk);
			checkOut:assert(z==1'b1)$display("Output is correct! %b", val[i]);
				else $display("Try again.");
		end

		@(posedge clk);
		checkOut:assert(z==1'b1)$display("Output is correct!");
			else $display("Try again.");
		$stop;
	end

endmodule
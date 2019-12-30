module syncTest();
	logic clk=1'b0;
	logic A;
	logic unlock;

	sync synchroize(.clk(clk),.button(A),.fall_edge(unlock));

	always #50 clk<=~clk;

	task testCase(input logic inVal);
		@(negedge clk);
		A<=inVal;
	endtask

	initial begin
		testCase(1'b1);
		testCase(1'b0);
		testCase(1'b0);
		testCase(1'b1);
		testCase(1'b1);
		testCase(1'b1);
		testCase(1'b0);
		testCase(1'b0);
		testCase(1'b0);
		testCase(1'b0);
		testCase(1'b1);
		testCase(1'b1);
		testCase(1'b1);
		testCase(1'b1);
		testCase(1'b1);
		testCase(1'b1);
		$stop;
	end
endmodule
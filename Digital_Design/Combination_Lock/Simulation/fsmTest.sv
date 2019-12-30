module fsmTest();
	logic clk=1'b0;
	logic [2:0] ABC;
	logic led;

	comb_FSM machine(.clk(clk),.switch(ABC),.led(led));

	always #50 clk<=~clk;

	task testCase(input logic [2:0] inval);
		@(negedge clk);
		ABC<=inval;
	endtask

	initial begin
		testCase(3'b101);
		testCase(3'b110);
		testCase(3'b011);
		testCase(3'b101);
		testCase(3'b101);
		testCase(3'b101);
		testCase(3'b101);
		testCase(3'b111);
		testCase(3'b111);
		testCase(3'b111);
		testCase(3'b111);
		testCase(3'b101);
		testCase(3'b110);
		testCase(3'b011);
		testCase(3'b101);
		testCase(3'b110);
		testCase(3'b011);
		testCase(3'b110);
		testCase(3'b011);
		$stop;
	end
endmodule
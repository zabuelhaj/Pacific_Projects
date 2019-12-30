module sign_magTest();
	logic clk=1'b0;
	logic [5:0] mag;
	logic sign;
	logic [5:0] value;
	logic [5:0]val[0:4]={6'b101010,6'b010111,6'b111111,6'b011111,6'b000011};

	sign_mag magnitude(.value(value),.mag(mag),.sign(sign));

	always #10 clk<=~clk;

	task testCase(input logic [5:0] inval);
		value<=inval;
	endtask

	initial
	begin
		for(int i=0;i<5;i++) begin
			testCase(val[i]);
			@(posedge clk);
			checkOut:assert(sign==1'b1)$display("Output is negative.");
				else $display("Output is positive.");
		end
		$stop;
	end
endmodule

module FSMTest();
	logic clk=1'b0;
	logic in_vec [0:15]={1'b1,1'b1,1'b1,1'b0,1'b1,1'b0,1'b0,1'b0,1'b1,1'b0,1'b1,1'b0,1'b0,1'b1,1'b1,1'b1};
	logic Add_Sub;
	logic A_D;

	Add_Sub_FSM controller(.clk(clk),.Add_Sub(Add_Sub),.A_D(A_D));

	always #50 clk<=~clk;

	task testCase(input logic inval);
		@(negedge clk);
		Add_Sub<=inval;
	endtask

	initial begin
		for(int i=0;i<16;i++) begin
			testCase(in_vec[i]);
			@(posedge clk);
			checkOut:assert(A_D==1'b1)$display("Adding the numbers.");
				else $display("Subtracting the numbers.");
		end
		$stop;
	end
endmodule
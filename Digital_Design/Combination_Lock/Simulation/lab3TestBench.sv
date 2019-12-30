module lab3TestBench();
	logic clk=1'b0;
	logic [2:0] button;
	logic led;
	logic [2:0] temp;
	int i;

	Comb_Lock lockDUT(.clk(clk),.comb(button),.unlock(led));

	always #50 clk<=~clk;

	task testCase(input logic [2:0] inVal);
		repeat($urandom_range(15,5))@(negedge clk);
		button<=inVal;
		@(posedge clk);
		checkIn:assert(button==3'b000||button==3'b001||button==3'b010||button==3'b100)$display("Cannot input more than one value: LOCKED.");
			else $display("");
	endtask

	initial begin
		
		for (i=0;i<10;i++)begin
			temp = $urandom_range(8,1);
			$display("ABC Input Combination: %b", temp);
			testCase(temp);
			@(posedge clk);
			checkOut:assert(led==1'b1)$display("System is unlocked!");
				else $display("System is LOCKED");
		end
		$stop;
	end

endmodule
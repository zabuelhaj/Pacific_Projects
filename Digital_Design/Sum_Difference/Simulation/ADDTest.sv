module ADDTest();
	logic clk=1'b0;
	logic [5:0] inA;
	logic [5:0] inB;
	logic OF_S;
	logic [5:0] sum;
	logic [5:0]A_val[0:7]={6'd15,6'd1,6'd14,6'd3,6'd32,6'd64,6'd6,6'd7};
	logic [5:0]B_val[0:7]={6'd8,6'd9,6'd10,6'd11,6'd32,6'd13,6'd14,6'd15};

	ADD adder(.inA(inA),.inB(inB),.OF_S(OF_S),.sum(sum));

	always #50 clk<=~clk;

	task testCase(input logic [5:0] invalA, input logic [5:0] invalB);
		@(negedge clk);
		inA<=invalA;
		inB<=invalB;
	endtask

	initial begin
		for(int i=0;i<8;i++) begin
			testCase(A_val[i],B_val[i]);
			@(posedge clk);
			checkOut:assert(OF_S==1'b0)$display("No overflow.");
				else $display("Overflow.");
		end
		$stop;
	end
endmodule
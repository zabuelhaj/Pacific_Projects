module MUXTest();
	logic clk=1'b0;
	logic con_vec [0:15]={1'b1,1'b1,1'b1,1'b0,1'b1,1'b0,1'b0,1'b0,1'b1,1'b0,1'b1,1'b0,1'b0,1'b1,1'b1,1'b1};
	logic sum_val [0:15]={1'b1,1'b1,1'b1,1'b0,1'b0,1'b1,1'b1,1'b1,1'b0,1'b0,1'b1,1'b0,1'b0,1'b1,1'b1,1'b1};
	logic diff_val [0:15]={1'b1,1'b1,1'b1,1'b0,1'b1,1'b0,1'b0,1'b1,1'b1,1'b1,1'b0,1'b0,1'b1,1'b0,1'b0,1'b1};
	logic control;
	logic sum;
	logic diff;
	logic outVal;

	MUX select(.control(control),.sum(sum),.diff(diff),.outVal(outVal));

	always #50 clk<=~clk;

	task testCase(input logic conval, input logic sumval, input logic diffval);
		@(negedge clk);
		control<=conval;
		sum<=sumval;
		diff<=diffval;
	endtask

	initial begin
		for(int i=0;i<16;i++) begin
			testCase(con_vec[i],sum_val[i],diff_val[i]);
			@(posedge clk);
			checkOut:assert(outVal==sum)$display("Displaying sum.");
				else $display("Displaying diff.");
		end
		$stop;
	end
endmodule
module hex_displayTest();
	logic clk=1'b0;
	logic [5:0] magnitude;
	logic [6:0] segment;
	logic [5:0]val[0:9]={6'd0,6'd1,6'd2,6'd3,6'd4,6'd5,6'd6,6'd7,6'd8,6'd9};

	hex_display display(.magnitude(magnitude),.segment(segment));

	always #10 clk<=~clk;

	task testCase(input logic [5:0] inval);
		@(posedge clk);
		magnitude<=inval;
	endtask

	initial
	begin
		for(int i=0;i<10;i++) begin
			testCase(val[i]);
		end
		$stop;
	end
endmodule

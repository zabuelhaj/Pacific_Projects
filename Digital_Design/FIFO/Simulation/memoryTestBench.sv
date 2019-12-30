module memoryTestBench();
	
	logic wen=1'b0, ren=1'b1, clk = 1'b0;
	logic [2:0] wrAddr=3'b000, rdAddr=3'b000;
	logic [14:0] datain, dataOut;
	
	always #50 clk <= ~clk;

	memModule dut(.clk(clk), .Wen(wen), .Ren(ren), .datain(datain), .wrAddr(wrAddr), .rdAddr(rdAddr), .dataOut(dataOut));
	
	task setRead(input logic [2:0] readin);
		@(negedge clk);
		ren<=1'b1;

		@(negedge clk);
		wen<=1'b0;
		
		@(negedge clk);
		rdAddr<=readin;
	endtask
	
	task setWrite(input logic [2:0] writein, input logic [14:0] data);
		@(negedge clk);
		ren<=1'b0;

		@(negedge clk);
		wen<=1'b1;
		
		@(negedge clk);
		wrAddr<=writein;

		@(negedge clk);
		datain<=data;
	endtask

	initial
	begin
		setWrite(3'd0, 15'h1100);
		setRead(3'd0);

		setWrite(3'd2, 15'd30001);
		setRead(3'd2);

		setWrite(3'd5, 15'd32001);
		setRead(3'd5);
		$stop;
	end
	
endmodule 
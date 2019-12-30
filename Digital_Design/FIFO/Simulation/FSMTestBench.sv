module FSMTestBench();

logic clk=1'b0;
logic button=1'b1;
logic mode=1'b0;
logic [3:0] Read_Ptr, Write_Ptr;
logic [2:0] out;
logic fifo_full;
logic fifo_empty;
logic modeSeq [0:7] = {1'b1,1'b0,1'b1,1'b1,1'b1,1'b0,1'b1,1'b1};
logic buttSeq [0:7] = {1'b0,1'b1,1'b0,1'b1,1'b0,1'b1,1'b0,1'b1};

deviceFSM FSM(.clk(clk),.button(button),.mode(mode),.Read_Ptr(Read_Ptr),.Write_Ptr(Write_Ptr),.out(out),.fifo_full(fifo_full),.fifo_empty(fifo_empty));

always #50 clk<=~clk;

task testCase(input logic inbutt,input logic inmode);

	@(negedge clk);
	button<=inbutt;
	@(negedge clk);
	button<=inbutt;
	@(negedge clk);
	button<=inbutt;
	@(negedge clk);
	button<=inbutt;
	@(negedge clk);
	mode<=inmode;
	@(negedge clk);
	mode<=inmode;
	@(negedge clk);
	mode<=inmode;
	@(negedge clk);
	mode<=inmode;

endtask

initial
begin
	for(int i=0;i<8;i++) begin
		testCase(buttSeq[i], modeSeq[i]);
	end
end

endmodule 
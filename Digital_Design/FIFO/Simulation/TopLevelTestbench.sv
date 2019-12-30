module TopLevelTestbench();

logic clk=1'b0;
logic button=1'b1;
logic mode=1'b0;
logic [14:0] datain;
//logic [2:0] Read_Ptr, Write_Ptr;
logic full;
logic empty;
logic [6:0] segment4, segment3, segment2, segment1;
logic modeSeq [0:7] = {1'b1,1'b0,1'b1,1'b0,1'b1,1'b0,1'b1,1'b1};
logic buttSeq [0:7] = {1'b0,1'b0,1'b0,1'b0,1'b0,1'b1,1'b0,1'b1};
logic [14:0] dataSeq [0:7] = {15'd32000,15'd30000,15'd10000,15'd1,15'd25000,15'd30110,15'd10250,15'd100};

FIFO DUT(.clk(clk),.button(button),.mode(mode),.datain(datain),.full(full),.empty(empty),.segment4(segment4),.segment3(segment3),.segment2(segment2),.segment1(segment1));

always #50 clk<=~clk;

task testCase(input logic inbutt,input logic inmode,input logic [14:0] indata);

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
	@(negedge clk);
	datain<=indata;
	@(negedge clk);
	datain<=indata;
	@(negedge clk);
	datain<=indata;
	@(negedge clk);
	datain<=indata;

endtask

initial
begin
	for(int i=0;i<8;i++) begin
		testCase(buttSeq[i], modeSeq[i], dataSeq[i]);
	end

	testCase(1'b0,1'b0, 15'h8000);
	$stop;
end

endmodule 
module devBSimulator();

logic clk=1'b0;
logic send=1'b0;
logic devB=1'b0;
logic devA, LED_devB, LED_send;
logic sendSeq [0:4] = {1'b1,1'b0,1'b1,1'b1,1'b1};
logic devBSeq [0:4] = {1'b0,1'b1,1'b0,1'b1,1'b0};

Handshaking DeviceA(.clk(clk),.send(send),.devB(devB),.devA(devA),.LED_devB(LED_devB),.LED_send(LED_send));

always #100 clk<=~clk;

task testCase(input logic insend,input logic indevB);

	@(negedge clk);
	send<=insend;
	@(negedge clk);
	send<=insend;
	@(negedge clk);
	send<=insend;
	@(negedge clk);
	send<=insend;
	@(negedge clk);
	devB<=indevB;
	@(negedge clk);
	devB<=indevB;
	@(negedge clk);
	devB<=indevB;
	@(negedge clk);
	devB<=indevB;

endtask

initial
begin
	for(int i=0;i<8;i++) begin
		testCase(sendSeq[i], devBSeq[i]);
	end
end

endmodule 
module toplevelTestbench();
logic clk=1'b0;
logic button=1'b1;
logic mode=1'b1;
logic reset=1'b0;
logic [14:0] dataIn;
logic full;
logic empty;
logic [6:0] gex_A1;
logic [6:0] gex_A2;
logic [6:0] gex_B1;
logic [6:0] gex_B2;
logic [6:0] gex_C1;
logic [6:0] gex_C2;
logic OF_LED, A_LED, B_LED, C_LED;
logic [14:0] tempIn;
logic tempB;
logic tempRw;
logic tempR;
logic [2:0] tempS;

int fileHandle = $fopen("topLevelSuite.txt","a");

ALU mrSkeletal(.clk(clk),.button(button),.mode(mode),.reset(reset),.dataIn(dataIn),.full(full),.empty(empty),.gex_A1(gex_A1),.gex_A2(gex_A2),.gex_B1(gex_B1),.gex_B2(gex_B2),.gex_C1(gex_C1),.gex_C2(gex_C2),.OF_LED(OF_LED),.A_LED(A_LED),.B_LED(B_LED),.C_LED(C_LED));

always #50 clk<=~clk;

task writeCase(input logic [14:0] inval, input logic pb, input logic rw, input logic r);
	@(negedge clk);
	dataIn<=inval;
	mode<=1'b1;
	reset<=r;

	repeat(5)@(negedge clk);
	button<=1'b0;
	repeat(5)@(negedge clk);
	button<=1'b1;
endtask

task readCase(input logic [14:0] inval, input logic pb, input logic rw, input logic r, input logic [2:0] select);
	@(negedge clk);
	dataIn<=inval;
	mode<=1'b0;
	reset<=r;

	repeat(5)@(negedge clk);
	button<=1'b0;
	repeat(5)@(negedge clk);
	button<=1'b1;

	repeat(5)@(negedge clk);

	case(select)
		3'b000: assert((mrSkeletal.A_mag+mrSkeletal.B_mag)==mrSkeletal.C_mag)$fdisplay(fileHandle, "Adding: %d + %d = %d when A_sign: %d B_sign: %d C_sign: %d Overflow: %d", mrSkeletal.A_mag, mrSkeletal.B_mag, mrSkeletal.C_mag, A_LED, B_LED, C_LED, OF_LED);
			else $fdisplay(fileHandle, "No luck.");
		3'b001: assert((mrSkeletal.A_mag-mrSkeletal.B_mag)==mrSkeletal.C_mag)$fdisplay(fileHandle, "Subtracting: %d - %d = %d when A_sign: %d B_sign: %d C_sign: %d Overflow: %d", mrSkeletal.A_mag, mrSkeletal.B_mag, mrSkeletal.C_mag, A_LED, B_LED, C_LED, OF_LED);
			else $fdisplay(fileHandle, "No luck.");
		3'b100: assert((mrSkeletal.A_mag==mrSkeletal.B_mag)==mrSkeletal.C_mag)$fdisplay(fileHandle, "Equal: %d = %d, %d when A_sign: %d B_sign: %d", mrSkeletal.A_mag, mrSkeletal.B_mag, mrSkeletal.C_mag, A_LED, B_LED);
			else $fdisplay(fileHandle, "No luck.");
		3'b101: assert((mrSkeletal.A_mag>mrSkeletal.B_mag)==mrSkeletal.C_mag)$fdisplay(fileHandle, "Greater: %d > %d, %d when A_sign: %d B_sign: %d", mrSkeletal.A_mag, mrSkeletal.B_mag, mrSkeletal.C_mag, A_LED, B_LED);
			else $fdisplay(fileHandle, "No luck.");
		3'b110: assert((mrSkeletal.A_mag<mrSkeletal.B_mag)==mrSkeletal.C_mag)$fdisplay(fileHandle, "Less: %d < %d, %d when A_sign: %d B_sign: %d", mrSkeletal.A_mag, mrSkeletal.B_mag, mrSkeletal.C_mag, A_LED, B_LED);
			else $fdisplay(fileHandle, "No luck.");
		3'b111: assert((mrSkeletal.A_mag==1'b0)==mrSkeletal.C_mag)$fdisplay(fileHandle, "Equals Zero: %d = 0, %d when A_sign: %d", mrSkeletal.A_mag, mrSkeletal.C_mag, A_LED);
			else $fdisplay(fileHandle, "No luck.");
	endcase

endtask

`include "toplevelTestbenchRandomWrite.sv"
`include "toplevelTestbenchRandomRead.sv"

initial begin

	//Write Directed

	writeCase(15'd29289,1'b0,1'b1,1'b1);
	repeat(5)@(negedge clk);

	writeCase(15'd29290,1'b0,1'b1,1'b1);
	repeat(5)@(negedge clk);

	writeCase(15'd29288,1'b0,1'b1,1'b1);
	repeat(5)@(negedge clk);

	writeCase(15'd29288,1'b0,1'b1,1'b1);
	repeat(5)@(negedge clk);

	writeCase(15'd29294,1'b1,1'b1,1'b1);
	repeat(5)@(negedge clk);

	writeCase(15'd29294,1'b0,1'b0,1'b1);
	repeat(5)@(negedge clk);

	writeCase(15'd29291,1'b1,1'b1,1'b1);
	repeat(5)@(negedge clk);

	writeCase(15'd29291,1'b0,1'b1,1'b1);
	repeat(5)@(negedge clk);

	writeCase(15'd29295,1'b0,1'b1,1'b1);
	repeat(5)@(negedge clk);

	//Read Directed
	
	readCase(15'd29295,1'b0,1'b0,1'b1, 1'd1);
	repeat(5)@(negedge clk);

	readCase(15'd29295,1'b0,1'b0,1'b1, 1'd2);
	repeat(5)@(negedge clk);

	readCase(15'd29295,1'b0,1'b0,1'b1, 1'd0);
	repeat(5)@(negedge clk);

	readCase(15'd29295,1'b0,1'b0,1'b1, 1'd0);
	repeat(5)@(negedge clk);

	readCase(15'd29295,1'b0,1'b0,1'b1, 1'd6);
	repeat(5)@(negedge clk);

	readCase(15'd29295,1'b0,1'b0,1'b1, 1'd6);
	repeat(5)@(negedge clk);

	readCase(15'd29295,1'b0,1'b0,1'b1, 1'd2);
	repeat(5)@(negedge clk);

	readCase(15'd29295,1'b0,1'b0,1'b1, 1'd2);
	repeat(5)@(negedge clk);

	readCase(15'd29295,1'b0,1'b0,1'b1, 1'd7);
	repeat(5)@(negedge clk);

	//Write Random

	for(int i=0;i<100;i++)begin
		tempIn=$urandom_range(32767,0);
		tempB=$urandom_range(1,0);
		tempRw=$urandom_range(1,0);
		if(i>40&&i<45)
			tempR=1'b0;
		else
			tempR=1'b1;
		testCaseRandomWrite(tempIn,tempB,tempRw,tempR);
		repeat($urandom_range(15,1))@(negedge clk);
	end
	repeat(5)@(negedge clk);

	//Read Random

	for(int i=0;i<100;i++)begin
		tempIn=$urandom_range(32767,0);
		tempB=$urandom_range(1,0);
		tempRw=$urandom_range(1,0);
		tempS=$urandom_range(7,0);
		if(i>40&&i<45)
			tempR=1'b0;
		else
			tempR=1'b1;
		testCaseRandomRead(tempIn,tempB,tempRw,tempR,tempS);
		repeat($urandom_range(15,1))@(negedge clk);
	end
	repeat(5)@(negedge clk);

	//$stop;
end

endmodule 
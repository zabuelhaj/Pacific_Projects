task testCaseRandomWrite(input logic [14:0] inval, input logic pb, input logic rw, input logic r);
	repeat($urandom_range(15,1))@(negedge clk);
	dataIn<=inval;
	mode<=1'b1;
	reset<=r;

	repeat($urandom_range(8,1))@(negedge clk);
	button<=pb;
	repeat($urandom_range(8,1))@(negedge clk);
	button<=1'b1;
endtask 
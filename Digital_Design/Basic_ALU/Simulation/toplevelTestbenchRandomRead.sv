task testCaseRandomRead(input logic [14:0] inval, input logic pb, input logic rw, input logic r, input logic [2:0] select);
	repeat($urandom_range(15,1))@(negedge clk);
	dataIn<=inval;
	mode<=1'b0;
	reset<=r;

	repeat($urandom_range(8,1))@(negedge clk);
	button<=pb;
	repeat($urandom_range(8,1))@(negedge clk);
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
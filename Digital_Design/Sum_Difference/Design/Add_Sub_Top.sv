module Add_Sub_Top(input logic clk, 
						 input logic [5:0] A, 
						 input logic [5:0] B, 
						 input logic Add_Sub, 
						 input logic button, 
						 output logic [6:0] hex_A1,
						 output logic [6:0] hex_A2, 
						 output logic [6:0] hex_B1,
						 output logic [6:0] hex_B2, 
						 output logic [6:0] hex_Answer1,
						 output logic [6:0] hex_Answer2, 
						 output logic OF_LED, 
						 output logic A_LED, 
						 output logic B_LED, 
						 output logic answer_LED);

logic press;
logic press_edge;
logic [5:0] A_D;
logic OF_S;
logic OF_D;
logic [5:0] sum;
logic [5:0] diff;
logic [5:0] sum_diff;
logic [5:0] answer_mag;
logic [5:0] A_mag;
logic [5:0] B_mag;
logic [3:0] display_A1;
logic [3:0] display_A2;
logic [3:0] display_B1;
logic [3:0] display_B2;
logic [3:0] display_Answer1;
logic [3:0] display_Answer2;
logic [6:0] answer1;
logic [6:0] answer2;
//logic sign_a;
//logic sign_b;
logic sign_c;
logic OF_sig;

sync syncronizer1(.clk(clk),.button(button),.in_sync1(press),.fall_edge(press_edge));

Add_Sub_FSM fsm(.clk(clk),.Add_Sub(Add_Sub),.A_D(A_D));

ADD adder(.inA(A),.inB(B),.OF_S(OF_S),.sum(sum));
SUB subtr(.inA(A),.inB(B),.OF_D(OF_D),.diff(diff));

always_ff@(posedge clk) begin
	if(Add_Sub==1'b0)begin
		sum_diff<=sum;
		OF_sig<=OF_S;
	end else begin
		sum_diff<=diff;
		OF_sig<=OF_D;
	end
end

//MUX addSub(.control(A_D),.sum(sum),.diff(diff),.outVal(sum_diff));
//MUX ofMux(.control(A_D),.sum(OF_S),.diff(OF_D),.outVal(OF_LED));

sign_mag Asign(.value(A),.mag(A_mag),.sign(A_LED));
sign_mag Bsign(.value(B),.mag(B_mag),.sign(B_LED));
sign_mag LastSign(.value(sum_diff),.mag(answer_mag),.sign(sign_c));

splicer vapour(.magnitude(A_mag),.tens_disp(display_A1),.ones_disp(display_A2));
splicer vapours(.magnitude(B_mag),.tens_disp(display_B1),.ones_disp(display_B2));
splicer vapoor(.magnitude(answer_mag),.tens_disp(display_Answer1),.ones_disp(display_Answer2));

hex_display Adisplay1(.magnitude(display_A1),.segment(hex_A1));
hex_display Adisplay2(.magnitude(display_A2),.segment(hex_A2));

hex_display Bdisplay1(.magnitude(display_B1),.segment(hex_B1));
hex_display Bdisplay2(.magnitude(display_B2),.segment(hex_B2));


//hex_display finaldisplay1(.magnitude(display_Answer1),.segment(answer1));
//hex_display finaldisplay2(.magnitude(display_Answer2),.segment(answer2));

hex_display finaldisplay1(.magnitude(display_Answer1),.segment(answer1));
hex_display finaldisplay2(.magnitude(display_Answer2),.segment(answer2));

always_ff@(posedge clk) begin

	if(press_edge==1'b0)
	begin
		hex_Answer1<=answer1;
		hex_Answer2<=answer2;
		answer_LED<=sign_c;
		OF_LED<=OF_sig;
	end

end

endmodule 
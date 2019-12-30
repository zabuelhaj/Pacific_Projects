module ALU( input logic clk, button, mode, reset,
				input logic [14:0] dataIn,
				output logic full, empty,
				output logic [6:0] gex_A1,
				output logic [6:0] gex_A2,
				output logic [6:0] gex_B1,
				output logic [6:0] gex_B2,
				output logic [6:0] gex_C1,
				output logic [6:0] gex_C2,
				output logic OF_LED, A_LED, B_LED, C_LED);
				
logic press_edge, resetsync;
logic [3:0] Read_Ptr, Write_Ptr;
logic Wen, Ren;
logic [14:0] dataOut;
logic [5:0] result;
logic [5:0] A_mag;
logic [5:0] B_mag;
logic [5:0] C_mag;
logic [3:0] display_A1;
logic [3:0] display_A2;
logic [3:0] display_B1;
logic [3:0] display_B2;
logic [3:0] display_C1;
logic [3:0] display_C2;

resetSync resetSwitch(.clk(clk),.reset(reset),.resetsync(resetsync));

sync pushbutton(.clk(clk),.button(button),.reset(resetsync),.fall_edge(press_edge));

ALU_FSM arithmetic_logic(.clk(clk),.button(press_edge),.mode(mode),.reset(resetsync),.Read_Ptr(Read_Ptr),.Write_Ptr(Write_Ptr),.Wen(Wen),.Ren(Ren),.fifo_full(full),.fifo_empty(empty));

memModule memory(.clk(clk),.Wen(Wen),.Ren(Ren),.datain(dataIn),.wrAddr(Write_Ptr[2:0]),.rdAddr(Read_Ptr[2:0]),.dataOut(dataOut));

Operation beepbeep(.clk(clk),.reset(resetsync),.A(dataOut[14:9]),.B(dataOut[8:3]),.select(dataOut[2:0]),.returns(result),.OF_LED(OF_LED));

sign_mag Asign(.value(dataOut[14:9]),.mag(A_mag),.sign(A_LED));
sign_mag Bsign(.value(dataOut[8:3]),.mag(B_mag),.sign(B_LED));
sign_mag CSign(.value(result),.mag(C_mag),.sign(C_LED));

splicer vapourA(.magnitude(A_mag),.tens_disp(display_A1),.ones_disp(display_A2));
splicer vapourB(.magnitude(B_mag),.tens_disp(display_B1),.ones_disp(display_B2));
splicer vapourC(.magnitude(C_mag),.tens_disp(display_C1),.ones_disp(display_C2));

hex_display Adisplay1(.magnitude(display_A1),.segment(gex_A1));
hex_display Adisplay2(.magnitude(display_A2),.segment(gex_A2));

hex_display Bdisplay1(.magnitude(display_B1),.segment(gex_B1));
hex_display Bdisplay2(.magnitude(display_B2),.segment(gex_B2));

hex_display Cdisplay1(.magnitude(display_C1),.segment(gex_C1));
hex_display Cdisplay2(.magnitude(display_C2),.segment(gex_C2));

endmodule 
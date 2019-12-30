module FIFO(input logic clk,
				input logic button,
				input logic mode,
				input logic [14:0] datain,
				output logic full,
				output logic empty,
				output logic [14:0] dataOut
				/*output logic [6:0] segment4,
				output logic [6:0] segment3,
				output logic [6:0] segment2,
				output logic [6:0] segment1*/);

logic press, press_edge;
logic [3:0] Read_Ptr, Write_Ptr;		
//logic [14:0] dataOut;
logic Wen, Ren;	

sync syncronizer(.clk(clk),.button(button),.fall_edge(press_edge),.in_sync1(press));

deviceFSM FSM(.clk(clk),.button(press_edge),.mode(mode),.Read_Ptr(Read_Ptr),.Write_Ptr(Write_Ptr),.Wen(Wen),.Ren(Ren),.fifo_full(full),.fifo_empty(empty));

memModule memory(.clk(clk),.Wen(Wen),.Ren(Ren),.datain(datain),.wrAddr(Write_Ptr[2:0]),.rdAddr(Read_Ptr[2:0]),.dataOut(dataOut));

/*hex_display display4(.magnitude(dataOut[3:0]),.segment(segment4));

hex_display display3(.magnitude(dataOut[7:4]),.segment(segment3));

hex_display display2(.magnitude(dataOut[11:8]),.segment(segment2));

hex_display display1(.magnitude(dataOut[14:12]),.segment(segment1));*/

endmodule 
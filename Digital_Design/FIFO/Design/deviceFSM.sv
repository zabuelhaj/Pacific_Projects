module deviceFSM( input logic clk, 
						input logic button, 
						input logic mode, 
						output logic [3:0] Read_Ptr=4'd0, 
						output logic [3:0] Write_Ptr=4'd0, 
						output logic Wen,
						output logic Ren,
						output logic fifo_full, 
						output logic fifo_empty);

typedef enum logic [2:0] {IDLE, Write_Mode, Read_Mode} statetype;
statetype state, nextstate;

always_ff @(posedge clk) begin
	state<=nextstate;
end

always_comb begin
	case(state)
		IDLE:
			if(button==1'b1&&mode==1'b0&&fifo_full==1'b1)begin
				nextstate<=IDLE;
			end else if(button==1'b1&&mode==1'b1&&fifo_full==1'b1)begin
				nextstate<=IDLE;
			end else if(button==1'b0&&mode==1'b1&&fifo_full==1'b0)begin
				nextstate<=Write_Mode;
			end else if(button==1'b0&&mode==1'b0&&fifo_empty==1'b0)begin
				nextstate<=Read_Mode;
			end else begin
				nextstate<=IDLE;
			end
		Write_Mode:
			nextstate<=IDLE;
		Read_Mode:
			nextstate<=IDLE;
		default: nextstate<=IDLE;
	endcase
end

always_comb begin
	if(Write_Ptr[3]!=Read_Ptr[3]&&Write_Ptr[2:0]==Read_Ptr[2:0])
		fifo_full<=1'b1;
	else
		fifo_full<=1'b0;
	if(Write_Ptr==Read_Ptr)
		fifo_empty<=1'b1;
	else
		fifo_empty<=1'b0;
end

always_ff @(posedge clk) begin
	if(Wen)
		Write_Ptr<=Write_Ptr+4'd1;
	if(Ren)
		Read_Ptr<=Read_Ptr+4'd1;
end

always_comb begin
	case(state)
		IDLE:
			begin
				Ren<=1'b0;
				Wen<=1'b0;
			end
		Write_Mode:
			begin
				Ren<=1'b0;
				Wen<=1'b1;
			end
		Read_Mode:
			begin
				Ren<=1'b1;
				Wen<=1'b0;
			end
	endcase
end

endmodule

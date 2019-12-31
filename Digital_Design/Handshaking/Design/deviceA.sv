module deviceA(input logic clk, input logic send, input logic reset, input logic devB, output logic devA);

typedef enum logic [3:0] {IDLE, A_Write, A_Done, A_Read} statetype;
statetype state, nextstate;

always_ff @(posedge clk or negedge reset) begin
	if(!reset)
		state<=IDLE;
	else
		state<=nextstate;
end

always_comb begin
	case(state)
		IDLE:
			if(send==1'b0 && devB==1'b0)
				nextstate<=IDLE;
			else if(send==1'b1 && devB==1'b1)
				nextstate<=A_Read;
			else if(send==1'b0 && devB==1'b1)
				nextstate<=A_Read;
			else if(send==1'b1 && devB==1'b0)
				nextstate<=A_Write;
			else
				nextstate<=IDLE;
		A_Read:
			if(send==1'b0 && devB==1'b0)
				nextstate<=IDLE;
			else if(send==1'b1 && devB==1'b0)
				nextstate<=IDLE;
			else if(send==1'b0 && devB==1'b1)
				nextstate<=A_Read;
			else if(send==1'b1 && devB==1'b1)
				nextstate<=A_Read;
			else
				nextstate<=IDLE;
		A_Write:
			if(send==1'b0 && devB==1'b1)
				nextstate<=A_Done;
			else if(send==1'b1 && devB==1'b1)
				nextstate<=A_Done;
			else if(send==1'b0 && devB==1'b0)
				nextstate<=A_Write;
			else if(send==1'b1 && devB==1'b0)
				nextstate<=A_Write;
			else
				nextstate<=IDLE;
		A_Done:
			if(send==1'b0 && devB==1'b0)
				nextstate<=IDLE;
			else if(send==1'b0 && devB==1'b1)
				nextstate<=A_Done;
			else if(send==1'b1 && devB==1'b1)
				nextstate<=A_Done;
			else if(send==1'b1 && devB==1'b0)
				nextstate<=IDLE;
			else
				nextstate<=IDLE;
		default: nextstate<=IDLE;
	endcase
end

always_comb begin
	case(state)
		IDLE:
			devA<=1'b0;
		A_Write:
			devA<=1'b1;
		A_Done:
			devA<=1'b0;
		A_Read:
			devA<=1'b1;
	endcase
end

endmodule

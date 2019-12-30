module Add_Sub_FSM(input logic clk, input logic Add_Sub, output logic A_D);

typedef enum logic [1:0] {SUB, ADD} statetype;
statetype state, nextstate;

always_ff @(posedge clk) begin
	state<=nextstate;
end

always_comb begin
	case(state)
		SUB:
			if(Add_Sub==1'b0)
				nextstate=SUB;
			else if(Add_Sub==1'b1)
				nextstate=ADD;
			else
				nextstate=SUB;
		ADD:
			if(Add_Sub==1'b1)
				nextstate=ADD;
			else if(Add_Sub==1'b0)
				nextstate=SUB;
			else
				nextstate=ADD;
		default: nextstate=SUB;
	endcase
end

always_comb begin
	case(state)
		SUB:
			begin
				A_D<=1'b0;
			end
		ADD:
			begin
				A_D<=1'b1;
			end
		default: A_D<=1'b1;
	endcase
end

endmodule
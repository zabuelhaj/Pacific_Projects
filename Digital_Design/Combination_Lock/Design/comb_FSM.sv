module comb_FSM (input logic clk, input logic [2:0] switch, output logic led);

typedef enum logic [4:0] {LOCKED, B, BC, BCA, BCAB} statetype;
statetype state, nextstate;

always_ff @(posedge clk) begin
	state<=nextstate;
end

always_comb begin
	case(state)
		LOCKED:
			if(switch==3'b101)
				nextstate=B;
			else
				nextstate=LOCKED;
		B:
			if(switch==3'b110)
				nextstate=BC;
			else if(switch==3'b111)
				nextstate=B;
			else
				nextstate=LOCKED;
		BC:
			if(switch==3'b011)
				nextstate=BCA;
			else if(switch==3'b111)
				nextstate=BC;
			else
				nextstate=LOCKED;
		BCA:
			if(switch==3'b101)
				nextstate=BCAB;
			else if(switch==3'b111)
				nextstate=BCA;
			else
				nextstate=LOCKED;
		BCAB:
			if(switch==3'b111)
				nextstate=BCAB;
			else
				nextstate=LOCKED;
		default: nextstate=LOCKED;
	endcase
end

assign led = (state==BCAB);

endmodule
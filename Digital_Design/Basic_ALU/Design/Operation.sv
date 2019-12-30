module Operation( input logic clk, reset,
						input logic [5:0] A,
						input logic [5:0] B,
						input logic [2:0] select, 
						output logic [5:0] returns, 
						output logic OF_LED);

logic [5:0] sum;
logic [5:0] diff;
logic [5:0] great;
logic [5:0] less;
logic [5:0] zip;
logic [5:0] eq;
logic OF_S, OF_D;
						
typedef enum logic [2:0] {IDLE, subt, addr, greater, lesser, zero, equal} statetype;
statetype state, nextstate;

always_ff @(posedge clk or negedge reset) begin
	if(!reset)
		state<=IDLE;
	else
		state<=nextstate;
end

ADD adder(.inA(A),.inB(B),.OF_S(OF_S),.sum(sum));
SUB subtr(.inA(A),.inB(B),.OF_D(OF_D),.diff(diff));
GREATER_THAN gth(.inA(A),.inB(B),.great(great));
LESS_THAN lth(.inA(A),.inB(B),.less(less));
EQUAL_ZERO nada(.inA(A),.inB(B),.zip(zip));
EQUALS same(.inA(A),.inB(B),.eq(eq));

always_comb begin
	case(state)
		IDLE:
			if(select==3'b000)
				nextstate<=addr;
			else if(select==3'b001)
				nextstate<=subt;
			else if(select==3'b100)
				nextstate<=equal;
			else if(select==3'b101)
				nextstate<=greater;
			else if(select==3'b110)
				nextstate<=lesser;
			else if(select==3'b111)
				nextstate<=zero;
			else if(select==3'b010)
				nextstate<=IDLE;
			else if(select==3'b011)
				nextstate<=IDLE;
			else
				nextstate<=IDLE;
		subt:
			if(select==3'b000)
				nextstate<=addr;
			else if(select==3'b001)
				nextstate<=subt;
			else if(select==3'b100)
				nextstate<=equal;
			else if(select==3'b101)
				nextstate<=greater;
			else if(select==3'b110)
				nextstate<=lesser;
			else if(select==3'b111)
				nextstate<=zero;
			else if(select==3'b010)
				nextstate<=IDLE;
			else if(select==3'b011)
				nextstate<=IDLE;
			else
				nextstate<=IDLE;
		addr:
			if(select==3'b000)
				nextstate<=addr;
			else if(select==3'b001)
				nextstate<=subt;
			else if(select==3'b100)
				nextstate<=equal;
			else if(select==3'b101)
				nextstate<=greater;
			else if(select==3'b110)
				nextstate<=lesser;
			else if(select==3'b111)
				nextstate<=zero;
			else if(select==3'b010)
				nextstate<=IDLE;
			else if(select==3'b011)
				nextstate<=IDLE;
			else
				nextstate<=IDLE;
		equal:
			if(select==3'b000)
				nextstate<=addr;
			else if(select==3'b001)
				nextstate<=subt;
			else if(select==3'b100)
				nextstate<=equal;
			else if(select==3'b101)
				nextstate<=greater;
			else if(select==3'b110)
				nextstate<=lesser;
			else if(select==3'b111)
				nextstate<=zero;
			else if(select==3'b010)
				nextstate<=IDLE;
			else if(select==3'b011)
				nextstate<=IDLE;
			else
				nextstate<=IDLE;
		greater:
			if(select==3'b000)
				nextstate<=addr;
			else if(select==3'b001)
				nextstate<=subt;
			else if(select==3'b100)
				nextstate<=equal;
			else if(select==3'b101)
				nextstate<=greater;
			else if(select==3'b110)
				nextstate<=lesser;
			else if(select==3'b111)
				nextstate<=zero;
			else if(select==3'b010)
				nextstate<=IDLE;
			else if(select==3'b011)
				nextstate<=IDLE;
			else
				nextstate<=IDLE;
		lesser:
			if(select==3'b000)
				nextstate<=addr;
			else if(select==3'b001)
				nextstate<=subt;
			else if(select==3'b100)
				nextstate<=equal;
			else if(select==3'b101)
				nextstate<=greater;
			else if(select==3'b110)
				nextstate<=lesser;
			else if(select==3'b111)
				nextstate<=zero;
			else if(select==3'b010)
				nextstate<=IDLE;
			else if(select==3'b011)
				nextstate<=IDLE;
			else
				nextstate<=IDLE;
		zero:
			if(select==3'b000)
				nextstate<=addr;
			else if(select==3'b001)
				nextstate<=subt;
			else if(select==3'b100)
				nextstate<=equal;
			else if(select==3'b101)
				nextstate<=greater;
			else if(select==3'b110)
				nextstate<=lesser;
			else if(select==3'b111)
				nextstate<=zero;
			else if(select==3'b010)
				nextstate<=IDLE;
			else if(select==3'b011)
				nextstate<=IDLE;
			else
				nextstate<=IDLE;
		default: nextstate<=IDLE;
	endcase
end

always_comb begin
	case(state)
		IDLE:
			begin
				returns=6'd0;
				OF_LED=1'b0;
			end
		subt:
			begin
				returns=diff;
				OF_LED=OF_D;
			end
		addr:
			begin
				returns=sum;
				OF_LED=OF_S;
			end
		equal:
			begin
				returns=eq;
				OF_LED=1'b0;
			end
		greater:
			begin
				returns=great;
				OF_LED=1'b0;
			end
		lesser:
			begin
				returns=less;
				OF_LED=1'b0;
			end
		zero:
			begin
				returns=zip;
				OF_LED=1'b0;
			end
		default: 
			begin 
				returns=6'd0;
				OF_LED=1'b0;
			end
	endcase
end

endmodule 
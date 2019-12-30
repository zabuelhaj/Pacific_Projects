module LED_FSM(input logic clk, input logic [2:0] Input, output logic [3:0] led);
					
/*typedef enum logic [7:0] {All_Off, All_On, Chase_1, Chase_2, Chase_3, Chase_4, Alt_Flash_1, Alt_Flash_2} statetype;
statetype state, nextstate;*/

typedef enum logic [5:0] {All_Off, All_On, Chase_1, Chase_2, Chase_3, Alt_Flash} statetype;
statetype state, nextstate;

always_ff @(posedge clk)
begin
	state<=nextstate;
end

/*always_comb
begin
	case(state)
		All_Off:
			if(Input==3'b001)
				nextstate=All_On;
			else if(Input==3'b011)
				nextstate=Chase_1;
			else if(Input==3'b101)
				nextstate=Alt_Flash_1;
			else if(Input==3'b111)
				nextstate=All_On;
			else
				nextstate=All_Off;
		All_On:
			if(Input==3'b001)
				nextstate=All_Off;
			else if(Input==3'b011)
				nextstate=Chase_1;
			else if(Input==3'b101)
				nextstate=Alt_Flash_1;
			else if(Input==3'b111)
				nextstate=All_On;
			else
				nextstate=All_Off;
		Chase_1:
			if(Input==3'b001)
				nextstate=All_Off;
			else if(Input==3'b011)
				nextstate=Chase_2;
			else if(Input==3'b101)
				nextstate=Alt_Flash_1;
			else if(Input==3'b111)
				nextstate=All_On;
			else
				nextstate=All_Off;
		Chase_2:
			if(Input==3'b001)
				nextstate=All_Off;
			else if(Input==3'b011)
				nextstate=Chase_3;
			else if(Input==3'b101)
				nextstate=Alt_Flash_1;
			else if(Input==3'b111)
				nextstate=All_On;
			else
				nextstate=All_Off;
		Chase_3:
			if(Input==3'b001)
				nextstate=All_Off;
			else if(Input==3'b011)
				nextstate=Chase_4;
			else if(Input==3'b101)
				nextstate=Alt_Flash_1;
			else if(Input==3'b111)
				nextstate=All_On;
			else
				nextstate=All_Off;
		Chase_4:
			if(Input==3'b001)
				nextstate=All_Off;
			else if(Input==3'b011)
				nextstate=All_Off;
			else if(Input==3'b101)
				nextstate=Alt_Flash_1;
			else if(Input==3'b111)
				nextstate=All_On;
			else
				nextstate=All_Off;
		Alt_Flash_1:
			if(Input==3'b001)
				nextstate=All_Off;
			else if(Input==3'b011)
				nextstate=Chase_1;
			else if(Input==3'b101)
				nextstate=Alt_Flash_2;
			else if(Input==3'b111)
				nextstate=All_On;
			else
				nextstate=All_Off;
		Alt_Flash_2:
			if(Input==3'b001)
				nextstate=All_Off;
			else if(Input==3'b011)
				nextstate=Chase_1;
			else if(Input==3'b101)
				nextstate=Alt_Flash_1;
			else if(Input==3'b111)
				nextstate=All_On;
			else
				nextstate=All_Off;
		default: nextstate=All_Off;
	endcase
end

always_comb
begin
	case(state)
		All_Off:
			led<=4'b0000;
		All_On:
			led<=4'b1111;
		Chase_1:
			led<=4'b0001;
		Chase_2:
			led<=4'b0010;
		Chase_3:
			led<=4'b0100;
		Chase_4:
			led<=4'b1000;
		Alt_Flash_1:
			led<=4'b0101;
		Alt_Flash_2:
			led<=4'b1010;
	endcase
end*/

always_comb
begin
	case(state)
		All_Off:
			if(Input==3'b001)begin
				nextstate=All_On;
				led<=4'b1111;
				end
			else if(Input==3'b011)begin
				nextstate=Chase_1;
				led<=4'b0001;
				end
			else if(Input==3'b101)begin
				nextstate=Alt_Flash;
				led<=4'b0101;
				end
			else if(Input==3'b111)begin
				nextstate=All_On;
				led<=4'b1111;
				end
			else begin
				nextstate=All_Off;
				led<=4'b0000;
				end
		All_On:
			if(Input==3'b001)begin
				nextstate=All_Off;
				led<=4'b0000;
				end
			else if(Input==3'b011)begin
				nextstate=Chase_1;
				led<=4'b0001;
				end
			else if(Input==3'b101)begin
				nextstate=Alt_Flash;
				led<=4'b0101;
				end
			else if(Input==3'b111)begin
				nextstate=All_On;
				led<=4'b1111;
				end
			else begin
				nextstate=All_Off;
				led<=4'b0000;
				end
		Chase_1:
			if(Input==3'b001)begin
				nextstate=All_Off;
				led<=4'b0000;
				end
			else if(Input==3'b011)begin
				nextstate=Chase_2;
				led<=4'b0010;
				end
			else if(Input==3'b101)begin
				nextstate=Alt_Flash;
				led<=4'b0101;
				end
			else if(Input==3'b111)begin
				nextstate=All_On;
				led<=4'b1111;
				end
			else begin
				nextstate=All_Off;
				led<=4'b0000;
				end
		Chase_2:
			if(Input==3'b001)begin
				nextstate=All_Off;
				led<=4'b0000;
				end
			else if(Input==3'b011)begin
				nextstate=Chase_3;
				led<=4'b0100;
				end
			else if(Input==3'b101)begin
				nextstate=Alt_Flash;
				led<=4'b0101;
				end
			else if(Input==3'b111)begin
				nextstate=All_On;
				led<=4'b1111;
				end
			else begin
				nextstate=All_Off;
				led<=4'b0000;
				end
		Chase_3:
			if(Input==3'b001)begin
				nextstate=All_Off;
				led<=4'b0000;
				end
			else if(Input==3'b011)begin
				nextstate=All_Off;
				led<=4'b1000;
				end
			else if(Input==3'b101)begin
				nextstate=Alt_Flash;
				led<=4'b0101;
				end
			else if(Input==3'b111)begin
				nextstate=All_On;
				led<=4'b1111;
				end
			else begin
				nextstate=All_Off;
				led<=4'b0000;
				end
		Alt_Flash:
			if(Input==3'b001)begin
				nextstate=All_Off;
				led<=4'b0000;
				end
			else if(Input==3'b011)begin
				nextstate=Chase_1;
				led<=4'b0001;
				end
			else if(Input==3'b101)begin
				nextstate=All_Off;
				led<=4'b1010;
				end
			else if(Input==3'b111)begin
				nextstate=All_On;
				led<=4'b1111;
				end
			else begin
				nextstate=All_Off;
				led<=4'b0000;
				end
		default: nextstate=All_Off;
	endcase
end
				
endmodule
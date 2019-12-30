module hex_display(input logic [3:0] magnitude, output logic [6:0] segment);

always_comb begin
	case(magnitude)
	4'd0 : segment = 7'b1000000; 		//to display 0
   4'd1 : segment = 7'b1111001;		//to display 1
   4'd2 : segment = 7'b0100100;		//to display 2
   4'd3 : segment = 7'b0110000;	 	//to display 3
   4'd4 : segment = 7'b0011001; 		//to display 4
   4'd5 : segment = 7'b0010010; 		//to display 5
   4'd6 : segment = 7'b0000010; 		//to display 6
   4'd7 : segment = 7'b1111000;		//to display 7
   4'd8 : segment = 7'b0000000;		//to display 8
   4'd9 : segment = 7'b0010000;		//to display 9
	4'ha : segment = 7'b0001000;		//to display A
	4'hb : segment = 7'b0000011;		//to display b
	4'hc : segment = 7'b1000110;		//to display C
	4'hd : segment = 7'b0100001;		//to display d
	4'he : segment = 7'b0000110;		//to display E
	4'hf : segment = 7'b0001110;		//to display F
   default : segment = 7'b1000000; 	//zero
  endcase
end

endmodule

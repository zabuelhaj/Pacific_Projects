module hex_display(input logic [3:0] magnitude, output logic [6:0] segment);

always_comb begin
	case(magnitude)
		4'd0:segment=7'b011_1111;
		4'd1:segment=7'b000_0011;
		4'd2:segment=7'b101_1101;
		4'd3:segment=7'b100_1111;
		4'd4:segment=7'b110_0110;
		4'd5:segment=7'b110_1101;
		4'd6:segment=7'b111_1101;
		4'd7:segment=7'b000_0111;
		4'd8:segment=7'b111_1111;
		4'd9:segment=7'b110_1111;
		default: segment=7'b000_0000;
	endcase
end

endmodule
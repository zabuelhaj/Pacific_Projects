module splicer(input logic [5:0] magnitude, 
					output logic [3:0] tens_disp,
					output logic [3:0] ones_disp);

always_comb
begin
	tens_disp <= magnitude / 4'd10;
	ones_disp <= magnitude % 4'd10;
end
endmodule

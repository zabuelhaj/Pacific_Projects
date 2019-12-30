module sign_mag(input logic [5:0] value, output logic [5:0] mag, output logic sign);

always_comb begin
	if(value[5]==1'b1) begin
		sign<=1'b1;
		mag<=(~value+6'b1);
		end
	else begin
		sign<=1'b0;
		mag<=value;
		end
end

endmodule

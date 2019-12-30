module memModule( input logic clk, Wen, Ren,
						input logic [14:0] datain, 
						input logic [2:0] wrAddr, rdAddr, 
						output logic [14:0] dataOut);

logic [14:0] memory [7:0];

always_ff@(posedge clk)
begin

	if(Wen)
		memory[wrAddr]<=datain;
	if(Ren)
		dataOut<=memory[rdAddr];

end

initial
begin
	for(int i=0; i<8;i++)
		memory[i]<=15'd0;
end

endmodule 
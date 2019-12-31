module Handshaking(input  logic clk, send, reset, devB, output logic devA, LED_devB, LED_send, LED_devA);

logic in_send;
logic in_reset;
logic in_devB;
logic edge_send;
logic edge_devB;
logic outclk;

clockdiv clockdiv(.iclk(clk),.oclk(outclk));

sync syncronizer1(.clk(outclk),.button(send),.in_sync(in_send),.rise_edge(edge_send));
resetSync syncronizer2(.clk(outclk),.reset(reset),.resetsync(in_reset));
sync syncronizer3(.clk(outclk),.button(devB),.in_sync(in_devB),.rise_edge(edge_devB));

deviceA deviceA(.clk(outclk),.send(~in_send),.reset(in_reset),.devB(in_devB),.devA(devA));

always_comb begin
	LED_send<=~in_send;
	LED_devB<=in_devB;
	LED_devA<=devA;
end

endmodule 
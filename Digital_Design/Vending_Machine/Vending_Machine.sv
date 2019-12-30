module Vending_Machine(input logic clk, input logic [2:0] in, output logic [3:0] out);

logic outclk;
clockdiv clockdiv(.iclk(clk),.oclk(outclk));

LED_FSM LED_FSM(.Input(in),.led(out),.clk(outclk));

endmodule
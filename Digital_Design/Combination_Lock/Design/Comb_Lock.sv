module Comb_Lock (input logic clk,input logic [2:0] comb,output logic unlock);

logic [2:0] press;

sync syncronizer1(.clk(clk),.button(comb[0]),.fall_edge(press[0]));
sync syncronizer2(.clk(clk),.button(comb[1]),.fall_edge(press[1]));
sync syncronizer3(.clk(clk),.button(comb[2]),.fall_edge(press[2]));

comb_FSM machine(.clk(clk),.switch(press),.led(unlock));

endmodule
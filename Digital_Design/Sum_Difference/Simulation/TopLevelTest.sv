module TopLevelTest();
	logic clk=1'b0;
	logic [5:0] A;
	logic [5:0] B;
	logic Add_Sub;
	logic button;
	logic [6:0] hex_A1; 
	logic [6:0] hex_A2;
	logic [6:0] hex_B1;
	logic [6:0] hex_B2;
	logic [6:0] hex_Answer1;
	logic [6:0] hex_Answer2;
	logic OF_LED;
	logic A_LED;
	logic B_LED;
	logic answer_LED;
	logic [5:0]A_val[0:7]={6'd15,6'd1,6'd14,6'd3,6'd32,6'd64,6'd6,6'd7};
	logic [5:0]B_val[0:7]={6'd8,6'd9,6'd10,6'd11,6'd32,6'd13,6'd14,6'd15};
	logic [5:0]button_val[0:7]={1'b1,1'b1,1'b1,1'b0,1'b0,1'b0,1'b0,1'b0};
	logic [5:0]addsub_val[0:7]={1'b0,1'b0,1'b1,1'b0,1'b0,1'b1,1'b1,1'b0};

	Add_Sub_Top top(.clk(clk),.A(A),.B(B),.Add_Sub(Add_Sub),.button(button),.hex_A1(hex_A1),.hex_A2(hex_A2),.hex_B1(hex_B1),.hex_B2(hex_B2),.hex_Answer1(hex_Answer1),.hex_Answer2(hex_Answer2),.OF_LED(OF_LED),.A_LED(A_LED),.B_LED(B_LED),.answer_LED(answer_LED));

	always #50 clk<=~clk;

	task testCase(input logic [5:0] invalA, input logic [5:0] invalB, input logic invalbutton, input logic invaladdsub);
		@(posedge clk);
		A<=invalA;
		B<=invalB;
		button<=invalbutton;
		Add_Sub<=invaladdsub;
	endtask

	initial
	begin
		for(int i=0;i<10;i++) begin
			testCase(A_val[i],B_val[i],button_val[i],addsub_val[i]);
			$display("A: %d B: %d ADD or SUB: %d", A_val[i], B_val[i], addsub_val[i]);
		end
		$stop;
	end
endmodule
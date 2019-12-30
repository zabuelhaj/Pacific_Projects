if {[file exists rtl_work]} {
	vdel -lib rtl_work -all
}
vlib rtl_work
vmap work rtl_work

### ---------------------------------------------- ###
### Compile code ###
### Enter files here; copy line for multiple files ###
vlog -sv -work work [pwd]/TopLevelTest.sv
vlog -sv -work work [pwd]/Add_Sub_Top.sv
vlog -sv -work work [pwd]/ADD.sv
vlog -sv -work work [pwd]/SUB.sv
vlog -sv -work work [pwd]/MUX.sv
vlog -sv -work work [pwd]/Add_Sub_FSM.sv
vlog -sv -work work [pwd]/sign_mag.sv
vlog -sv -work work [pwd]/hex_display.sv
vlog -sv -work work [pwd]/sync.sv
vlog -sv -work work [pwd]/splicer.sv

### ---------------------------------------------- ###
### Load design for simulation ###
### Replace topLevelModule with the name of your top level module (no .sv) ###
### Do not duplicate! ###
vsim TopLevelTest

### ---------------------------------------------- ###
### Add waves here ###
### Use add wave * to see all signals ###
add wave *

### Force waves here ###

### ---------------------------------------------- ###
### Run simulation ###
### Do not modify ###
# to see your design hierarchy and signals 
view structure 

# to see all signal names and current values
view signals 

### ---------------------------------------------- ###
### Edit run time ###
run 10 ns     

### ---------------------------------------------- ###
### Will create large wave window and zoom to show all signals
view -undock wave
wave zoomfull 

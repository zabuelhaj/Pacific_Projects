if {[file exists rtl_work]} {
	vdel -lib rtl_work -all
}
vlib rtl_work
vmap work rtl_work

### ---------------------------------------------- ###
### Compile code ###
### Enter files here; copy line for multiple files ###
vlog -sv -work work [pwd]/toplevelTestbench.sv
vlog -sv -work work [pwd]/ALU.sv
vlog -sv -work work [pwd]/ADD.sv
vlog -sv -work work [pwd]/SUB.sv
vlog -sv -work work [pwd]/splicer.sv
vlog -sv -work work [pwd]/hex_display.sv
vlog -sv -work work [pwd]/sync.sv
vlog -sv -work work -suppress 7061 [pwd]/memModule.sv
vlog -sv -work work [pwd]/resetSync.sv
vlog -sv -work work [pwd]/ALU_FSM.sv
vlog -sv -work work [pwd]/Operation.sv
vlog -sv -work work [pwd]/GREATER_THAN.sv
vlog -sv -work work [pwd]/LESS_THAN.sv
vlog -sv -work work [pwd]/EQUAL_ZERO.sv
vlog -sv -work work [pwd]/EQUALS.sv
vlog -sv -work work [pwd]/sign_mag.sv

### ---------------------------------------------- ###
### Load design for simulation ###
### Replace topLevelModule with the name of your top level module (no .sv) ###
### Do not duplicate! ###
vsim toplevelTestbench

### ---------------------------------------------- ###
### Add waves here ###
### Use add wave * to see all signals ###
add wave *
add wave mrSkeletal/*

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
run 1000 ns     

### ---------------------------------------------- ###
### Will create large wave window and zoom to show all signals
view -undock wave
wave zoomfull 

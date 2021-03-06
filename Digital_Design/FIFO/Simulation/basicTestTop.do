if {[file exists rtl_work]} {
	vdel -lib rtl_work -all
}
vlib rtl_work
vmap work rtl_work

### ---------------------------------------------- ###
### Compile code ###
### Enter files here; copy line for multiple files ###
vlog -sv -work work [pwd]/TopLevelTestbench.sv
vlog -sv -work work [pwd]/FIFO.sv
vlog -sv -work work -suppress 7061 [pwd]/memModule.sv
vlog -sv -work work [pwd]/sync.sv
vlog -sv -work work [pwd]/deviceFSM.sv
vlog -sv -work work [pwd]/hex_display.sv

### ---------------------------------------------- ###
### Load design for simulation ###
### Replace topLevelModule with the name of your top level module (no .sv) ###
### Do not duplicate! ###
vsim TopLevelTestbench

### ---------------------------------------------- ###
### Add waves here ###
### Use add wave * to see all signals ###
add wave *
add wave DUT/*

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
run 10000 ns     

### ---------------------------------------------- ###
### Will create large wave window and zoom to show all signals
view -undock wave
wave zoomfull 

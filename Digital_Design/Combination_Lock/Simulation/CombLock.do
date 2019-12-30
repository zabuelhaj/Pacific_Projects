if {[file exists rtl_work]} {
	vdel -lib rtl_work -all
}
vlib rtl_work
vmap work rtl_work

### ---------------------------------------------- ###
### Compile code ###
### Enter files here; copy line for multiple files ###
vlog -sv -work work [pwd]/lab3TestBench.sv
vlog -sv -work work [pwd]/sync.sv
vlog -sv -work work [pwd]/comb_FSM.sv
vlog -sv -work work [pwd]/Comb_Lock.sv

### ---------------------------------------------- ###
### Load design for simulation ###
### Replace topLevelModule with the name of your top level module (no .sv) ###
### Do not duplicate! ###
vsim lab3TestBench

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

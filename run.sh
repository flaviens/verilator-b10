# Verilator
rm -rf obj_dir_noflag && mkdir -p obj_dir_noflag
verilator --cc --exe -Wno-MULTIDRIVEN --Wno-UNOPTFLAT -Wno-WIDTHTRUNC --build tb_base.cc top.sv -CFLAGS '-I.. -g' --Mdir obj_dir_noflag --build-jobs 32

rm -rf obj_dir_withflag && mkdir -p obj_dir_withflag
verilator --cc -fno-expand --exe -Wno-MULTIDRIVEN --Wno-UNOPTFLAT -Wno-WIDTHTRUNC --build tb_base.cc top.sv -CFLAGS '-I.. -g' --Mdir obj_dir_withflag --build-jobs 32

echo "Noflag"
TRACEFILE=trace.vcd ./obj_dir_noflag/Vtop
echo "-fno-expand"
TRACEFILE=trace.vcd ./obj_dir_withflag/Vtop

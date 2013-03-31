@echo Need to compile these manually

@"%SBS_GCCE463BIN%/arm-none-symbianelf-as.exe" port/sdl/div.S -march=armv6 -o port/sdl/div.o
@echo compiled [div.o]
@"%SBS_GCCE463BIN%/arm-none-symbianelf-as.exe" gpu/gpu_unai/gpu_arm.s -march=armv6 -o gpu/gpu_unai/gpu_arm.o
@echo compiled [gpu_arm.o]
@"%SBS_GCCE463BIN%/arm-none-symbianelf-as.exe" EmiPSX/relocator/relocator_glue.s -march=armv6 -o EmiPSX/relocator/relocator_glue.o
@echo compiled [relocator_glue.o]
@"%SBS_GCCE463BIN%/arm-none-symbianelf-as.exe" recompiler/arm/run.s -march=armv6 -o recompiler/arm/run.o
@echo compiled [run.o]
@@cp port/sdl/div.o gpu/gpu_unai/gpu_arm.o EmiPSX/relocator/relocator_glue.o recompiler/arm/run.o %EPOCROOT%/epoc32/release/armv5/urel/
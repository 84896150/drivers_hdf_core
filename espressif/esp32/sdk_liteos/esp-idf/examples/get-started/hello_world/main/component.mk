#
# "main" pseudo-component makefile.
#
# (Uses default behaviour of compiling all source files in directory, adding 'include' to include path.)
COMPONENT_ADD_INCLUDEDIRS += ../../../../../../../../../kernel/liteos_m/utils 
                             ../../../../../../../../../kernel/liteos_m/kal/cmsis
							 ../../../../../../../../../kernel/liteos_m/kernel/include
							 ../../../../../../../../../kernel/liteos_m/utils
							 ../../../../../../../../../kernel/liteos_m/kernel/arch/risc-v/riscv32/gcc
							 ../../../../../../../../../kernel/liteos_m/kernel/arch/risc-v/riscv32/gcc/asm
							 ../../../../../../../../../kernel/liteos_m/targets/esp32
							 ../../../../../../../../../kernel/liteos_m/targets/esp32/include
							 ../../../../../../../../../kernel/liteos_m/targets/esp32/include/asm
							 ../../../../../../../../../kernel/liteos_m/kal/cmsis
							 ../../../../../../../../../utils/native/lite/include
							 ../../../../../../../../../kernel/liteos_m/kernel/arch/include
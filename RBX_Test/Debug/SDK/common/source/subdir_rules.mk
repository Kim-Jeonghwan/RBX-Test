################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
SDK/common/source/%.obj: ../SDK/common/source/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccs2040/ccs/tools/compiler/ti-cgt-c2000_22.6.3.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla2 --float_support=fpu64 --idiv_support=idiv0 --tmu_support=tmu0 --vcu_support=vcrc -O2 --opt_for_speed=5 --fp_mode=relaxed --fp_reassoc=on --include_path="D:/Nexcom/Firmware/01_Project/01_RBX/RBX_Test/RBX_Test/RBX_Test" --include_path="C:/ti/ccs2040/ccs/tools/compiler/ti-cgt-c2000_22.6.3.LTS/include" --include_path="D:/Nexcom/Firmware/01_Project/01_RBX/RBX_Test/RBX_Test/RBX_Test/SDK/common/include" --include_path="D:/Nexcom/Firmware/01_Project/01_RBX/RBX_Test/RBX_Test/RBX_Test/SDK/headers/include" --include_path="D:/Nexcom/Firmware/01_Project/01_RBX/RBX_Test/RBX_Test/RBX_Test/SDK/driverlib" --include_path="D:/Nexcom/Firmware/01_Project/01_RBX/RBX_Test/RBX_Test/RBX_Test/SDK/driverlib/inc" --include_path="D:/Nexcom/Firmware/01_Project/01_RBX/RBX_Test/RBX_Test/RBX_Test/Dev" --include_path="D:/Nexcom/Firmware/01_Project/01_RBX/RBX_Test/RBX_Test/RBX_Test/CSU" --include_path="D:/Nexcom/Firmware/01_Project/01_RBX/RBX_Test/RBX_Test/RBX_Test/targetConfigs" --advice:performance=all --define=CPU1 --define=_FLASH --define=USE_20MHZ_XTAL --define=_DUAL_HEADERS --define=DONT_DECLARE_IFR_IER -g --diag_warning=225 --diag_wrap=off --display_error_number --gen_func_subsections=on --abi=eabi --preproc_with_compile --preproc_dependency="SDK/common/source/$(basename $(<F)).d_raw" --obj_directory="SDK/common/source" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

SDK/common/source/%.obj: ../SDK/common/source/%.asm $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccs2040/ccs/tools/compiler/ti-cgt-c2000_22.6.3.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla2 --float_support=fpu64 --idiv_support=idiv0 --tmu_support=tmu0 --vcu_support=vcrc -O2 --opt_for_speed=5 --fp_mode=relaxed --fp_reassoc=on --include_path="D:/Nexcom/Firmware/01_Project/01_RBX/RBX_Test/RBX_Test/RBX_Test" --include_path="C:/ti/ccs2040/ccs/tools/compiler/ti-cgt-c2000_22.6.3.LTS/include" --include_path="D:/Nexcom/Firmware/01_Project/01_RBX/RBX_Test/RBX_Test/RBX_Test/SDK/common/include" --include_path="D:/Nexcom/Firmware/01_Project/01_RBX/RBX_Test/RBX_Test/RBX_Test/SDK/headers/include" --include_path="D:/Nexcom/Firmware/01_Project/01_RBX/RBX_Test/RBX_Test/RBX_Test/SDK/driverlib" --include_path="D:/Nexcom/Firmware/01_Project/01_RBX/RBX_Test/RBX_Test/RBX_Test/SDK/driverlib/inc" --include_path="D:/Nexcom/Firmware/01_Project/01_RBX/RBX_Test/RBX_Test/RBX_Test/Dev" --include_path="D:/Nexcom/Firmware/01_Project/01_RBX/RBX_Test/RBX_Test/RBX_Test/CSU" --include_path="D:/Nexcom/Firmware/01_Project/01_RBX/RBX_Test/RBX_Test/RBX_Test/targetConfigs" --advice:performance=all --define=CPU1 --define=_FLASH --define=USE_20MHZ_XTAL --define=_DUAL_HEADERS --define=DONT_DECLARE_IFR_IER -g --diag_warning=225 --diag_wrap=off --display_error_number --gen_func_subsections=on --abi=eabi --preproc_with_compile --preproc_dependency="SDK/common/source/$(basename $(<F)).d_raw" --obj_directory="SDK/common/source" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '



################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
targetConfigs/%.obj: ../targetConfigs/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccs2040/ti-cgt-c2000_22.6.1.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla2 --float_support=fpu64 --idiv_support=idiv0 --tmu_support=tmu0 --vcu_support=vcrc -O2 --opt_for_speed=5 --fp_mode=relaxed --fp_reassoc=on --include_path="D:/Nexcom/Firmware/RBX_Test/Test01/Test01" --include_path="C:/ti/ccs2040/ti-cgt-c2000_22.6.1.LTS/include" --include_path="D:/Nexcom/Firmware/RBX_Test/Test01/Test01/SDK/common/include" --include_path="D:/Nexcom/Firmware/RBX_Test/Test01/Test01/SDK/headers/include" --include_path="D:/Nexcom/Firmware/RBX_Test/Test01/Test01/SDK/driverlib" --include_path="D:/Nexcom/Firmware/RBX_Test/Test01/Test01/SDK/driverlib/inc" --include_path="D:/Nexcom/Firmware/RBX_Test/Test01/Test01/Dev" --include_path="D:/Nexcom/Firmware/RBX_Test/Test01/Test01/CSU" --include_path="D:/Nexcom/Firmware/RBX_Test/Test01/Test01/targetConfigs" --advice:performance=all --define=CPU1 --define=_FLASH --define=USE_20MHZ_XTAL --define=_DUAL_HEADERS -g --diag_warning=225 --diag_wrap=off --display_error_number --gen_func_subsections=on --abi=eabi --preproc_with_compile --preproc_dependency="targetConfigs/$(basename $(<F)).d_raw" --obj_directory="targetConfigs" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '



################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
LcdDrivermsp430/%.obj: ../LcdDrivermsp430/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ti/ccs1110/ccs/tools/compiler/ti-cgt-msp430_21.6.0.LTS/bin/cl430" -vmspx --data_model=restricted --use_hw_mpy=F5 --include_path="C:/ti/ccs1110/ccs/ccs_base/msp430/include" --include_path="C:/Users/think/workspace_v11/project" --include_path="C:/Users/think/workspace_v11/project/driverlib/MSP430FR5xx_6xx" --include_path="C:/Users/think/workspace_v11/project/GrLib/grlib" --include_path="C:/Users/think/workspace_v11/project/GrLib/fonts" --include_path="C:/ti/ccs1110/ccs/tools/compiler/ti-cgt-msp430_21.6.0.LTS/include" --advice:hw_config=all --define=DEPRECATED --define=__MSP430FR5994__ -g --printf_support=full --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --preproc_with_compile --preproc_dependency="LcdDrivermsp430/$(basename $(<F)).d_raw" --obj_directory="LcdDrivermsp430" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '



################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
driverlib/MSP430F5xx_6xx/%.obj: ../driverlib/MSP430F5xx_6xx/%.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ti/ccs901/ccs/tools/compiler/ti-cgt-msp430_18.12.1.LTS/bin/cl430" -vmspx --data_model=restricted --use_hw_mpy=F5 --include_path="C:/ti/ccs901/ccs/ccs_base/msp430/include" --include_path="C:/Users/regin/Google Drive/eel4746_labs_online/ccs_fa19/home_versions/LAB8_Solution" --include_path="C:/Users/regin/Google Drive/eel4746_labs_online/ccs_fa19/home_versions/LAB8_Solution/driverlib/MSP430F5xx_6xx" --include_path="C:/Users/regin/Google Drive/eel4746_labs_online/ccs_fa19/home_versions/LAB8_Solution/GrLib/grlib" --include_path="C:/Users/regin/Google Drive/eel4746_labs_online/ccs_fa19/home_versions/LAB8_Solution/GrLib/fonts" --include_path="C:/ti/ccs901/ccs/tools/compiler/ti-cgt-msp430_18.12.1.LTS/include" --advice:power="none" --define=__MSP430F5529__ --define=DEPRECATED -g --printf_support=nofloat --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU23 --silicon_errata=CPU40 --preproc_with_compile --preproc_dependency="driverlib/MSP430F5xx_6xx/$(basename $(<F)).d_raw" --obj_directory="driverlib/MSP430F5xx_6xx" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '



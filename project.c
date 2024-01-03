#include "LcdDrivermsp430/Crystalfontz128x128_ST7735.h"
#include "LcdDrivermsp430/HAL_MSP_EXP430FR5994_Crystalfontz128x128_ST7735.h"
#include "grlib.h"
#include "driverlib.h"
#include <stdint.h>
#include <stdio.h>

//initialize functions
void configIO(void);
void configTimerA(uint16_t);
void myMotorController(void);
void joyStick_init(void);
void ADC_init(void);
void LCD_init(void);
void motorDriverPattern();
void RotateBladeStep1(void);
void RotateBladeStep2(void);
void SpinMotorCW(uint8_t); //Only even integer
void SpinMotorCCW(uint8_t); //Only even integer
void RotateBladeStepMid(void);
void updateStick(void);
void blinkLED(void);

//Define Structure for up parameters
Graphics_Context g_sContext;
uint8_t pbs1, pbs2, patternCount;
Timer_A_initUpModeParam MyTimerA;
uint16_t joyStickX, joyStickY, delayValue = 0;
const uint8_t stepArray[] = {68, 50, 16, 58, 24, 58, 50, 50, 76, 50}; //Only even integer

//*********************************************************************************************************
//
//                                   Main Function: main()
//        Narrative: This function serves as the overall control system of the project. We first configure
//                   the functionalities of the project (ADC, LCD, I/O, Joystick). After, we then find
//                   the current state of the system (joysticks and pushbuttons) and send them to the
//                   appropriate function. We also configure the timer A delay depending on which
//                   state is active and drive the motor using the timer A interrupt
//
//      Primary Author: Zachary Jones
//      Secondary Author: Kennyth Kouch
//
//*********************************************************************************************************
void main()
{

    //Stop the Watchdog Timer
    WDT_A_hold(WDT_A_BASE);
    char buffer[100];

    //ConfigIO
    configIO();

    // Initialize Joystick
    joyStick_init();

    // Initialize ADC
    ADC_init();

    // Initialize LCD
    LCD_init();

    configTimerA(5000);

    while(1)
    {
        updateStick();

        //  Write to the LCD screen
        sprintf(buffer, "JoyStick X 0x%.3x", joyStickX);
        Graphics_drawStringCentered(&g_sContext, (int8_t*) buffer, AUTO_STRING_LENGTH, 64, 40,OPAQUE_TEXT);
        sprintf(buffer, "JoyStick Y 0x%.3x", joyStickY);
        Graphics_drawStringCentered(&g_sContext, (int8_t*) buffer,AUTO_STRING_LENGTH, 64, 50,OPAQUE_TEXT);

        //Get Status of mode operations
        pbs1 = GPIO_getInputPinValue(GPIO_PORT_P4, GPIO_PIN3);
        pbs2 = GPIO_getInputPinValue(GPIO_PORT_P4, GPIO_PIN2);

        if (pbs1 == 0 && pbs2 == 0)
        {
            motorDriverPattern(); //Call function to begin pattern mode
        }
        else if (pbs1 == 0 && pbs2 == 1)
        {
            configTimerA(469);
            while(pbs1 == 0 && pbs2 == 1)
            {
                SpinMotorCW(10); //Only even integer
                pbs1 = GPIO_getInputPinValue(GPIO_PORT_P4, GPIO_PIN3);
                pbs2 = GPIO_getInputPinValue(GPIO_PORT_P4, GPIO_PIN2);
            }
        }
        else if (pbs1 == 1 && pbs2 == 0)
        {
            configTimerA(938);
            while(pbs1 == 1 && pbs2 == 0)
            {
                SpinMotorCCW(10); //Only even integer
                pbs1 = GPIO_getInputPinValue(GPIO_PORT_P4, GPIO_PIN3);
                pbs2 = GPIO_getInputPinValue(GPIO_PORT_P4, GPIO_PIN2);
            }
        }
        else
        {
            myMotorController();
        }
        Timer_A_enableInterrupt(TIMER_A0_BASE);
        Timer_A_startCounter(TIMER_A0_BASE,TIMER_A_UP_MODE);
        __enable_interrupt();
    }
}

//***************************************************************************
//
//                    ISR
//              Joystick controls gets called
//       no inputs or returns
// Primary Author: Kennyth Kouch
// Secondary Author: Zachary Jones
//
//***************************************************************************
#pragma vector = TIMER0_A1_VECTOR
__interrupt void motorISR()
{
    myMotorController();
    Timer_A_clearTimerInterrupt(TIMER_A0_BASE);
}

//***************************************************************************
//
//                    myMotorController()
//       Controls motor using joystick input
//       no inputs or returns
// Primary Author: Kennyth Kouch
// Secondary Author: Zachary Jones
//
//***************************************************************************
void myMotorController()
{

    uint8_t jpbs = GPIO_getInputPinValue(GPIO_PORT_P6, GPIO_PIN2);

    if(joyStickY > 0x0bff)  //Case of joyStickY High
    {
        configTimerA(469);
        GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
        GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN1);
        while(joyStickY > 0x0bff)
            {
                SpinMotorCW(10); //Only even integer
                updateStick();
            }
    }
    else if(joyStickY < 0x0400)
    {
        configTimerA(938);
        GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
        GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN1);
        while(joyStickY < 0x0400)
            {
                SpinMotorCCW(10); //Only even integer
                updateStick();
            }
    }
    else if(joyStickX > 0x0bc1)
    {
        GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);
        GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN1);
        configTimerA(469);
        while(joyStickX > 0x0bc1)
        {
            SpinMotorCW(10); //Only even integer
            updateStick();
        }
    }
    else if(joyStickX < 0x03eb)
    {
        GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);
        GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN1);
        configTimerA(938);
        while(joyStickX < 0x03eb){
            SpinMotorCCW(10); //Only even integer
            updateStick();
        }
    }
    else if(jpbs == 0)
    {
        GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);
        GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN1);
        motorDriverPattern();
    }
    else
    {
        GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
        blinkLED();
    }
    configTimerA(delayValue);
}

//***************************************************************************
//
//                    updateStick()
//       Gets current joystick position
//              no inputs or outputs
// Primary Author: Kennyth Kouch
//
//***************************************************************************
void updateStick()
{
    ADC12_B_startConversion(ADC12_B_BASE, ADC12_B_START_AT_ADC12MEM0,
    ADC12_B_SEQOFCHANNELS);
    while (ADC12_B_getInterruptStatus(ADC12_B_BASE, 0, ADC12_B_IFG1) != ADC12_B_IFG1);   // Wait for conversion

    // Get joyStickX results IFG is cleared
    joyStickX = ADC12_B_getResults(ADC12_B_BASE, ADC12_B_MEMORY_0);
    // Get joyStickY results IFG is cleared
    joyStickY = ADC12_B_getResults(ADC12_B_BASE, ADC12_B_MEMORY_1);

    // Clear IFG1 explicitly
    ADC12_B_clearInterrupt(ADC12_B_BASE, 0, ADC12_B_IFG1);
}

//***************************************************************************
//
//                    configIO()
//       this function configures the pins on the LaunchPad
//              no inputs or outputs
// Primary Author: Kennyth Kouch
// Secondary Author: Zachary Jones
//
//***************************************************************************
void configIO(void)
{

    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P4, GPIO_PIN3); //Pushbutton 1
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P4, GPIO_PIN2); //Pushbutton 2
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P6,GPIO_PIN2);

    //Stepper Motor Pins
    GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN7);
    GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN6);
    GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN5);
    GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN4);

    GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN6);
    GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN5);
    GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN4);

    //LEDs
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN1);

    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN1);

    PMM_unlockLPM5();

}

//**************************************************************************
//
//                     joystick_init()
//      this function initializes the joystick controller
//  inputs: none
//  outputs: none
//  Primary Author: Kennyth Kouch
//  Secondary Author: Zachary Jones
//
//**************************************************************************
void joyStick_init()
{

    // JoyStick X
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P3, GPIO_PIN3,GPIO_TERNARY_MODULE_FUNCTION);

    // JoyStick Y
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, GPIO_PIN2,GPIO_TERNARY_MODULE_FUNCTION);

}

//**************************************************************************
//
//                    ADC_init()
//      this function allows the joystick to be an input
//  inputs: none
//  outputs: none
//  Primary author: Kennyth Kouch
//  Secondary Author: Zachary Jones
//
//**************************************************************************
void ADC_init()
{

//Initialize the ADC12B Module
/*
 * Base address of ADC12B Module
 * Use internal ADC12B bit as sample/hold signal to start conversion
 * USE MODOSC 5MHZ Digital Oscillator as clock source
 * Use default clock divider/pre-divider of 1
 * Not use internal channel
 */
    ADC12_B_initParam initParam = { 0 };
    initParam.sampleHoldSignalSourceSelect = ADC12_B_SAMPLEHOLDSOURCE_SC;
    initParam.clockSourceSelect = ADC12_B_CLOCKSOURCE_ADC12OSC;
    initParam.clockSourceDivider = ADC12_B_CLOCKDIVIDER_1;
    initParam.clockSourcePredivider = ADC12_B_CLOCKPREDIVIDER__1;
    initParam.internalChannelMap = ADC12_B_NOINTCH;
    ADC12_B_init(ADC12_B_BASE, &initParam);

//Enable the ADC12B module
    ADC12_B_enable(ADC12_B_BASE);

/*
 * Base address of ADC12B Module
 * For memory buffers 0-7 sample/hold for 64 clock cycles
 * For memory buffers 8-15 sample/hold for 4 clock cycles (default)
 * Enable Multiple Sampling
 */

    ADC12_B_setupSamplingTimer(ADC12_B_BASE,ADC12_B_CYCLEHOLD_16_CYCLES,ADC12_B_CYCLEHOLD_4_CYCLES,ADC12_B_MULTIPLESAMPLESENABLE);

//Configure Memory Buffer
/*
 * Base address of the ADC12B Module
 * Configure memory buffer 0
 * Map input A1 to memory buffer 0
 * Vref+ = AVcc
 * Vref- = AVss
 * Memory buffer 0 is not the end of a sequence
 */

//  joyStickXParam Structure
    ADC12_B_configureMemoryParam joyStickXParam = { 0 };
    joyStickXParam.memoryBufferControlIndex = ADC12_B_MEMORY_0;
    joyStickXParam.inputSourceSelect = ADC12_B_INPUT_A2;
    joyStickXParam.refVoltageSourceSelect = ADC12_B_VREFPOS_AVCC_VREFNEG_VSS;
    joyStickXParam.endOfSequence = ADC12_B_NOTENDOFSEQUENCE;
    joyStickXParam.windowComparatorSelect = ADC12_B_WINDOW_COMPARATOR_DISABLE;
    joyStickXParam.differentialModeSelect = ADC12_B_DIFFERENTIAL_MODE_DISABLE;
    ADC12_B_configureMemory(ADC12_B_BASE, &joyStickXParam);

//  joyStickYParam Structure
    ADC12_B_configureMemoryParam joyStickYParam = { 0 };
    joyStickYParam.memoryBufferControlIndex = ADC12_B_MEMORY_1;
    joyStickYParam.inputSourceSelect = ADC12_B_INPUT_A15;
    joyStickYParam.refVoltageSourceSelect = ADC12_B_VREFPOS_AVCC_VREFNEG_VSS;
    joyStickYParam.endOfSequence = ADC12_B_ENDOFSEQUENCE;
    joyStickYParam.windowComparatorSelect = ADC12_B_WINDOW_COMPARATOR_DISABLE;
    joyStickYParam.differentialModeSelect = ADC12_B_DIFFERENTIAL_MODE_DISABLE;
    ADC12_B_configureMemory(ADC12_B_BASE, &joyStickYParam);

// Clear Interrupt
    ADC12_B_clearInterrupt(ADC12_B_BASE, 0, ADC12_B_IFG1);

}

//**************************************************************************
//
//                    LCD_init()
//      this function initializes the liquid crystal display
//  inputs: none
//  outputs: none
//  Primary Author: Kennyth Kouch
//**************************************************************************
void LCD_init()
{

/* Initializes display */
    Crystalfontz128x128_Init();

/* Set default screen orientation */
    Crystalfontz128x128_SetOrientation(0);

/* Initializes graphics context */
    Graphics_initContext(&g_sContext, &g_sCrystalfontz128x128);
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_RED);
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    GrContextFontSet(&g_sContext, &g_sFontFixed6x8);
    Graphics_clearDisplay(&g_sContext);

}

//****************************************************************************
//
//                     configTimerA()
//      this function initializes the controller timer
//  inputs: clockSource, delayValue, clockDividerValue
//  outputs: none
//  Primary Author: Zachary Jones
//  Secondary Author: Victor Perry
//***************************************************************************

void configTimerA(uint16_t delayValue)

{

    MyTimerA.clockSource = TIMER_A_CLOCKSOURCE_SMCLK;
    MyTimerA.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_20;
    MyTimerA.timerPeriod = delayValue;
    MyTimerA.timerClear = TIMER_A_DO_CLEAR;
    MyTimerA.startTimer = false;
    Timer_A_clearTimerInterrupt(TIMER_A0_BASE);   // Reset TAIFG to Zero
    Timer_A_initUpMode(TIMER_A0_BASE,&MyTimerA); // Initialize the timer
    Timer_A_startCounter(TIMER_A0_BASE,TIMER_A_UP_MODE);  // Start Timer

}

//****************************************************************************
//              RotateBladeStep1()
//      Sets A and A(bar) to high
// Inputs: none
// Returns: none
// Primary Author: Charleston Andrews
// Secondary Author: Michael Leo
//***************************************************************************
void RotateBladeStep1(){
    GPIO_setOutputHighOnPin(GPIO_PORT_P3,GPIO_PIN4);
    GPIO_setOutputLowOnPin(GPIO_PORT_P3,GPIO_PIN5);
    GPIO_setOutputHighOnPin(GPIO_PORT_P3,GPIO_PIN6);
    GPIO_setOutputLowOnPin(GPIO_PORT_P3,GPIO_PIN7);
    uint16_t i;
    for(i = 0; i < 500; i++);
}

//****************************************************************************
//              RotateBladeStep2()
//      Sets B and B(bar) to high
// Inputs: none
// Returns: none
// Primary Author: Charleston Andrews
// Secondary Author: Michael Leo
//***************************************************************************
void RotateBladeStep2(){
    GPIO_setOutputLowOnPin(GPIO_PORT_P3,GPIO_PIN4);
    GPIO_setOutputHighOnPin(GPIO_PORT_P3,GPIO_PIN5);
    GPIO_setOutputLowOnPin(GPIO_PORT_P3,GPIO_PIN6);
    GPIO_setOutputHighOnPin(GPIO_PORT_P3,GPIO_PIN7);
    uint16_t i;
    for(i = 0; i < 500; i++);
}

//****************************************************************************
//              RotateBladeStepMid()
//      Sets all magnets to low between each step
// Inputs: none
// Returns: none
// Primary Author: Kennyth Kouch
//***************************************************************************
void RotateBladeStepMid(){
    GPIO_setOutputLowOnPin(GPIO_PORT_P3,GPIO_PIN4);
    GPIO_setOutputLowOnPin(GPIO_PORT_P3,GPIO_PIN5);
    GPIO_setOutputLowOnPin(GPIO_PORT_P3,GPIO_PIN6);
    GPIO_setOutputLowOnPin(GPIO_PORT_P3,GPIO_PIN7);
    uint16_t i;
    for(i = 0; i < 500; i++);
}

//****************************************************************************
//              SpinMotorCW()
//      Drives the motor CW
// Inputs: inputVal, inputVal*2 = # of steps
// Returns: none
// Primary Author: Charleston Andrews
// Secondary Author: Michael Leo
//***************************************************************************
void SpinMotorCW (uint8_t inputVal){
    uint8_t i;
    for(i = 0; i < inputVal; i++){
        if(i % 2 == 0)
            RotateBladeStep1();
        else
            RotateBladeStep2();
        RotateBladeStepMid();
    }
}

//****************************************************************************
//              SpinMotorCCW()
//      Drives the motor CCW
// Inputs: inputVal, inputVal*2 = # of steps
// Returns: none
// Primary Author: Charleston Andrews
// Secondary Author: Michael Leo
//***************************************************************************
void SpinMotorCCW (uint8_t inputVal){
    uint8_t i;
    for(i = 0; i < inputVal; i++){
        if(i % 2 == 0)
            RotateBladeStep2();
        else
            RotateBladeStep1();
        RotateBladeStepMid();
    }
}

//****************************************************************************
//              motorDriverPattern()
//      Drives the motor in pattern mode
// Inputs: patternCount, gives the driver the current state
// Returns: none
// Primary Author: Michael Leo
// Secondary Author: Charleston Andrews
//***************************************************************************
void motorDriverPattern()
{

    uint8_t i;
    uint32_t j;
    for(i = 0; i < 10; i++){
        SpinMotorCW(stepArray[i]);
        for(j = 0; j < 100000; j++);
        //Delay

    }

}

//****************************************************************************
//              blinkLED()
//      Blinks LED2 when design is on standby
// Inputs: none
// Returns: none
// Primary Author: Kennyth Kouch
//***************************************************************************
void blinkLED()
{
    uint16_t j;
    GPIO_setOutputLowOnPin(GPIO_PORT_P1,GPIO_PIN1);
    for(j = 0; j < 50000; j++);
    GPIO_setOutputHighOnPin(GPIO_PORT_P1,GPIO_PIN1);
}

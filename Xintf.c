/*
 * Xintf.c
 *
 *  Created on: 2013-8-8
 *      Author: velarn
 *        功能 ：外部SRAM调用
 */

#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File
#include "MyXintf.H"

void My_InitXintf16Gpio()
{
     EALLOW;
     GpioCtrlRegs.GPCMUX1.bit.GPIO64 = 3;  // XD15
     GpioCtrlRegs.GPCMUX1.bit.GPIO65 = 3;  // XD14
     GpioCtrlRegs.GPCMUX1.bit.GPIO66 = 3;  // XD13
     GpioCtrlRegs.GPCMUX1.bit.GPIO67 = 3;  // XD12
     GpioCtrlRegs.GPCMUX1.bit.GPIO68 = 3;  // XD11
     GpioCtrlRegs.GPCMUX1.bit.GPIO69 = 3;  // XD10
     GpioCtrlRegs.GPCMUX1.bit.GPIO70 = 3;  // XD19
     GpioCtrlRegs.GPCMUX1.bit.GPIO71 = 3;  // XD8
     GpioCtrlRegs.GPCMUX1.bit.GPIO72 = 3;  // XD7
     GpioCtrlRegs.GPCMUX1.bit.GPIO73 = 3;  // XD6
     GpioCtrlRegs.GPCMUX1.bit.GPIO74 = 3;  // XD5
     GpioCtrlRegs.GPCMUX1.bit.GPIO75 = 3;  // XD4
     GpioCtrlRegs.GPCMUX1.bit.GPIO76 = 3;  // XD3
     GpioCtrlRegs.GPCMUX1.bit.GPIO77 = 3;  // XD2
     GpioCtrlRegs.GPCMUX1.bit.GPIO78 = 3;  // XD1
     GpioCtrlRegs.GPCMUX1.bit.GPIO79 = 3;   // XD0

     GpioCtrlRegs.GPBMUX1.bit.GPIO40 = 3;  // XA0/XWE1n
     GpioCtrlRegs.GPBMUX1.bit.GPIO41 = 3;  // XA1
     GpioCtrlRegs.GPBMUX1.bit.GPIO42 = 3;  // XA2
     GpioCtrlRegs.GPBMUX1.bit.GPIO43 = 3;  // XA3
     GpioCtrlRegs.GPBMUX1.bit.GPIO44 = 3;  // XA4
     GpioCtrlRegs.GPBMUX1.bit.GPIO45 = 3;  // XA5
     GpioCtrlRegs.GPBMUX1.bit.GPIO46 = 3;  // XA6
     GpioCtrlRegs.GPBMUX1.bit.GPIO47 = 3;  // XA7
     GpioCtrlRegs.GPCMUX2.bit.GPIO80 = 3;  // XA8
     GpioCtrlRegs.GPCMUX2.bit.GPIO81 = 3;  // XA9
     GpioCtrlRegs.GPCMUX2.bit.GPIO82 = 3;  // XA10
     GpioCtrlRegs.GPCMUX2.bit.GPIO83 = 3;  // XA11
     GpioCtrlRegs.GPCMUX2.bit.GPIO84 = 3;  // XA12
     GpioCtrlRegs.GPCMUX2.bit.GPIO85 = 3;  // XA13
     GpioCtrlRegs.GPCMUX2.bit.GPIO86 = 3;  // XA14
     GpioCtrlRegs.GPCMUX2.bit.GPIO87 = 3;  // XA15
     GpioCtrlRegs.GPBMUX1.bit.GPIO39 = 3;  // XA16
     GpioCtrlRegs.GPAMUX2.bit.GPIO31 = 3;  // XA17
     GpioCtrlRegs.GPAMUX2.bit.GPIO30 = 3;  // XA18

//     GpioCtrlRegs.GPAMUX2.bit.GPIO29 = 3;  // XA19

//   GpioCtrlRegs.GPBMUX1.bit.GPIO34 = 3;  // XREADY
	 GpioCtrlRegs.GPBMUX1.bit.GPIO35 = 3;  // XRNW
     GpioCtrlRegs.GPBMUX1.bit.GPIO38 = 3;  // XWE0

//   GpioCtrlRegs.GPBMUX1.bit.GPIO36 = 3;  // XZCS0
     GpioCtrlRegs.GPBMUX1.bit.GPIO37 = 3;  // XZCS7
//     GpioCtrlRegs.GPAMUX2.bit.GPIO28 = 3;  // XZCS6

     EDIS;
}

void My_InitXintf(void)
{
    // This shows how to write to the XINTF registers.  The
    // values used here are the default state after reset.
    // Different hardware will require a different configuration.

    // For an example of an XINTF configuration used with the
    // F28335 eZdsp, refer to the examples/run_from_xintf project.

    // Any changes to XINTF timing should only be made by code
    // running outside of the XINTF.

    // All Zones---------------------------------

    // All Zones---------------------------------
    // Timing for all zones based on XTIMCLK = 1/2 SYSCLKOUT
    EALLOW;
    XintfRegs.XINTCNF2.bit.XTIMCLK = 1;
    // No write buffering
    XintfRegs.XINTCNF2.bit.WRBUFF = 0;
    // XCLKOUT is enabled
    XintfRegs.XINTCNF2.bit.CLKOFF = 0;
    // XCLKOUT = XTIMCLK/2
    XintfRegs.XINTCNF2.bit.CLKMODE = 1;

    // Zone 6------------------------------------
    // When using ready, ACTIVE must be 1 or greater
    // Lead must always be 1 or greater
    // Zone write timing
    XintfRegs.XTIMING6.bit.XWRLEAD = 1;
    XintfRegs.XTIMING6.bit.XWRACTIVE = 1;
    XintfRegs.XTIMING6.bit.XWRTRAIL = 0;
    // Zone read timing
    XintfRegs.XTIMING6.bit.XRDLEAD = 1;
    XintfRegs.XTIMING6.bit.XRDACTIVE = 1;
    XintfRegs.XTIMING6.bit.XRDTRAIL = 0;

    // double all Zone read/write lead/active/trail timing
    XintfRegs.XTIMING6.bit.X2TIMING = 0;

    // Zone will sample XREADY signal
    XintfRegs.XTIMING6.bit.USEREADY = 0;
    XintfRegs.XTIMING6.bit.READYMODE = 1;  // sample asynchronous

    // Size must be either:
    // 0,1 = x32 or
    // 1,1 = x16 other values are reserved
    XintfRegs.XTIMING6.bit.XSIZE = 3;

    // Zone 7------------------------------------
    // When using ready, ACTIVE must be 1 or greater
    // Lead must always be 1 or greater
    // Zone write timing
    XintfRegs.XTIMING7.bit.XWRLEAD   = 1;//3 1 mean 2 XTIMCLK cycles Lead Period
    XintfRegs.XTIMING7.bit.XWRACTIVE = 1;//7
    XintfRegs.XTIMING7.bit.XWRTRAIL  = 0;//3
    // Zone read timing
    XintfRegs.XTIMING7.bit.XRDLEAD   = 1;//3
    XintfRegs.XTIMING7.bit.XRDACTIVE = 1;//7
    XintfRegs.XTIMING7.bit.XRDTRAIL  = 0;//3


    // Zone will sample XREADY signal
    XintfRegs.XTIMING7.bit.USEREADY  = 0;
    XintfRegs.XTIMING7.bit.READYMODE = 1;  // sample asynchronous

    // Size must be either:
    // 0,1 = x32 or
    // 1,1 = x16 other values are reserved
    XintfRegs.XTIMING7.bit.XSIZE = 3;

    // Bank switching
    // Assume Zone 7 is slow, so add additional BCYC cycles
    // when ever switching from Zone 7 to another Zone.
    // This will help avoid bus contention.
    XintfRegs.XBANK.bit.BANK = 1;//7
    XintfRegs.XBANK.bit.BCYC = 7;//7
    EDIS;
   //Force a pipeline flush to ensure that the write to
   //the last register configured occurs before returning.
   //Force a pipeline flush to ensure that the write to
   //the last register configured occurs before returning.

    My_InitXintf16Gpio();

   asm(" RPT #7 || NOP");

}



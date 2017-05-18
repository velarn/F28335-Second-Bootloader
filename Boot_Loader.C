/*
 * Boot Loader.C
 *
 *  Created on: 2015-4-17
 *      Author: Velarn

 */


#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File
#include "math.h"
#include "float.h"
//
#include "stdio.h"
#include "SCI.H"
#include "MyXintf.H"
#include "Flash2833x_API_Library.h"
#include "FPU.h"


#define		Debug

#define		Cycle_LED_ON	GpioDataRegs.GPACLEAR.bit.GPIO27 	= 1;
#define		Cycle_LED_OFF	GpioDataRegs.GPASET.bit.GPIO27 		= 1;

#define		Target_LED_ON	GpioDataRegs.GPACLEAR.bit.GPIO26 	= 1;
#define		Target_LED_OFF	GpioDataRegs.GPASET.bit.GPIO26 		= 1;

#define		Trig_LED_ON		GpioDataRegs.GPACLEAR.bit.GPIO25 	= 1;
#define		Trig_LED_OFF	GpioDataRegs.GPASET.bit.GPIO25 		= 1;


#define		Sys_State_Wait_Update_Cmd	0x00
#define		Sys_State_Updating			0x01
#define		Sys_Jump_To_App				0x03

#define		App_Address		0x00307FF6
extern 		void jumpToAppEntry();

const unsigned long *App_Entry = (unsigned long*)(App_Address);

void main()
{
	unsigned int 	i = 0;
	unsigned int	sys_state = Sys_State_Wait_Update_Cmd;

	InitSysCtrl();

	DINT;

	InitPieCtrl();

	MemCopy(&RamfuncsLoadStart,	&RamfuncsLoadEnd, &RamfuncsRunStart);
	MemCopy(&Flash28_API_LoadStart, &Flash28_API_LoadEnd, &Flash28_API_RunStart);

	InitFlash();

	// Disable CPU interrupts and clear all CPU interrupt flags:
	IER = 0x0000;
	IFR = 0x0000;

	InitPieVectTable();
	PieCtrlRegs.PIECTRL.bit.ENPIE = 1;



	SCI_A_GPIO_Init();
	SCI_A_Init();

	My_InitXintf();

	EALLOW;
		GpioCtrlRegs.GPAMUX2.bit.GPIO27  	= 0;
		GpioCtrlRegs.GPADIR.bit.GPIO27   	= 1;//D7
		GpioDataRegs.GPASET.bit.GPIO27 		= 1;

		GpioCtrlRegs.GPAMUX2.bit.GPIO26  	= 0;
		GpioCtrlRegs.GPADIR.bit.GPIO26   	= 1;//D6
		GpioDataRegs.GPASET.bit.GPIO26 		= 1;

		GpioCtrlRegs.GPAMUX2.bit.GPIO25		= 0;
		GpioCtrlRegs.GPADIR.bit.GPIO25		= 1;//D5
		GpioDataRegs.GPASET.bit.GPIO25		= 1;

	EDIS;

	IER |= M_INT8;//Enable INT9 (8.1 I2C_INT1A)
	IER |= M_INT9;//Enable INT9 (9.1 SCI_A)
	EINT;
	ERTM;



	while(1)
	{
		switch(sys_state)
		{
		case Sys_State_Wait_Update_Cmd:
			SCI_A_Send_Char(0x24);
			SCI_A_Send_Char(0x10);
			SCI_A_Send_Char(Firmware>>8);
			SCI_A_Send_Char(Firmware);
			SCI_A_Send_Char(0x23);
			sys_state = Sys_Jump_To_App;
			i = 10;
			while(i--)
			{
				SCI_A_Process();
				if(Update_Flag==1)
				{
					sys_state = Sys_State_Updating;
					break;
				}
				DELAY_US(100000);
			}
			break;
		case Sys_State_Updating:
				while(FW_Burning_Success == 0)
				{
					SCI_A_Process();
				}
				if(FW_Burning_Success == 1)
				{

					SCI_A_Send_Char(0x24);
					SCI_A_Send_Char(0x86);
					SCI_A_Send_Char(0x00);
					SCI_A_Send_Char(FW_Burning_Success);
					SCI_A_Send_Char(0x23);
					DELAY_US(1000000);
					sys_state = Sys_Jump_To_App;
				}
				else if(FW_Burning_Success == 2)
				{

					SCI_A_Send_Char(0x24);
					SCI_A_Send_Char(0x87);
					SCI_A_Send_Char(0x00);
					SCI_A_Send_Char(FW_Burning_Success);
					SCI_A_Send_Char(0x23);
					DELAY_US(1000000);
					sys_state = Sys_State_Wait_Update_Cmd;
				}
				else
				{

					SCI_A_Send_Char(0x24);
					SCI_A_Send_Char(0x89);
					SCI_A_Send_Char(0x00);
					SCI_A_Send_Char(FW_Burning_Success);
					SCI_A_Send_Char(0x23);
					DELAY_US(1000000);
					sys_state = Sys_State_Wait_Update_Cmd;
				}

			break;
		case Sys_Jump_To_App:
				if((*App_Entry != 0xFFFFFFFF))
				{
					SCI_A_Send_Char(0x24);
					SCI_A_Send_Char(0x91);
					SCI_A_Send_Char(0x00);
					SCI_A_Send_Char(0x01);
					SCI_A_Send_Char(0x23);
					DELAY_US(100000);
					jumpToAppEntry();
				}
				else
				{
					SCI_A_Send_Char(0x24);
					SCI_A_Send_Char(0x91);
					SCI_A_Send_Char(0x00);
					SCI_A_Send_Char(0x00);
					SCI_A_Send_Char(0x23);
					DELAY_US(1000);
					sys_state = Sys_State_Wait_Update_Cmd;
				}
			break;
		}

	}
}


//===========================================================================
// No more.
//===========================================================================








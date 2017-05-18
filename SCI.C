/*
 * SCI.C
 *
 *  Created on: 2013-8-8
 *      Author: velarn
 */
#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File
#include "SCI.H"
#include "Flash2833x_API_Library.h"

unsigned int			SCI_A_Rec_Buf[256]		= {0};
unsigned int 			SCI_A_Rec_Buf_Count     = 0;
unsigned int			SCI_A_Rec_Flag          = 0;

unsigned int			SCI_A_Rec_Buf_Len       = 0;
unsigned int			SCI_A_Rec_Buf_Line_Cnt  = 0;
unsigned int 			FW_Rec_Success			= 0;
unsigned int 			FW_Burning_Success		= 0;
unsigned int			Update_Flag				= 0;

#pragma DATA_SECTION(UPDATA,"UPDATABUF")
unsigned int			UPDATA[20000]			= {0};

static unsigned int		*UPDATAPTR 				= UPDATA;


FLASH_ST FlashStatus;

extern Uint32 Flash_CPUScaleFactor;


#pragma CODE_SECTION(SCI_A_Rec_ISR,"ramfuncs")
interrupt void SCI_A_Rec_ISR(void)
{
	if(SciaRegs.SCIRXST.bit.RXERROR)//if any Rx error occer,reset sci A
	{
		SciaRegs.SCICTL1.bit.SWRESET = 0;
		SciaRegs.SCICTL1.all 		 = 0x0023;  // enable TX, RX, internal SCICLK,
	}
	else
	{
		SCI_A_Rec_Buf[SCI_A_Rec_Buf_Count] = SciaRegs.SCIRXBUF.all;
		SCI_A_Rec_Buf_Count++;
		if(SCI_A_Rec_Buf[0]==0x24)
		{
			if(SCI_A_Rec_Buf_Count >= 5)
				{
					SCI_A_Rec_Flag = 1;
					SCI_A_Rec_Buf_Count = 0;
				}
		}
		else if(SCI_A_Rec_Buf[0]==0x55)
		{
			if(SCI_A_Rec_Buf_Count >= SCI_A_Rec_Buf_Len)
			{
				SCI_A_Rec_Flag = 1;
				SCI_A_Rec_Buf_Count = 0;
			}
		}
		else
		{
			SCI_A_Rec_Buf_Count = 0;
		}

	}
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;
}

void SCI_A_GPIO_Init()
{
	EALLOW;
	/* Enable internal pull-up for the selected pins */
	// Pull-ups can be enabled or disabled disabled by the user.
	// This will enable the pullups for the specified pins.

		GpioCtrlRegs.GPAPUD.bit.GPIO28 = 0;    // Enable pull-up for GPIO28 (SCIRXDA)
		GpioCtrlRegs.GPAPUD.bit.GPIO29 = 0;	   // Enable pull-up for GPIO29 (SCITXDA)

//		GpioCtrlRegs.GPBPUD.bit.GPIO35 = 0;		// Enable pull-up for GPIO35 (SCITXDA)
//		GpioCtrlRegs.GPBPUD.bit.GPIO36 = 0;		// Enable pull-up for GPIO36 (SCIRXDA)


	/* Set qualification for selected pins to asynch only */
	// Inputs are synchronized to SYSCLKOUT by default.
	// This will select asynch (no qualification) for the selected pins.

		GpioCtrlRegs.GPAQSEL2.bit.GPIO28 = 3;  // Asynch input GPIO28 (SCIRXDA)
	//	GpioCtrlRegs.GPBQSEL1.bit.GPIO36 = 3;  // Asynch input GPIO36 (SCIRXDA)



	/* Configure SCI-A pins using GPIO regs*/
	// This specifies which of the possible GPIO pins will be SCI functional pins.

	//Real board use GPIO28.29
		GpioCtrlRegs.GPAMUX2.bit.GPIO28 = 1;   // Configure GPIO28 for SCIRXDA operation
		GpioCtrlRegs.GPAMUX2.bit.GPIO29 = 1;   // Configure GPIO29 for SCITXDA operation

	//Test Board use GPIO35.36
//		GpioCtrlRegs.GPBMUX1.bit.GPIO36 = 1;   // Configure GPIO36 for SCIRXDA operation
//		GpioCtrlRegs.GPBMUX1.bit.GPIO35 = 1;   // Configure GPIO35 for SCITXDA operation

	EDIS;
}

void SCI_A_Init()
{
 	SciaRegs.SCICCR.all 			= 0x0007;   // 1 stop bit,  No loopback
                                   // No parity,8 char bits,
                                   // async mode, idle-line protocol
	SciaRegs.SCICTL1.all 			= 0x0003;  // enable TX, RX, internal SCICLK,
                                   // Disable RX ERR, SLEEP, TXWAKE
	SciaRegs.SCICTL2.all 			= 0x0003;

//    SciaRegs.SCIHBAUD    			= 0x0001;  // 9600 baud @LSPCLK = 37.5MHz.
//    SciaRegs.SCILBAUD    			= 0x00E7;

	SciaRegs.SCIHBAUD    			= 0x0000;  // 38400 baud @LSPCLK = 37.5MHz.
	SciaRegs.SCILBAUD    			= 0x0079;
	SciaRegs.SCICTL1.all 			= 0x0023;  // enable TX, RX, internal SCICLK,


	EALLOW;
		PieVectTable.SCIRXINTA      = &SCI_A_Rec_ISR;
//		PieVectTable.SCITXINTA      = &SCI_A_Tx_ISR;
	EDIS;

	PieCtrlRegs.PIEIER9.bit.INTx1	= 1;	 //	SCI_A RX INTERRUPT ENABLE
	PieCtrlRegs.PIEIER9.bit.INTx2	= 0;	 // SCI_A TX INTERRUPT ENABLE
}

#pragma CODE_SECTION(SCI_A_Send_Char,"ramfuncs")
void SCI_A_Send_Char(unsigned int data)
{
    while (SciaRegs.SCICTL2.bit.TXRDY == 0) {}
    SciaRegs.SCITXBUF	= data;
}

#pragma CODE_SECTION(Call_IAP,"ramfuncs")
unsigned int Call_IAP()
{
	Uint16  Status,i;
	Uint32  Length;         // Number of 16-bit values to be programmed
	Uint32  StartAddr;

	DINT;//Disable Global Interrupt

	Flash_CPUScaleFactor = SCALE_FACTOR;

	UPDATAPTR 	= UPDATA;

	Status = Flash_Erase((SECTORH),&FlashStatus);
	if(Status != STATUS_SUCCESS)
	{
		return Status;
	}
	for(i=0;i<SCI_A_Rec_Buf_Line_Cnt;i++)
	{
		Length	    = *UPDATAPTR++;
		Length    >>= 1;
		StartAddr   = *UPDATAPTR++;
		StartAddr <<= 16;
		StartAddr  |= *UPDATAPTR++;

	    Status = Flash_Program((unsigned int *)StartAddr,UPDATAPTR,Length,&FlashStatus);
	    if(Status != STATUS_SUCCESS)
	    {
	    	return Status;
	    }

	    // Verify the values programmed.  The Program step itself does a verify
	    // as it goes.  This verify is a 2nd verification that can be done.
	    Status = Flash_Verify((unsigned int *)StartAddr,UPDATAPTR,Length,&FlashStatus);
	    if(Status != STATUS_SUCCESS)
	    {
	    	return Status;
	    }

	    UPDATAPTR+=Length;
	}
	EINT;//Enable Global Interrupt
	return 0;
}


void SCI_A_Send_String( unsigned int *str)
{
	while(*str !='\0')
	{
		SCI_A_Send_Char(*str);
		str++;
	}
}

#pragma CODE_SECTION(SCI_A_Process,"ramfuncs")
void SCI_A_Process()
{
	unsigned int i;
	if(SCI_A_Rec_Flag == 1)
	{
		//Begin with 0x24 means it's the Command Package
		if((SCI_A_Rec_Buf[0]==0x24)&&(SCI_A_Rec_Buf[4]==0x23))
		{
			switch(SCI_A_Rec_Buf[1])
			{
				//IAP Process
				//Response the pc broadcast
				case 0x80:
					SCI_A_Send_Char(0x24);
					SCI_A_Send_Char(0x80);
					SCI_A_Send_Char(0x00);
					SCI_A_Send_Char(0x00);
					SCI_A_Send_Char(0x23);
					UPDATAPTR 			= UPDATA;
					SCI_A_Rec_Buf_Line_Cnt = 0;
					SCI_A_Rec_Buf_Line_Cnt = 0;
					Update_Flag = 1;
				break;
				//PC Send the package(Begin with 0xAA) length to the DSP
				//And DSP response the data length
				case 0x81:
					SCI_A_Rec_Buf_Len = SCI_A_Rec_Buf[2];
					SCI_A_Send_Char(0x24);
					SCI_A_Send_Char(0x81);
					SCI_A_Send_Char(SCI_A_Rec_Buf_Len);
					SCI_A_Send_Char(0x00);
					SCI_A_Send_Char(0x23);

				break;
				//Response the total of line count that received
				case 0x83:
					i   = SCI_A_Rec_Buf[2];
					i <<=8;
					i  |=SCI_A_Rec_Buf[3];

					if(i==SCI_A_Rec_Buf_Line_Cnt)
					{
						SCI_A_Send_Char(0x24);
						SCI_A_Send_Char(0x83);
						SCI_A_Send_Char(0x00);
						SCI_A_Send_Char(0x00);
						SCI_A_Send_Char(0x23);
						FW_Rec_Success = 1;
					}
					else
					{
						SCI_A_Send_Char(0x24);
						SCI_A_Send_Char(0x88);
						SCI_A_Send_Char(0x00);
						SCI_A_Send_Char(0x00);
						SCI_A_Send_Char(0x23);
						FW_Rec_Success = 0;
					}
				break;
				case 0x84:
					if(FW_Rec_Success == 1)
					{
						SCI_A_Send_Char(0x24);
						SCI_A_Send_Char(0x84);
						SCI_A_Send_Char(0x00);
						SCI_A_Send_Char(0x00);
						SCI_A_Send_Char(0x23);
						i = Call_IAP();
						if(i==0)
						{
							FW_Rec_Success = 0;
							FW_Burning_Success = 1;
							GpioDataRegs.GPACLEAR.bit.GPIO25 	= 1;
						}
						else
						{
							FW_Rec_Success = 0;
							FW_Burning_Success = 2;
							GpioDataRegs.GPACLEAR.bit.GPIO26	= 1;
						}
					}
					else
					{
						GpioDataRegs.GPACLEAR.bit.GPIO27 	= 1;
						FW_Burning_Success = 3;
					}
				break;
				case 0x90:
					SCI_A_Send_Char(0x24);
					SCI_A_Send_Char(0x90);
					SCI_A_Send_Char(0x00);
					SCI_A_Send_Char(0x00);
					SCI_A_Send_Char(0x23);
					EALLOW;
						SysCtrlRegs.WDCR= 0x0078;
					EDIS;
				break;

			}
		}
		//Begin with 0x55 means it's the Data Package
		else if((SCI_A_Rec_Buf[0]==0x55))
		{
			unsigned int ECC = 0;
			unsigned int i=0;
			//Calculate The ECC
			for(i=0;i<SCI_A_Rec_Buf_Len-1;i++)
			{
				ECC += SCI_A_Rec_Buf[i];
			}

			ECC = 256 - ECC;
			//If ECC equal the PC sent,Response ECC;
			if((ECC&0x00FF) == SCI_A_Rec_Buf[SCI_A_Rec_Buf_Len-1])
			{
				*UPDATAPTR++ = SCI_A_Rec_Buf[1];
				for(i=2;i<SCI_A_Rec_Buf_Len-1;i=i+2)
				{
					*UPDATAPTR++ = ((SCI_A_Rec_Buf[i]<<8) | SCI_A_Rec_Buf[i+1]);
				}

				SCI_A_Rec_Buf_Line_Cnt++;
				SCI_A_Send_Char(0x24);
				SCI_A_Send_Char(0x82);
				SCI_A_Send_Char(0x00);
				SCI_A_Send_Char(0x00);
				SCI_A_Send_Char(0x23);
			}
			//If ECC don't equal the PC sent ECC,ACK and request Send this package again;
			else
			{
				SCI_A_Send_Char(0x24);
				SCI_A_Send_Char(0x85);
				SCI_A_Send_Char(0x00);
				SCI_A_Send_Char(0x00);
				SCI_A_Send_Char(0x23);
			}

		}
		else
		{
			SCI_A_Rec_Buf[0] = 0;
			SCI_A_Rec_Buf[1] = 0;
			SCI_A_Rec_Buf[2] = 0;
			SCI_A_Rec_Buf[3] = 0;
			SCI_A_Rec_Buf[4] = 0;

			SCI_A_Send_Char(0x24);
			SCI_A_Send_Char(0xAA);
			SCI_A_Send_Char(0xAA);
			SCI_A_Send_Char(0xAA);
			SCI_A_Send_Char(0x23);
		}

		SCI_A_Rec_Flag = 0;
	}
}


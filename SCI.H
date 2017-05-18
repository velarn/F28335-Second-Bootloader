/*
 * SCI.H
 *
 *  Created on: 2013-8-12
 *      Author: velarn
 */

#ifndef SCI_H_
#define SCI_H_

//Firmware Vision
#define					Firmware				0x1002

extern unsigned int     Update_Flag				;   //升级标志位
extern unsigned int 	FW_Burning_Success		;	//烧录成功标志位

extern void 			SCI_A_GPIO_Init();
extern void 			SCI_A_Init();
extern void 			SCI_A_Send_Char(unsigned int data);
extern void 			SCI_A_Send_String(unsigned int *str);
extern void 			SCI_A_Process();

#endif /* SCI_H_ */

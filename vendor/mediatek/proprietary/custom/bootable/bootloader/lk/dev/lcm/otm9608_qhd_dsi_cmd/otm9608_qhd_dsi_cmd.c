/* Copyright Statement:
*
* This software/firmware and related documentation ("MediaTek Software") are
* protected under relevant copyright laws. The information contained herein
* is confidential and proprietary to MediaTek Inc. and/or its licensors.
* Without the prior written permission of MediaTek inc. and/or its licensors,
* any reproduction, modification, use or disclosure of MediaTek Software,
* and information contained herein, in whole or in part, shall be strictly prohibited.
*/
/* MediaTek Inc. (C) 2015. All rights reserved.
*
* BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
* THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
* RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
* AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
* NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
* SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
* SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
* THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
* THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
* CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
* SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
* STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
* CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
* AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
* OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
* MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*/
#include <debug.h>
#include "lcm_drv.h"

// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH  (540)
#define FRAME_HEIGHT (960)

// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)    (lcm_util.set_reset_pin((v)))

#define UDELAY(n) (lcm_util.udelay(n))
#define MDELAY(n) (lcm_util.mdelay(n))
#define REGFLAG_DELAY                                       0XFD
#define REGFLAG_END_OF_TABLE                                0xFE   // END OF REGISTERS MARKER


// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------
#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)    lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)       lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)                                  lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)              lcm_util.dsi_write_regs(addr, pdata, byte_nums)
//#define read_reg                                          lcm_util.dsi_read_reg()
#define read_reg_v2(cmd, buffer, buffer_size)                   lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)



#define LCM_DSI_CMD_MODE

struct LCM_setting_table {
	unsigned cmd;
	unsigned char count;
	unsigned char para_list[64];
};


static struct LCM_setting_table lcm_initialization_setting_xinli[] = {
/*
	Note :

	Data ID will depends on the following rule.
	
		count of parameters > 1	=> Data ID = 0x39
		count of parameters = 1	=> Data ID = 0x15
		count of parameters = 0	=> Data ID = 0x05

	Structure Format :

	{DCS command, count of parameters, {parameter list}}
	{REGFLAG_DELAY, milliseconds of time, {}},

	...

	Setting ending by predefined flag
	
	{REGFLAG_END_OF_TABLE, 0x00, {}}
	*/

    {0x00,	1,	{0x00}},
    {0xFF,3,	{0x96, 0x08, 0x01}},// Enable cmd                                                                                                                                            
    {0x00,1,	{0x80}},	                                                                                                                                                                  
    {0xFF,2,	{0x96, 0x08}},// Enable cmd                                                                                                                                                  
    {0x00,1,	{0x00}},                                                                                                                                                                     
    {0xA0,1,	{0x00}},//OTP select region                                                                                                                                                  
    {0x00,1,	{0x80}},                                                                                                                                                                     
    {0xB3,5,    {0x00, 0x00, 0x20, 0x00, 0x00}},//Command Set Option Parameter                                                                                                               
    {0x00,1,	{0xC0}},                                                                                                                                                                     
    {0xB3,1,	{0x09}},//SRAM Setting                                                                                                                                                       
    {0x00,1,	{0x80}},                                                                                                                                                                     
    {0xC0,9,	{0x00, 0x48, 0x00, 0x10, 0x10, 0x00, 0x47, 0x10, 0x10}},//TCON Setting Parameters                                                                                            
    {0x00,1,	{0x92}},                                                                                                                                                                     
    {0xC0,4,	{0x00, 0x10, 0x00, 0x13}},//Panel Timing Setting Parameter                                                                                                                   
    {0x00,1,	{0xA2}},                                                                                                                                                                     
    {0xC0,3,	{0x0C, 0x05, 0x02}},//Panel Timing Setting Parameter                                                                                                                         
    {0x00,1,	{0xB3}},                                                                                                                                                                     
    {0xC0,2,	{0x00, 0x50}},//Interval Scan Frame Setting                                                                                                                                  
    {0x00,1,    {0x81}},                                                                                                                                                                     
    {0xC1,1,    {0x55}},//Oscillator Adjustment for Idle/Normal Mode  change fps                                                                                                                         
    {0x00,1,    {0x80}},                                                                                                                                                                     
    {0xC4,3,    {0x00, 0x84, 0xFC}},                                                                                                                                                         

    {0x00,1,    {0x80}},
    {0xC4,1,    {0x30}},
    {REGFLAG_DELAY, 10, {}},

    {0x00,1,    {0x80}},
    {0xC4,1,    {0x40}},
    {REGFLAG_DELAY, 10, {}},
    {0x00,1,    {0xA0}},                                                                                                                                                                     
    {0xB3,2,	{0x10, 0x00} },                                                                                                                                                              
    {0x00,1,	{0xA0}},                                                                                                                                                                     
    {0xC0,1,    {0x00}},                                                                                                                                                                     
                                                                                                                                    
    {0x00,1,	{0xA0}},                                                                                                                                                                     
    {0xC4,8,	{0x33, 0x09, 0x90, 0x2B, 0x33, 0x09, 0x90, 0x54}},//DC2DC Setting                                                                                                            
    {0x00,1,	{0x80}},                                                                                                                                                                     
    {0xC5,4,	{0x08, 0x00, 0xA0, 0x11}},                                                                                                                                                   
    {0x00,1,	{0x90}},                                                                                                                                                                     
    {0xC5,7,	{0x96, 0x38, 0x01, 0x38, 0x33, 0x33, 0x34}},//Power Control Setting2 for Normal Mode                                                                                         
    {0x00,1,	{0xA0}},                                                                                                                                                                     
    {0xC5,7,	{0x96, 0x57, 0x00, 0x57, 0x33, 0x33, 0x34}},//Power Control Setting3 for Idle Mode                                                                                           
    {0x00,1,	{0xB0}},                                                                                                                                                                     
    {0xC5,7,	{0x04, 0xAC, 0x01, 0x00, 0x71, 0xB1, 0x83}},//Power Control Setting3 for DC Voltage Settings                                                                                 
    {0x00,1,	{0x00}},                                                                                                                                                                     
    {0xD9,1,	{0x74}},//Vcom setting //0x75                                                                                                                                                      
    {0x00,1,	{0x80}},                                                                                                                                                                     
    {0xC6,1,	{0x64}},//ABC Parameter                                                                                                                                                      
    {0x00,1,	{0xB0}},                                                                                                                                                                     
    {0xC6,5,	{0x03, 0x10, 0x00, 0x1F, 0x12}},//ABC Parameter                                                                                                                              
                                                                                                                                                    
    {0x00,1,	{0xB7}},                                                                                                                                                                     
    {0xB0,1,	{0x10}},                                                                                                                                                                     
    {0x00,1,	{0xC0}},                                                                                                                                                                     
    {0xB0,1,	{0x55}},                                                                                                                                                                     
    {0x00,1,	{0xB1}},                                                                                                                                                                     
    {0xB0,1,	{0x03}},                                                                                                                                                                     
    {0x00,1,	{0x81}},                                                                                                                                                                     
    {0xD6,1,	{0x00}},//Sharpness Off                                                                                                                                                      
    {0x00,1,	{0x00}},                                                                                                                                                                     
    {0xE1,16,   {0x01, 0x0b, 0x10, 0x0e, 0x06, 0x0d, 0x0A, 0x08, 0x06, 0x08, 0x0F, 0x08, 0x0F, 0x15, 0x0e, 0x01}},                                                                           
    {0x00,1,	{0x00}},                                                                                                                                                                     
    {0xE2,17,   {0x02, 0x0a, 0x10, 0x0d, 0x06, 0x0D, 0x09, 0x07, 0x05, 0x08, 0x0f, 0x08, 0x0E, 0x14, 0x0e, 0x00}},                                                                           
    {0x00,1,	{0x80}},                                                                                                                                                                     
    {0xCB,10,   {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},//TCON GOA WAVE(Panel timing state control)                                                                    
    {0x00,1,	{0x90}},                                                                                                                                                                     
    {0xCB,15,   {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},//TCON GOA WAVE(Panel timing state control)                                      
    {0x00,1,	{0xA0}},                                                                                                                                                                     
    {0xCB,15,   {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},//TCON GOA WAVE(Panel timing state control)                                      
    {0x00,1,	{0xB0}},                                                                                                                                                                     
    {0xCB,10,   {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},//TCON GOA WAVE(Panel timing state control)                                                                    
    {0x00,1,	{0xC0}},                                                                                                                                                                     
    {0xCB,15,   {0x04, 0x04, 0x04, 0x04, 0x08, 0x04, 0x08, 0x04, 0x08, 0x04, 0x08, 0x04, 0x04, 0x04, 0x08}},//TCON GOA WAVE(Panel timing state control)                                      
    {0x00,1,	{0xD0}},                                                                                                                                                                     
    {0xCB,15,   {0x08, 0x00, 0x00, 0x00, 0x00, 0x04, 0x04, 0x04, 0x04, 0x08, 0x04, 0x08, 0x04, 0x08, 0x04}},//TCON GOA WAVE(Panel timing state control)                                      
    {0x00,1,	{0xE0}},                                                                                                                                                                     
    {0xCB,10,   {0x08, 0x04, 0x04, 0x04, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00}},//TCON GOA WAVE(Panel timing state control)                                                                    
    {0x00,1,	{0xF0}},                                                                                                                                                                     
    {0xCB,10,   {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}},//TCON GOA WAVE(Panel timing state control)                                                                    
    {0x00,1,	{0x80}},                                                                                                                                                                     
    {0xCC,10,   {0x26, 0x25, 0x23, 0x24, 0x00, 0x0F, 0x00, 0x0D, 0x00, 0x0B}},//TCON GOA WAVE(Panel timing state control)                                                                    
    {0x00,1,	{0x90}},                                                                                                                                                                     
    {0xCC,15,   {0x00, 0x09, 0x01, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x26, 0x25, 0x21, 0x22, 0x00}},//TCON GOA WAVE(Panel timing state control)                                      
    {0x00,1,	{0xA0}},                                                                                                                                                                     
    {0xCC,15,   {0x10, 0x00, 0x0E, 0x00, 0x0C, 0x00, 0x0A, 0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},//TCON GOA WAVE(Panel timing state control)                                      
    {0x00,1,	{0xB0}},                                                                                                                                                                     
    {0xCC,10,   {0x25, 0x26, 0x21, 0x22, 0x00, 0x0A, 0x00, 0x0C, 0x00, 0x0E}},//TCON GOA WAVE(Panel timing state control)                                                                    
    {0x00,1,	{0xC0}},                                                                                                                                                                     
    {0xCC,15,   {0x00, 0x10, 0x04, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x25, 0x26, 0x23, 0x24, 0x00}},//TCON GOA WAVE(Panel timing state control)                                      
    {0x00,1,	{0xD0}},                                                                                                                                                                     
    {0xCC,15,   {0x09, 0x00, 0x0B, 0x00, 0x0D, 0x00, 0x0F, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},//TCON GOA WAVE(Panel timing state control)                                      
    {0x00,1,	{0x80}},                                                                                                                                                                     
    {0xCE,12,   {0x8A, 0x03, 0x06, 0x89, 0x03, 0x06, 0x88, 0x03, 0x06, 0x87, 0x03, 0x06}},                                                                                                   
    {0x00,1,	{0x90}},                                                                                                                                                                     
    {0xCE,14,   {0xF0, 0x00, 0x00, 0xF0, 0x00, 0x00, 0xF0, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x00}},//GOA VEND and Group Setting                                                           
    {0x00,1,	{0xA0}},                                                                                                                                                                     
    {0xCE,14,   {0x38, 0x02, 0x03, 0xC1, 0x00, 0x06, 0x00, 0x38, 0x01, 0x03, 0xC2, 0x00, 0x06, 0x00}},//GOA CLK1 and GOA CLK2 Setting                                                        
    {0x00,1,	{0xB0}},                                                                                                                                                                     
    {0xCE,14,   {0x38, 0x00, 0x03, 0xC3, 0x00, 0x06, 0x00, 0x30, 0x00, 0x03, 0xC4, 0x00, 0x06, 0x00}},//GOA CLK3 and GOA CLK4 Setting                                                        
    {0x00,1,	{0xC0}},                                                                                                                                                                     
    {0xCE,14,   {0x38, 0x06, 0x03, 0xBD, 0x00, 0x06, 0x00, 0x38, 0x05, 0x03, 0xBE, 0x00, 0x06, 0x00}},//GOA CLKB1 and GOA CLKB2 Setting                                                      
    {0x00,1,	{0xD0}},                                                                                                                                                                     
    {0xCE,14,   {0x38, 0x04, 0x03, 0xBF, 0x00, 0x06, 0x00, 0x38, 0x03, 0x03, 0xC0, 0x00, 0x06, 0x00}},//GOA CLKB3 and GOA CLKB4 Setting                                                      
    {0x00,1,	{0x80}},                                                                                                                                                                     
    {0xCF,14,   {0xF0, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00}},//GOA CLKC1 and GOA CLKC2 Setting                                                      
    {0x00,1,	{0x90}},                                                                                                                                                                     
    {0xCF,14,   {0xF0, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00}},//GOA CLKC3 and GOA CLKC4 Setting                                                      
    {0x00,1,	{0xA0}},                                                                                                                                                                     
    {0xCF,14,   {0xF0, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00}},//GOA CLKD1 and GOA CLKD2 Setting                                                      
    {0x00,1,	{0xB0}},                                                                                                                                                                     
    {0xCF,14,   {0xF0, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00}},//GOA CLKD3 and GOA CLKD4 Setting                                                      
    {0x00,1,	{0xC0}},                                                                                                                                                                     
    {0xCF,10,   {0x02, 0x02, 0x20, 0x20, 0x00, 0x00, 0x01, 0x00, 0x00, 0x02}},//GOA ECLK Setting and GOA Other Options1 and GOA Signal Toggle Option Setting                                 
    {0x00,1,	{0x00}},                                                                                                                                                                     
    {0xD8,2,	{/*{0xA7, 0xA7}*/0x67,0x67}},// GVDD=5.2V, NGVDD=-5.2V                                                                                                                       
    {0x35,1,{0}},
    {0x11,0,{}},
    {REGFLAG_DELAY, 120, {}},
    {0x29,0,{}},
    {REGFLAG_DELAY, 5, {}},


	// Note
	// Strongly recommend not to set Sleep out / Display On here. That will cause messed frame to be shown as later the backlight is on.


	// Setting ending by predefined flag
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};


static struct LCM_setting_table lcm_initialization_setting_yushun[] = {

{0x00,1,{0x00}},
{0xFF,3,{0x96,0x08,0x01}},

{0x00,1,{0x80}},
{0xFF,2,{0x96,0x08}},

{0x00,1,{0xB1}},
{0xB0,2,{0x03,0x06}},

{0x00,1,{0x80}},
{0xC4,1,{0x30}},

{0x00,1,{0xB4}},
{0xC0,1,{0x50}},

{0x00,1,{0x88}},
{0xC4,1,{0x40}},

{0x00,1,{0x00}},
{0xA0,1,{0x00}},

{0x00,1,{0x80}},
{0xB3,5,{0x00,0x00,0x00,0x21,0x00}},

{0x00,1,{0x92}},
{0xB3,1,{0x01}},

{0x00,1,{0xC0}},
{0xB3,1,{0x19}},

{0x00,1,{0x80}},
{0xC0,9,{0x00,0x48,0x00,0x10,0x10,0x00,0x47,0x1F,0x1F}},

{0x00,1,{0x92}},
{0xC0,4,{0x00,0x0E,0x00,0x11}},

{0x00,1,{0xA2}},
{0xC0,3,{0x01,0x10,0x00}},

{0x00,1,{0xB3}},
{0xC0,2,{0x00,0x50}},
{0x00,1,{0x81}},
{0xC1,1,{0x66}}, // change fps 55 66 77

{0x00,1,{0xA0}},
{0xC4,8,{0x33,0x09,0x90,0x2B,0x33,0x09,0x90,0x54}},

{0x00,1,{0x80}},
{0xC5,4,{0x08,0x00,0x90,0x11}},

{0x00,1,{0x90}},
{0xC5,7,{0x84,0x76,0x00,0x76,0x33,0x33,0x34}},

{0x00,1,{0xA0}},
{0xC5,7,{0x96,0x76,0x06,0x76,0x33,0x33,0x34}},

{0x00,1,{0xB0}},
{0xC5,2,{0x04,0x08}},

{0x00,1,{0xC1}},
{0xF5,1,{0x94}},

{0x00,1,{0x80}},
{0xC6,1,{0x64}},

{0x00,1,{0xB0}},
{0xC6,5,{0x03,0x10,0x00,0x1F,0x12}},

{0x00,1,{0xE1}},
{0xC0,1,{0x9F}},

{0x00,1,{0x00}},
{0xD0,1,{0x01}},

{0x00,1,{0x00}},
{0xD1,2,{0x01,0x01}},

{0x00,1,{0xB7}},
{0xB0,1,{0x10}},

{0x00,1,{0xC0}},
{0xB0,1,{0x55}},

{0x00,1,{0x80}},
{0xCB,10,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},

{0x00,1,{0x90}},
{0xCB,15,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},

{0x00,1,{0xA0}},
{0xCB,15,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},

{0x00,1,{0xB0}},
{0xCB,10,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},

{0x00,1,{0xC0}},
{0xCB,15,{0x00,0x00,0x00,0x04,0x00,0x00,0x04,0x04,0x00,0x00,0x04,0x04,0x04,0x00,0x00}},

{0x00,1,{0xD0}},
{0xCB,15,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x04,0x04,0x00,0x00,0x04,0x04}},

{0x00,1,{0xE0}},
{0xCB,10,{0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00}},

{0x00,1,{0xF0}},
{0xCB,10,{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}},

{0x00,1,{0x80}},
{0xCC,10,{0x00,0x00,0x00,0x02,0x00,0x00,0x0A,0x0E,0x00,0x00}},

{0x00,1,{0x90}},
{0xCC,15,{0x0C,0x10,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x09}},

{0x00,1,{0xA0}},
{0xCC,15,{0x0D,0x00,0x00,0x0B,0x0F,0x00,0x00,0x00,0x00,0x05,0x00,0x00,0x00,0x00,0x00}},

{0x00,1,{0xB0}},
{0xCC,10,{0x00,0x00,0x00,0x02,0x00,0x00,0x0A,0x0E,0x00,0x00}},

{0x00,1,{0xC0}},
{0xCC,15,{0x0C,0x10,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x09}},

{0x00,1,{0xD0}},
{0xCC,15,{0x0D,0x00,0x00,0x0B,0x0F,0x00,0x00,0x00,0x00,0x05,0x00,0x00,0x00,0x00,0x00}},

{0x00,1,{0x80}},
{0xCE,12,{0x84,0x03,0x18,0x83,0x03,0x18,0x00,0x0F,0x00,0x00,0x0F,0x00}},

{0x00,1,{0x90}},
{0xCE,14,{0x33,0xBF,0x18,0x33,0xC0,0x18,0xF0,0x00,0x00,0xF0,0x00,0x00,0x00,0x00}},

{0x00,1,{0xA0}},
{0xCE,14,{0x38,0x02,0x03,0xC1,0x00,0x18,0x00,0x38,0x01,0x03,0xC2,0x00,0x18,0x00}},

{0x00,1,{0xB0}},
{0xCE,14,{0x38,0x00,0x03,0xC3,0x00,0x18,0x00,0x30,0x00,0x03,0xC4,0x00,0x18,0x00}},

{0x00,1,{0xC0}},
{0xCE,14,{0x30,0x01,0x03,0xC5,0x00,0x18,0x00,0x30,0x02,0x03,0xC6,0x00,0x18,0x00}},

{0x00,1,{0xD0}},
{0xCE,14,{0x30,0x03,0x03,0xC7,0x00,0x18,0x00,0x30,0x04,0x03,0xC8,0x00,0x18,0x00}},

{0x00,1,{0x80}},
{0xCF,14,{0xF0,0x00,0x00,0x10,0x00,0x00,0x00,0xF0,0x00,0x00,0x10,0x00,0x00,0x00}},

{0x00,1,{0x90}},
{0xCF,14,{0xF0,0x00,0x00,0x10,0x00,0x00,0x00,0xF0,0x00,0x00,0x10,0x00,0x00,0x00}},

{0x00,1,{0xA0}},
{0xCF,14,{0xF0,0x00,0x00,0x10,0x00,0x00,0x00,0xF0,0x00,0x00,0x10,0x00,0x00,0x00}},

{0x00,1,{0xB0}},
{0xCF,14,{0xF0,0x00,0x00,0x10,0x00,0x00,0x00,0xF0,0x00,0x00,0x10,0x00,0x00,0x00}},

{0x00,1,{0xC0}},
{0xCF,10,{0x01,0x01,0x20,0x20,0x00,0x00,0x02,0x00,0x00,0x00}},

{0x00,1,{0x80}},
{0xD6,1,{0x00}},

{0x00,1,{0x00}},
{0xD7,1,{0x00}},

{0x00,1,{0x00}},
{0xD8,2,{0x6F,0x6F}},

{0x00,1,{0x00}},
{0xD9,1,{0x68}},//68

{0x00,1,{0x00}},
{0xe1,16,{0x01,0x06,0x0A,0x0C,0x05,0x0B,0x0A,0x08,0x05,0x09,0x11,0x09,0x0E,0x11,0x0C,0x09}},
{0x00,1,{0x00}},
{0xe2,16,{0x01,0x05,0x09,0x0C,0x04,0x0B,0x09,0x08,0x05,0x08,0x10,0x08,0x0E,0x11,0x0C,0x09}},

{0x00,1,{0x00}},
{0xFF,3,{0xFF,0xFF,0xFF}},

{0x35,1,{0}},//enable te
{0x11,0,{}},//go sleep
{REGFLAG_DELAY, 120, {}},
{0x29,0,{}}, //turn lcm
{REGFLAG_DELAY, 5, {}},

};

void push_table(struct LCM_setting_table *table, unsigned int count, unsigned char force_update)
{
	unsigned int i;

	for (i = 0; i < count; i++) {

		unsigned cmd;
		cmd = table[i].cmd;

		switch (cmd) {

			case REGFLAG_DELAY :
				MDELAY(table[i].count);
				break;

			case REGFLAG_END_OF_TABLE :
				break;

			default:
				dsi_set_cmdq_V2(cmd, table[i].count, table[i].para_list, force_update);
		}
	}

}


// ---------------------------------------------------------------------------
//  LCM Driver Implementations
// ---------------------------------------------------------------------------

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
	memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}


static void lcm_get_params(LCM_PARAMS *params)
{
	memset(params, 0, sizeof(LCM_PARAMS));

	params->type   = LCM_TYPE_DSI;

	params->width  = FRAME_WIDTH;
	params->height = FRAME_HEIGHT;

	// enable tearing-free
	params->dbi.te_mode                 = LCM_DBI_TE_MODE_VSYNC_ONLY;
	params->dbi.te_edge_polarity        = LCM_POLARITY_RISING;

#if defined(LCM_DSI_CMD_MODE)
	params->dsi.mode   = CMD_MODE;
#else
	params->dsi.mode   = SYNC_PULSE_VDO_MODE;
#endif

	// DSI
	/* Command mode setting */
	params->dsi.LANE_NUM                = LCM_TWO_LANE;
	//The following defined the fomat for data coming from LCD engine.
	params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
	params->dsi.data_format.trans_seq   = LCM_DSI_TRANS_SEQ_MSB_FIRST;
	params->dsi.data_format.padding     = LCM_DSI_PADDING_ON_LSB;
	params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;

	// Highly depends on LCD driver capability.
	// Not support in MT6573

	params->dsi.DSI_WMEM_CONTI=0x3C;
	params->dsi.DSI_RMEM_CONTI=0x3E;


	params->dsi.packet_size=256;

	// Video mode setting
	params->dsi.intermediat_buffer_num = 2;

	params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;

	params->dsi.vertical_sync_active                = 3;
	params->dsi.vertical_backporch                  = 12;
	params->dsi.vertical_frontporch                 = 2;
	params->dsi.vertical_active_line                = FRAME_HEIGHT;

	params->dsi.horizontal_sync_active              = 10;
	params->dsi.horizontal_backporch                = 50;
	params->dsi.horizontal_frontporch               = 50;
	params->dsi.horizontal_active_pixel             = FRAME_WIDTH;

	// Bit rate calculation
	params->dsi.PLL_CLOCK               = 260;

	params->dsi.clk_lp_per_line_enable = 0;
	params->dsi.esd_check_enable = 1;
	params->dsi.customization_esd_check_enable = 1;
	params->dsi.lcm_esd_check_table[0].cmd          = 0x0A;
	params->dsi.lcm_esd_check_table[0].count        = 1;
	params->dsi.lcm_esd_check_table[0].para_list[0] = 0x9C;
}

static unsigned int lcm_compare_id(void)
{
	int   array[4];
	char  buffer[3];
	char  id0=0;
	char  id1=0;
	char  id2=0;

	SET_RESET_PIN(0);
	MDELAY(200);
	SET_RESET_PIN(1);
	MDELAY(200);

	array[0] = 0x00033700;// read id return two byte,version and id
	dsi_set_cmdq(array, 1, 1);

	read_reg_v2(0xDA,buffer, 1);

	array[0] = 0x00033700;// read id return two byte,version and id
	dsi_set_cmdq(array, 1, 1);
	read_reg_v2(0xDB,buffer+1, 1);


	array[0] = 0x00033700;// read id return two byte,version and id
	dsi_set_cmdq(array, 1, 1);
	read_reg_v2(0xDC,buffer+2, 1);

	id0 = buffer[0]; //should be 0x00
	id1 = buffer[1];//should be 0xaa
	id2 = buffer[2];//should be 0x55

	dprintf(INFO, "%s, id0 = 0x%08x\n", __func__, id0);//should be 0x00
	dprintf(INFO, "%s, id1 = 0x%08x\n", __func__, id1);//should be 0xaa
	dprintf(INFO, "%s, id2 = 0x%08x\n", __func__, id2);//should be 0x55

	if ((id0 == 0x40) && (id1 == 0x00)) {
		dprintf(INFO, "disp lcm compare id success\n");
		return 1;
	} else {
		dprintf(INFO, "disp lcm compare id failed\n");
		return 0;
	}
}

static void lcm_init(void)
{
    mt_set_gpio_mode(GPIO_LCM_ID, GPIO_LCM_ID_M_GPIO);
    mt_set_gpio_dir(GPIO_LCM_ID, GPIO_DIR_IN);
	SET_RESET_PIN(0);
	MDELAY(10);
	SET_RESET_PIN(1);
	MDELAY(10);

	if(mt_get_gpio_in(GPIO_LCM_ID)==1)
        push_table(lcm_initialization_setting_yushun, sizeof(lcm_initialization_setting_yushun)/sizeof(struct LCM_setting_table), 1); 
    else
        push_table(lcm_initialization_setting_xinli, sizeof(lcm_initialization_setting_xinli)/sizeof(struct LCM_setting_table), 1);
}



static void lcm_suspend(void)
{
	unsigned int data_array[2];

	data_array[0] = 0x00280500; // Display Off
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(10);
	data_array[0] = 0x00100500; // Sleep In
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(100);

	dprintf(INFO, "lk %s\n", __func__);
}


static void lcm_resume(void)
{
	dprintf(INFO, "lk %s\n", __func__);

//	push_table(lcm_sleep_out_setting, sizeof(lcm_sleep_out_setting) / sizeof(struct LCM_setting_table), 1);
	lcm_init();
}


static void lcm_update(unsigned int x, unsigned int y,
                       unsigned int width, unsigned int height)
{
	unsigned int x0 = x;
	unsigned int y0 = y;
	unsigned int x1 = x0 + width - 1;
	unsigned int y1 = y0 + height - 1;

	unsigned char x0_MSB = ((x0>>8)&0xFF);
	unsigned char x0_LSB = (x0&0xFF);
	unsigned char x1_MSB = ((x1>>8)&0xFF);
	unsigned char x1_LSB = (x1&0xFF);
	unsigned char y0_MSB = ((y0>>8)&0xFF);
	unsigned char y0_LSB = (y0&0xFF);
	unsigned char y1_MSB = ((y1>>8)&0xFF);
	unsigned char y1_LSB = (y1&0xFF);

	unsigned int data_array[16];

	dprintf(INFO, "lk %s\n", __func__);

	data_array[0]= 0x00053902;
	data_array[1]= (x1_MSB<<24)|(x0_LSB<<16)|(x0_MSB<<8)|0x2a;
	data_array[2]= (x1_LSB);
	data_array[3]= 0x00053902;
	data_array[4]= (y1_MSB<<24)|(y0_LSB<<16)|(y0_MSB<<8)|0x2b;
	data_array[5]= (y1_LSB);
	data_array[6]= 0x002c3909;

	dsi_set_cmdq(data_array, 7, 0);
}



LCM_DRIVER otm9608_qhd_dsi_cmd_drv = {
	.name       = "otm9608a_qhd_dsi_cmd",
	.set_util_funcs = lcm_set_util_funcs,
	.compare_id     = lcm_compare_id,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
#if defined(LCM_DSI_CMD_MODE)
	.update         = lcm_update,
#endif
};


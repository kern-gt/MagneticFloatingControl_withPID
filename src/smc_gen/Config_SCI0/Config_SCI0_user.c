/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products.
* No other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIESREGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED
* OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY
* LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE FOR ANY DIRECT,
* INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR
* ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability 
* of this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2016, 2017 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name    : Config_SCI0_user.c
* Version      : 1.0.0
* Device(s)    : R5F564MLDxFP
* Description  : This file implements device driver for Config_SCI0.
* Creation Date: 2017-10-12
***********************************************************************************************************************/

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "Config_SCI0.h"
/* Start user code for include. Do not edit comment generated here */
#include "oled.h"
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
extern uint8_t   g_sci0_iic_transmit_receive_flag;  /* SCI0 transmit receive flag for I2C */
extern uint8_t   g_sci0_iic_cycle_flag;             /* SCI0 start stop flag for I2C */
extern uint8_t   g_sci0_slave_address;              /* SCI0 target slave address */
extern uint8_t * gp_sci0_tx_address;                /* SCI0 send buffer address */
extern uint16_t  g_sci0_tx_count;                   /* SCI0 send data number */
extern uint8_t * gp_sci0_rx_address;                /* SCI0 receive buffer address */
extern uint16_t  g_sci0_rx_count;                   /* SCI0 receive data number */
extern uint16_t  g_sci0_rx_length;                  /* SCI0 receive data length */
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_Config_SCI0_Create_UserInit
* Description  : This function adds user code after initializing the SCI0 channel
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_SCI0_Create_UserInit(void)
{
    /* Start user code for user init. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_Config_SCI0_transmit_interrupt
* Description  : This function is TXI0 interrupt service routine
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

#if FAST_INTERRUPT_VECTOR == VECT_SCI0_TXI0
#pragma interrupt r_Config_SCI0_transmit_interrupt(vect=VECT(SCI0,TXI0),fint)
#else
#pragma interrupt r_Config_SCI0_transmit_interrupt(vect=VECT(SCI0,TXI0))
#endif
static void r_Config_SCI0_transmit_interrupt(void)
{
    /* Receive last data in master receive mode (ACK/NACK) */
    if (1U == SCI0.SISR.BIT.IICACKR)
    {
        *gp_sci0_rx_address = SCI0.RDR;
        gp_sci0_rx_address++;
        g_sci0_rx_count++;
    }

    /* Generate stop condition */
    g_sci0_iic_cycle_flag = _00_SCI_IIC_STOP_CYCLE;
    R_Config_SCI0_IIC_StopCondition();
}

/***********************************************************************************************************************
* Function Name: r_Config_SCI0_transmitend_interrupt
* Description  : This function is TEI0 interrupt service routine
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void r_Config_SCI0_transmitend_interrupt(void)
{
    if (_80_SCI_IIC_START_CYCLE == g_sci0_iic_cycle_flag)
    {
        SCI0.SIMR3.BIT.IICSTIF = 0U;
        SCI0.SIMR3.BIT.IICSCLS = 0U;
        SCI0.SIMR3.BIT.IICSDAS = 0U;
        SCI0.TDR = g_sci0_slave_address;
    }
    else if (_00_SCI_IIC_STOP_CYCLE == g_sci0_iic_cycle_flag)
    {
        SCI0.SIMR3.BIT.IICSTIF = 0U;
        SCI0.SIMR3.BYTE |= _30_SCI_SSDA_HIGH_IMPEDANCE | _C0_SCI_SSCL_HIGH_IMPEDANCE;
        if (_80_SCI_IIC_TRANSMISSION == g_sci0_iic_transmit_receive_flag)
        {
            r_Config_SCI0_callback_transmitend();
        }
        if (_00_SCI_IIC_RECEPTION == g_sci0_iic_transmit_receive_flag)
        {
            r_Config_SCI0_callback_receiveend();
        }
    }
    else
    {
        /* Do nothing */
    }
}

/***********************************************************************************************************************
* Function Name: r_Config_SCI0_receive_interrupt
* Description  : This function is RXI0 interrupt service routine
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

#if FAST_INTERRUPT_VECTOR == VECT_SCI0_RXI0
#pragma interrupt r_Config_SCI0_receive_interrupt(vect=VECT(SCI0,RXI0),fint)
#else
#pragma interrupt r_Config_SCI0_receive_interrupt(vect=VECT(SCI0,RXI0))
#endif
static void r_Config_SCI0_receive_interrupt(void)
{
    volatile uint8_t dummy;

    if (0U == SCI0.SISR.BIT.IICACKR)
    {
        if (_80_SCI_IIC_TRANSMISSION == g_sci0_iic_transmit_receive_flag)
        {
            if (g_sci0_tx_count > 0U)
            {
                SCI0.TDR = *gp_sci0_tx_address;
                gp_sci0_tx_address++;
                g_sci0_tx_count--;
            }
            else
            {
                /* Generate stop condition */
                g_sci0_iic_cycle_flag = _00_SCI_IIC_STOP_CYCLE;
                R_Config_SCI0_IIC_StopCondition();
            }
        }
        else if (_00_SCI_IIC_RECEPTION == g_sci0_iic_transmit_receive_flag)
        {
            if (0U == SCI0.SIMR2.BIT.IICACKT)
            {
                if (g_sci0_rx_length > g_sci0_rx_count)
                {
                    *gp_sci0_rx_address = SCI0.RDR;
                    gp_sci0_rx_address++;
                    g_sci0_rx_count++;
                }
            }
            else
            {
                dummy = SCI0.RDR;
            }

            if (0U == g_sci0_rx_count)
            {
                if(1U == g_sci0_rx_length)
                {
                    SCI0.SIMR2.BIT.IICACKT = 1U;
                }
                else
                {
                    SCI0.SIMR2.BIT.IICACKT = 0U; 
                    SCI0.SCR.BIT.RIE = 1U;
                }
            }
            else if (g_sci0_rx_length == (g_sci0_rx_count + 1))
            {
                 SCI0.SIMR2.BIT.IICACKT = 1U;
            }
            else
            {
                /* Do nothing */
            }

            /* Write dummy */
            SCI0.TDR = 0xFFU;
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        /* Generate stop condition */
        g_sci0_iic_cycle_flag = _00_SCI_IIC_STOP_CYCLE;
        R_Config_SCI0_IIC_StopCondition();
    }
}

/***********************************************************************************************************************
* Function Name: r_Config_SCI0_callback_transmitend
* Description  : This function is a callback function when SCI0 finishes transmission
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

static void r_Config_SCI0_callback_transmitend(void)
{
    /* Start user code for r_Config_SCI0_callback_transmitend. Do not edit comment generated here */
    OledDataSendFinish();
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_Config_SCI0_callback_receiveend
* Description  : This function is a callback function when SCI0 finishes reception
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

static void r_Config_SCI0_callback_receiveend(void)
{
    /* Start user code for r_Config_SCI0_callback_receiveend. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */   





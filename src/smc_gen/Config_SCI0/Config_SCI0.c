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
* File Name    : Config_SCI0.c
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
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
uint8_t   g_sci0_iic_transmit_receive_flag; /* SCI0 transmit receive flag for I2C */
uint8_t   g_sci0_iic_cycle_flag;            /* SCI0 start stop flag for I2C */
uint8_t   g_sci0_slave_address;             /* SCI0 target slave address */
uint8_t * gp_sci0_tx_address;               /* SCI0 transmit buffer address */
uint16_t  g_sci0_tx_count;                  /* SCI0 transmit data number */
uint8_t * gp_sci0_rx_address;               /* SCI0 receive buffer address */
uint16_t  g_sci0_rx_count;                  /* SCI0 receive data number */
uint16_t  g_sci0_rx_length;                 /* SCI0 receive data length */
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_Config_SCI0_Create
* Description  : This function initializes the SCI0 channel
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_SCI0_Create(void)
{
    /* Cancel SCI stop state */
    MSTP(SCI0) = 0U;

    /* Set interrupt priority */
    IPR(SCI0, RXI0) = _02_SCI_PRIORITY_LEVEL2;
    IPR(SCI0, TXI0) = _02_SCI_PRIORITY_LEVEL2;

    /* Clear the control register */
    SCI0.SCR.BYTE = 0x00U;

    /* Initialize SSCL and SSDA pins to high impedance */
    SCI0.SIMR3.BYTE = _C0_SCI_SSCL_HIGH_IMPEDANCE | _30_SCI_SSDA_HIGH_IMPEDANCE;

    /* Set up transfer or reception format in SMR and SCMR */
    SCI0.SMR.BYTE = _00_SCI_CLOCK_PCLK | _00_SCI_ASYNCHRONOUS_OR_I2C_MODE;
    SCI0.SCMR.BIT.SMIF = 0U;
    SCI0.SCMR.BIT.SINV = 0U;
    SCI0.SCMR.BIT.SDIR = 1U;

    /* Set bit rate */
    SCI0.BRR = 0x02U;
    SCI0.MDDR = 0xA4U;
    SCI0.SEMR.BYTE = _00_SCI_NOISE_FILTER_DISABLE | _04_SCI_BIT_MODULATION_ENABLE;
    SCI0.SIMR1.BYTE |= _01_SCI_IIC_MODE | _00_SCI_NONE;
    SCI0.SIMR2.BYTE |= _00_SCI_ACK_NACK_INTERRUPTS | _02_SCI_SYNCHRONIZATION | _20_SCI_NACK_TRANSMISSION;
    SCI0.SPMR.BYTE = _00_SCI_CLOCK_NOT_INVERTED | _00_SCI_CLOCK_NOT_DELAYED;
    SCI0.SCR.BYTE = _10_SCI_RECEIVE_ENABLE | _20_SCI_TRANSMIT_ENABLE | _40_SCI_RXI_ERI_ENABLE | _80_SCI_TXI_ENABLE | 
                    _04_SCI_TEI_INTERRUPT_ENABLE;

    /* Set SSCL0 pin */
    MPC.P21PFS.BYTE = 0x0AU;
    PORT2.ODR0.BYTE |= 0x04U;
    PORT2.PMR.BYTE |= 0x02U;

    /* Set SSDA0 pin */
    MPC.P20PFS.BYTE = 0x0AU;
    PORT2.ODR0.BYTE |= 0x01U;
    PORT2.PMR.BYTE |= 0x01U;
    
    R_Config_SCI0_Create_UserInit();
}

/***********************************************************************************************************************
* Function Name: R_Config_SCI0_Start
* Description  : This function starts the SCI0 channel
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_SCI0_Start(void)
{
    /* Clear interrupt flag */
    IR(SCI0,TXI0) = 0U;
    IR(SCI0,RXI0) = 0U;

    /* Enable SCI interrupt */
    IEN(SCI0,TXI0) = 1U;
    ICU.GENBL0.BIT.EN0 = 1U;
    IEN(SCI0,RXI0) = 1U;
    ICU.GENBL0.BIT.EN1 = 1U;
}

/***********************************************************************************************************************
* Function Name: R_Config_SCI0_Stop
* Description  : This function stop the SCI0 channel
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_SCI0_Stop(void)
{
    IR(SCI0,TXI0) = 0U;
    IEN(SCI0,TXI0) = 0U;
    ICU.GENBL0.BIT.EN0 = 0U;
    IR(SCI0,RXI0) = 0U;
    IEN(SCI0,RXI0) = 0U;
    ICU.GENBL0.BIT.EN1 = 0U;
}

/***********************************************************************************************************************
* Function Name: R_Config_SCI0_IIC_StartCondition
* Description  : This function generate IIC start condition
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_SCI0_IIC_StartCondition(void)
{
    SCI0.SIMR3.BYTE = _01_SCI_START_CONDITION_ON | _10_SCI_SSDA_START_RESTART_STOP_CONDITION | 
                      _40_SCI_SSCL_START_RESTART_STOP_CONDITION;
}

/***********************************************************************************************************************
* Function Name: R_Config_SCI0_IIC_StopCondition
* Description  : This function generate IIC stop condition
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_SCI0_IIC_StopCondition(void)
{
    SCI0.SIMR3.BYTE = _04_SCI_STOP_CONDITION_ON | _10_SCI_SSDA_START_RESTART_STOP_CONDITION | 
                      _40_SCI_SSCL_START_RESTART_STOP_CONDITION;
}

/***********************************************************************************************************************
* Function Name: R_Config_SCI0_IIC_Master_Send
* Description  : This function sends simple IIC(SCI0) data to slave device
* Arguments    : adr -
*                    slave device address
*                tx_buf -
*                    transfer buffer pointer (Not used when receive data handled by DTC or DMAC)
*                tx_num -
*                    buffer size (Not used when receive data handled by DTC or DMAC)
* Return Value : None
***********************************************************************************************************************/

void R_Config_SCI0_IIC_Master_Send(uint8_t adr, uint8_t * const tx_buf, uint16_t tx_num)
{
    if (tx_num < 1U)
    {
        return;
    }

    gp_sci0_tx_address = tx_buf;
    g_sci0_tx_count = tx_num;
    g_sci0_slave_address = adr;
    g_sci0_iic_transmit_receive_flag = _80_SCI_IIC_TRANSMISSION;
    g_sci0_iic_cycle_flag = _80_SCI_IIC_START_CYCLE;

    /* Generate start condition */
    R_Config_SCI0_IIC_StartCondition();
}

/***********************************************************************************************************************
* Function Name: R_Config_SCI0_IIC_Master_Receive
* Description  : This function receive simple IIC(SCI0) data from slave device
* Arguments    : adr -
*                    slave device address
*                rx_buf -
*                    receive buffer pointer (Not used when receive data handled by DTC or DMAC)
*                rx_num -
*                    buffer size (Not used when receive data handled by DTC or DMAC)
* Return Value : None
***********************************************************************************************************************/

void R_Config_SCI0_IIC_Master_Receive(uint8_t adr, uint8_t * const rx_buf, uint16_t rx_num)
{
    if (rx_num < 1U)
    {
        return;
    }

    g_sci0_rx_length = rx_num;
    g_sci0_rx_count = 0;
    gp_sci0_rx_address = rx_buf;
    g_sci0_slave_address = adr;
    g_sci0_iic_transmit_receive_flag = _00_SCI_IIC_RECEPTION;
    g_sci0_iic_cycle_flag = _80_SCI_IIC_START_CYCLE;

    /* Generate start condition */
    R_Config_SCI0_IIC_StartCondition();
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */   

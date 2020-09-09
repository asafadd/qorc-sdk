/*==========================================================
 * Copyright 2020 QuickLogic Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *==========================================================*/

/*==========================================================
 *
 *    File   : main.c
 *    Purpose: main for advancedfpga example using ledctlr.v
 *                                                          
 *=========================================================*/

#include "Fw_global_config.h"
#include "Bootconfig.h"

#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "timers.h"
#include "RtosTask.h"

/*    Include the generic headers required for sensorHub */
#include "eoss3_hal_gpio.h"
#include "eoss3_hal_rtc.h"

#include "ql_time.h"
#include "s3x_clock_hal.h"
#include "s3x_clock.h"
#include "s3x_pi.h"
#include "dbg_uart.h"
#include "eoss3_hal_spi.h"
#include "eoss3_hal_uart.h"
#include "cli.h"
#include "tinytester.h"

extern const struct cli_cmd_entry my_main_menu[];

#include "fpga_loader.h"        // API for loading FPGA
#include "AL4S3B_FPGA_top_bit.h"   // FPGA bitstream to load into FPGA


const char *SOFTWARE_VERSION_STR;

tinytester_channelconfig_t atinytester_channelconfig[] = {
    output, false, false, true, false,  // RZ (clock)
    output, true,  true,  true,  true,  // NRZ
    input,  false, false, false, false, // input
};
uint32_t kchannelconfig = sizeof(atinytester_channelconfig)/sizeof(atinytester_channelconfig[0]);

uint32_t auxOutputvector[] = {
    2, 0, 3, 1, 3
};
uint32_t kvectorOutput = sizeof(auxOutputvector)/sizeof(auxOutputvector[0]);

uint32_t auxInputvector[5];
uint32_t kvectorInput = sizeof(auxInputvector)/sizeof(auxInputvector[0]);


/*
 * Global variable definition
 */


extern void qf_hardwareSetup();
static void nvic_init(void);

int main(void)
{

    SOFTWARE_VERSION_STR = "qorc-sdk/qf_testapps/qf_tinytester";
    
    qf_hardwareSetuptinytester();                                     // Note: pincfg_table.c has been updated to give FPGA control of LEDs
    
    nvic_init();
    S3x_Clk_Disable(S3X_FB_21_CLK);
    S3x_Clk_Disable(S3X_FB_16_CLK);
    S3x_Clk_Enable(S3X_A1_CLK);
    S3x_Clk_Enable(S3X_CFG_DMA_A1_CLK);
    load_fpga(sizeof(axFPGABitStream),axFPGABitStream);     // Load bitstrem into FPGA
    tinytester_init();		                                  // Start FPGA clock
    
    dbg_str("\n\n");
    dbg_str( "##########################\n");
    dbg_str( "Quicklogic QuickFeather Tinytester\n");
    dbg_str( "SW Version: ");
    dbg_str( SOFTWARE_VERSION_STR );
    dbg_str( "\n" );
    dbg_str( __DATE__ " " __TIME__ "\n" );
    dbg_str( "##########################\n\n");
             
    CLI_start_task( my_main_menu );
    
    /* Start the tasks and timer running. */
    vTaskStartScheduler();
    dbg_str("\n");
      
    while(1);

}

static void nvic_init(void)
 {
    // To initialize system, this interrupt should be triggered at main.
    // So, we will set its priority just before calling vTaskStartScheduler(), not the time of enabling each irq.
    NVIC_SetPriority(Ffe0_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(SpiMs_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(CfgDma_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(Uart_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(FbMsg_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
 }    

//needed for startup_EOSS3b.s asm file
void SystemInit(void)
{

}



/**
 ******************************************************************************
 *
 * @file       board_hw_defs.c.h
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2013.
 * @brief      Defines board hardware for the OpenPilot OPLink Mini board.
 * @see        The GNU Public License (GPL) Version 3
 *
 *****************************************************************************/
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */


#if defined(PIOS_INCLUDE_LED)

#include <pios_led_priv.h>
static const struct pios_led pios_leds[] = {
    [PIOS_LED_USB] =  {
        .pin                =        {
            .gpio = GPIOA,
            .init =        {
                .GPIO_Pin   = GPIO_Pin_3,
                .GPIO_Mode  = GPIO_Mode_Out_PP,
                .GPIO_Speed = GPIO_Speed_50MHz,
            },
        },
    },
    [PIOS_LED_LINK] = {
        .pin                =        {
            .gpio = GPIOB,
            .init =        {
                .GPIO_Pin   = GPIO_Pin_5,
                .GPIO_Mode  = GPIO_Mode_Out_PP,
                .GPIO_Speed = GPIO_Speed_50MHz,
            },
        },
    },
    [PIOS_LED_RX] =   {
        .pin                =        {
            .gpio = GPIOB,
            .init =        {
                .GPIO_Pin   = GPIO_Pin_6,
                .GPIO_Mode  = GPIO_Mode_Out_PP,
                .GPIO_Speed = GPIO_Speed_50MHz,
            },
        },
    },
    [PIOS_LED_TX] =   {
        .pin                =        {
            .gpio = GPIOB,
            .init =        {
                .GPIO_Pin   = GPIO_Pin_7,
                .GPIO_Mode  = GPIO_Mode_Out_PP,
                .GPIO_Speed = GPIO_Speed_50MHz,
            },
        },
    },
#ifdef PIOS_RFM22B_DEBUG_ON_TELEM
    [PIOS_LED_D1] =   {
        .pin                =        {
            .gpio = GPIOB,
            .init =        {
                .GPIO_Pin   = GPIO_Pin_14,
                .GPIO_Mode  = GPIO_Mode_Out_PP,
                .GPIO_Speed = GPIO_Speed_50MHz,
            },
        },
    },
    [PIOS_LED_D2] =   {
        .pin                =        {
            .gpio = GPIOB,
            .init =        {
                .GPIO_Pin   = GPIO_Pin_15,
                .GPIO_Mode  = GPIO_Mode_Out_PP,
                .GPIO_Speed = GPIO_Speed_50MHz,
            },
        },
    },
    [PIOS_LED_D3] =   {
        .pin                =        {
            .gpio = GPIOA,
            .init =        {
                .GPIO_Pin   = GPIO_Pin_9,
                .GPIO_Mode  = GPIO_Mode_Out_PP,
                .GPIO_Speed = GPIO_Speed_50MHz,
            },
        },
    },
    [PIOS_LED_D4] =   {
        .pin                =        {
            .gpio = GPIOA,
            .init =        {
                .GPIO_Pin   = GPIO_Pin_10,
                .GPIO_Mode  = GPIO_Mode_Out_PP,
                .GPIO_Speed = GPIO_Speed_50MHz,
            },
        },
    },
#endif /* ifdef PIOS_RFM22B_DEBUG_ON_TELEM */
};

static const struct pios_led_cfg pios_led_cfg = {
    .leds     = pios_leds,
    .num_leds = NELEMENTS(pios_leds),
};

const struct pios_led_cfg *PIOS_BOARD_HW_DEFS_GetLedCfg(__attribute__((unused)) uint32_t board_revision)
{
    return &pios_led_cfg;
}

#endif /* PIOS_INCLUDE_LED */

#if defined(PIOS_INCLUDE_FLASH)
#include "pios_flashfs_logfs_priv.h"
#include "pios_flash_internal_priv.h"

static const struct pios_flash_internal_cfg flash_internal_cfg = {};

static const struct flashfs_logfs_cfg flashfs_internal_cfg = {
    .fs_magic      = 0x99abcfef,
    .total_fs_size = EE_BANK_SIZE, /* 2K bytes (2x1KB sectors) */
    .arena_size    = 0x00002000, /* 4 * slot size = 1K bytes = 1 sector */
    .slot_size     = 0x00000100, /* 256 bytes */

    .start_offset  = EE_BANK_BASE, /* start after the bootloader */
    .sector_size   = 0x00000400, /* 1K bytes */
    .page_size     = 0x00000400, /* 1K bytes */
};

#include "pios_flash.h"

#endif /* PIOS_INCLUDE_FLASH */


#if defined(PIOS_INCLUDE_SPI)

#include <pios_spi_priv.h>

/* OP Interface
 *
 * NOTE: Leave this declared as const data so that it ends up in the
 * .rodata section (ie. Flash) rather than in the .bss section (RAM).
 */
void PIOS_SPI_port_irq_handler(void);
void DMA1_Channel5_IRQHandler() __attribute__((alias("PIOS_SPI_port_irq_handler")));
void DMA1_Channel4_IRQHandler() __attribute__((alias("PIOS_SPI_port_irq_handler")));

static const struct pios_spi_cfg pios_spi_rfm22b_cfg = {
    .regs = SPI1,

    .init = {
        .SPI_Mode              = SPI_Mode_Master,
        .SPI_Direction         = SPI_Direction_2Lines_FullDuplex,
        .SPI_DataSize          = SPI_DataSize_8b,
        .SPI_NSS                                   = SPI_NSS_Soft,
        .SPI_FirstBit          = SPI_FirstBit_MSB,
        .SPI_CRCPolynomial     = 0,
        .SPI_CPOL              = SPI_CPOL_Low,
        .SPI_CPHA              = SPI_CPHA_1Edge,
        .SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16,              // slowest SCLK
    },
    .use_crc = FALSE,

    .dma     = {
        .ahb_clk = RCC_AHBPeriph_DMA1,
        .irq     = {
            .flags = (DMA1_FLAG_TC2 | DMA1_FLAG_TE2 | DMA1_FLAG_HT2 | DMA1_FLAG_GL2),
            .init  = {
                .NVIC_IRQChannel    = DMA1_Channel2_IRQn,
                .NVIC_IRQChannelPreemptionPriority = PIOS_IRQ_PRIO_HIGH,
                .NVIC_IRQChannelSubPriority        = 0,
                .NVIC_IRQChannelCmd = ENABLE,
            },
        },

        .rx                                        = {
            .channel = DMA1_Channel2,
            .init    = {
                .DMA_PeripheralBaseAddr            = (uint32_t)&(SPI1->DR),
                .DMA_DIR                           = DMA_DIR_PeripheralSRC,
                .DMA_PeripheralInc      = DMA_PeripheralInc_Disable,
                .DMA_MemoryInc          = DMA_MemoryInc_Enable,
                .DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte,
                .DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte,
                .DMA_Mode     = DMA_Mode_Normal,
                .DMA_Priority = DMA_Priority_Medium,
                .DMA_M2M                           = DMA_M2M_Disable,
            },
        },
        .tx                                        = {
            .channel = DMA1_Channel3,
            .init    = {
                .DMA_PeripheralBaseAddr            = (uint32_t)&(SPI1->DR),
                .DMA_DIR                           = DMA_DIR_PeripheralDST,
                .DMA_PeripheralInc      = DMA_PeripheralInc_Disable,
                .DMA_MemoryInc          = DMA_MemoryInc_Enable,
                .DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte,
                .DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte,
                .DMA_Mode     = DMA_Mode_Normal,
                .DMA_Priority = DMA_Priority_Medium,
                .DMA_M2M                           = DMA_M2M_Disable,
            },
        },
    },
    .slave_count                                   = 1,
    .ssel                                          = {
        {
            .gpio = GPIOA,
            .init = {
                .GPIO_Pin   = GPIO_Pin_4,
                .GPIO_Speed = GPIO_Speed_10MHz,
                .GPIO_Mode  = GPIO_Mode_Out_PP,
            },
        }
    },
    .sclk                                          = {
        .gpio = GPIOA,
        .init = {
            .GPIO_Pin   = GPIO_Pin_5,
            .GPIO_Speed = GPIO_Speed_10MHz,
            .GPIO_Mode  = GPIO_Mode_AF_PP,
        },
    },
    .miso                                          = {
        .gpio = GPIOA,
        .init = {
            .GPIO_Pin   = GPIO_Pin_6,
            .GPIO_Speed = GPIO_Speed_10MHz,
            .GPIO_Mode  = GPIO_Mode_IN_FLOATING,
        },
    },
    .mosi                                          = {
        .gpio = GPIOA,
        .init = {
            .GPIO_Pin   = GPIO_Pin_7,
            .GPIO_Speed = GPIO_Speed_10MHz,
            .GPIO_Mode  = GPIO_Mode_AF_PP,
        },
    },
};

uint32_t pios_spi_rfm22b_id;
void PIOS_SPI_port_irq_handler(void)
{
    /* Call into the generic code to handle the IRQ for this specific device */
    PIOS_SPI_IRQ_Handler(pios_spi_rfm22b_id);
}

#endif /* PIOS_INCLUDE_SPI */

#if defined(PIOS_INCLUDE_RFM22B)

#include <pios_rfm22b_priv.h>

static const struct pios_exti_cfg pios_exti_rfm22b_cfg __exti_config = {
    .vector = PIOS_RFM22_EXT_Int,
    .line   = EXTI_Line2,
    .pin    = {
        .gpio = GPIOA,
        .init = {
            .GPIO_Pin  = GPIO_Pin_2,
            .GPIO_Mode = GPIO_Mode_IN_FLOATING,
        },
    },
    .irq                                       = {
        .init                                  = {
            .NVIC_IRQChannel    = EXTI2_IRQn,
            .NVIC_IRQChannelPreemptionPriority = PIOS_IRQ_PRIO_LOW,
            .NVIC_IRQChannelSubPriority        = 0,
            .NVIC_IRQChannelCmd = ENABLE,
        },
    },
    .exti                                      = {
        .init                                  = {
            .EXTI_Line    = EXTI_Line2,
            .EXTI_Mode    = EXTI_Mode_Interrupt,
            .EXTI_Trigger = EXTI_Trigger_Falling,
            .EXTI_LineCmd = ENABLE,
        },
    },
};

#include <pios_rfm22b_priv.h>

struct pios_rfm22b_cfg pios_rfm22b_cfg = {
    .spi_cfg   = &pios_spi_rfm22b_cfg,
    .exti_cfg  = &pios_exti_rfm22b_cfg,
    .RFXtalCap = 0x7f,
    .slave_num = 0,
    .gpio_direction = GPIO0_TX_GPIO1_RX,
};

// ! Compatibility layer for various hardware revisions
const struct pios_rfm22b_cfg *PIOS_BOARD_HW_DEFS_GetRfm22Cfg(__attribute__((unused)) uint32_t board_revision)
{
    return &pios_rfm22b_cfg;
}

#endif /* PIOS_INCLUDE_RFM22B */

#if defined(PIOS_INCLUDE_ADC)

/*
 * ADC system
 */
#include "pios_adc_priv.h"
extern void PIOS_ADC_handler(void);
void DMA1_Channel1_IRQHandler() __attribute__((alias("PIOS_ADC_handler")));
// Remap the ADC DMA handler to this one
static const struct pios_adc_cfg pios_adc_cfg = {
    .dma                                           = {
        .ahb_clk = RCC_AHBPeriph_DMA1,
        .irq     = {
            .flags = (DMA1_FLAG_TC1 | DMA1_FLAG_TE1 | DMA1_FLAG_HT1 | DMA1_FLAG_GL1),
            .init  = {
                .NVIC_IRQChannel    = DMA1_Channel1_IRQn,
                .NVIC_IRQChannelPreemptionPriority = PIOS_IRQ_PRIO_HIGH,
                .NVIC_IRQChannelSubPriority        = 0,
                .NVIC_IRQChannelCmd = ENABLE,
            },
        },
        .rx                                        = {
            .channel = DMA1_Channel1,
            .init    = {
                .DMA_PeripheralBaseAddr            = (uint32_t)&ADC1->DR,
                .DMA_DIR                           = DMA_DIR_PeripheralSRC,
                .DMA_PeripheralInc      = DMA_PeripheralInc_Disable,
                .DMA_MemoryInc          = DMA_MemoryInc_Enable,
                .DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word,
                .DMA_MemoryDataSize     = DMA_MemoryDataSize_Word,
                .DMA_Mode     = DMA_Mode_Circular,
                .DMA_Priority = DMA_Priority_High,
                .DMA_M2M                           = DMA_M2M_Disable,
            },
        }
    },
    .half_flag = DMA1_IT_HT1,
    .full_flag = DMA1_IT_TC1,
};

struct pios_adc_dev pios_adc_devs[] = {
    {
        .cfg = &pios_adc_cfg,
        .callback_function = NULL,
    },
};

uint8_t pios_adc_num_devices = NELEMENTS(pios_adc_devs);

void PIOS_ADC_handler()
{
    PIOS_ADC_DMA_Handler();
}

#endif /* PIOS_INCLUDE_ADC */

#if defined(PIOS_INCLUDE_TIM)

#include "pios_tim_priv.h"

static const TIM_TimeBaseInitTypeDef tim_1_2_3_4_time_base = {
    .TIM_Prescaler         = (PIOS_MASTER_CLOCK / 1000000) - 1,
    .TIM_ClockDivision     = TIM_CKD_DIV1,
    .TIM_CounterMode       = TIM_CounterMode_Up,
    .TIM_Period            = ((1000000 / PIOS_SERVO_UPDATE_HZ) - 1),
    .TIM_RepetitionCounter = 0x0000,
};

static const struct pios_tim_clock_cfg tim_1_cfg = {
    .timer = TIM1,
    .time_base_init                            = &tim_1_2_3_4_time_base,
    .irq   = {
        .init                                  = {
            .NVIC_IRQChannel    = TIM1_CC_IRQn,
            .NVIC_IRQChannelPreemptionPriority = PIOS_IRQ_PRIO_MID,
            .NVIC_IRQChannelSubPriority        = 0,
            .NVIC_IRQChannelCmd = ENABLE,
        },
    },
};

static const struct pios_tim_clock_cfg tim_2_cfg = {
    .timer = TIM2,
    .time_base_init                            = &tim_1_2_3_4_time_base,
    .irq   = {
        .init                                  = {
            .NVIC_IRQChannel    = TIM2_IRQn,
            .NVIC_IRQChannelPreemptionPriority = PIOS_IRQ_PRIO_MID,
            .NVIC_IRQChannelSubPriority        = 0,
            .NVIC_IRQChannelCmd = ENABLE,
        },
    },
};

static const struct pios_tim_clock_cfg tim_3_cfg = {
    .timer = TIM3,
    .time_base_init                            = &tim_1_2_3_4_time_base,
    .irq   = {
        .init                                  = {
            .NVIC_IRQChannel    = TIM3_IRQn,
            .NVIC_IRQChannelPreemptionPriority = PIOS_IRQ_PRIO_MID,
            .NVIC_IRQChannelSubPriority        = 0,
            .NVIC_IRQChannelCmd = ENABLE,
        },
    },
};

static const struct pios_tim_clock_cfg tim_4_cfg = {
    .timer = TIM4,
    .time_base_init                            = &tim_1_2_3_4_time_base,
    .irq   = {
        .init                                  = {
            .NVIC_IRQChannel    = TIM4_IRQn,
            .NVIC_IRQChannelPreemptionPriority = PIOS_IRQ_PRIO_MID,
            .NVIC_IRQChannelSubPriority        = 0,
            .NVIC_IRQChannelCmd = ENABLE,
        },
    },
};

static const struct pios_tim_channel pios_tim_all_port_pins[] = {
    /* Main Tx */
    {
        .timer = TIM1,
        .timer_chan = TIM_Channel_2,
        .pin   = {
            .gpio = GPIOA,
            .init = {
                .GPIO_Pin   = GPIO_Pin_9,
                .GPIO_Mode  = GPIO_Mode_AF_PP,
                .GPIO_Speed = GPIO_Speed_2MHz,
            },
        },
    },
    /* Main Rx */
    {
        .timer = TIM1,
        .timer_chan = TIM_Channel_3,
        .pin   = {
            .gpio = GPIOA,
            .init = {
                .GPIO_Pin   = GPIO_Pin_10,
                .GPIO_Mode  = GPIO_Mode_AF_PP,
                .GPIO_Speed = GPIO_Speed_2MHz,
            },
        },
    },
    /* Flexi Tx */
    {
        .timer = TIM2,
        .timer_chan = TIM_Channel_3,
        .pin   = {
            .gpio = GPIOB,
            .init = {
                .GPIO_Pin   = GPIO_Pin_10,
                .GPIO_Mode  = GPIO_Mode_AF_PP,
                .GPIO_Speed = GPIO_Speed_2MHz,
            },
        },
        .remap = GPIO_PartialRemap2_TIM2,
    },
    /* Flexi Rx */
    {
        .timer = TIM2,
        .timer_chan = TIM_Channel_4,
        .pin   = {
            .gpio = GPIOB,
            .init = {
                .GPIO_Pin   = GPIO_Pin_11,
                .GPIO_Mode  = GPIO_Mode_AF_PP,
                .GPIO_Speed = GPIO_Speed_2MHz,
            },
        },
        .remap = GPIO_PartialRemap2_TIM2,
    },
};

static const struct pios_tim_channel pios_tim_main_port_ppm = {
    .timer = TIM1,
    .timer_chan = TIM_Channel_3,
    .pin   = {
        .gpio = GPIOA,
        .init = {
            .GPIO_Pin   = GPIO_Pin_10,
            .GPIO_Mode  = GPIO_Mode_IPD,
            .GPIO_Speed = GPIO_Speed_2MHz,
        },
    },
};

static const struct pios_tim_channel pios_tim_flexi_port_ppm = {
    .timer = TIM2,
    .timer_chan = TIM_Channel_4,
    .pin   = {
        .gpio = GPIOB,
        .init = {
            .GPIO_Pin   = GPIO_Pin_11,
            .GPIO_Mode  = GPIO_Mode_IPD,
            .GPIO_Speed = GPIO_Speed_2MHz,
        },
    },
    .remap = GPIO_PartialRemap2_TIM2,
};

#endif /* PIOS_INCLUDE_TIM */

#if defined(PIOS_INCLUDE_SERVO) && defined(PIOS_INCLUDE_TIM)
/*
 * Servo outputs
 */
#include <pios_servo_priv.h>

const struct pios_servo_cfg pios_servo_main_cfg = {
    .tim_oc_init          = {
        .TIM_OCMode       = TIM_OCMode_PWM1,
        .TIM_OutputState  = TIM_OutputState_Enable,
        .TIM_OutputNState = TIM_OutputNState_Disable,
        .TIM_Pulse        = PIOS_SERVOS_INITIAL_POSITION,
        .TIM_OCPolarity   = TIM_OCPolarity_High,
        .TIM_OCNPolarity  = TIM_OCPolarity_High,
        .TIM_OCIdleState  = TIM_OCIdleState_Reset,
        .TIM_OCNIdleState = TIM_OCNIdleState_Reset,
    },
    .channels     = pios_tim_all_port_pins,
    .num_channels = 2
};

const struct pios_servo_cfg pios_servo_flexi_cfg = {
    .tim_oc_init          = {
        .TIM_OCMode       = TIM_OCMode_PWM1,
        .TIM_OutputState  = TIM_OutputState_Enable,
        .TIM_OutputNState = TIM_OutputNState_Disable,
        .TIM_Pulse        = PIOS_SERVOS_INITIAL_POSITION,
        .TIM_OCPolarity   = TIM_OCPolarity_High,
        .TIM_OCNPolarity  = TIM_OCPolarity_High,
        .TIM_OCIdleState  = TIM_OCIdleState_Reset,
        .TIM_OCNIdleState = TIM_OCNIdleState_Reset,
    },
    .channels     = &(pios_tim_all_port_pins[2]),
    .num_channels = 2
};

const struct pios_servo_cfg pios_servo_main_flexi_cfg = {
    .tim_oc_init          = {
        .TIM_OCMode       = TIM_OCMode_PWM1,
        .TIM_OutputState  = TIM_OutputState_Enable,
        .TIM_OutputNState = TIM_OutputNState_Disable,
        .TIM_Pulse        = PIOS_SERVOS_INITIAL_POSITION,
        .TIM_OCPolarity   = TIM_OCPolarity_High,
        .TIM_OCNPolarity  = TIM_OCPolarity_High,
        .TIM_OCIdleState  = TIM_OCIdleState_Reset,
        .TIM_OCNIdleState = TIM_OCNIdleState_Reset,
    },
    .channels     = pios_tim_all_port_pins,
    .num_channels = 4
};

#endif /* PIOS_INCLUDE_SERVO && PIOS_INCLUDE_TIM */

/*
 * PPM Inputs
 */
#if defined(PIOS_INCLUDE_PPM)
#include <pios_ppm_priv.h>

const struct pios_ppm_cfg pios_ppm_main_cfg = {
    .tim_ic_init         = {
        .TIM_ICPolarity  = TIM_ICPolarity_Rising,
        .TIM_ICSelection = TIM_ICSelection_DirectTI,
        .TIM_ICPrescaler = TIM_ICPSC_DIV1,
        .TIM_ICFilter    = 0x0,
    },
    .channels     = &pios_tim_main_port_ppm,
    .num_channels = 1,
};

const struct pios_ppm_cfg pios_ppm_flexi_cfg = {
    .tim_ic_init         = {
        .TIM_ICPolarity  = TIM_ICPolarity_Rising,
        .TIM_ICSelection = TIM_ICSelection_DirectTI,
        .TIM_ICPrescaler = TIM_ICPSC_DIV1,
        .TIM_ICFilter    = 0x0,
    },
    .channels     = &pios_tim_flexi_port_ppm,
    .num_channels = 1,
};

#endif /* PIOS_INCLUDE_PPM */

/*
 * PPM Output
 */
#if defined(PIOS_INCLUDE_PPM_OUT)
#include <pios_ppm_out_priv.h>

const struct pios_ppm_out_cfg pios_main_ppm_out_cfg = {
    .tim_oc_init          = {
        .TIM_OCMode       = TIM_OCMode_PWM1,
        .TIM_OutputState  = TIM_OutputState_Enable,
        .TIM_OutputNState = TIM_OutputNState_Disable,
        .TIM_Pulse        = PIOS_SERVOS_INITIAL_POSITION,
        .TIM_OCPolarity   = TIM_OCPolarity_Low,
        .TIM_OCNPolarity  = TIM_OCPolarity_Low,
        .TIM_OCIdleState  = TIM_OCIdleState_Reset,
        .TIM_OCNIdleState = TIM_OCNIdleState_Reset,
    },
    .channel              = &(pios_tim_all_port_pins[0]),
};

const struct pios_ppm_out_cfg pios_flexi_ppm_out_cfg = {
    .tim_oc_init          = {
        .TIM_OCMode       = TIM_OCMode_PWM1,
        .TIM_OutputState  = TIM_OutputState_Enable,
        .TIM_OutputNState = TIM_OutputNState_Disable,
        .TIM_Pulse        = PIOS_SERVOS_INITIAL_POSITION,
        .TIM_OCPolarity   = TIM_OCPolarity_Low,
        .TIM_OCNPolarity  = TIM_OCPolarity_Low,
        .TIM_OCIdleState  = TIM_OCIdleState_Reset,
        .TIM_OCNIdleState = TIM_OCNIdleState_Reset,
    },
    .channel              = &(pios_tim_all_port_pins[2]),
};

#endif /* PIOS_INCLUDE_PPM_OUT */

#if defined(PIOS_INCLUDE_USART)

#include <pios_usart_priv.h>

/*
 * SERIAL USART
 */
static const struct pios_usart_cfg pios_usart_serial_cfg = {
    .regs = USART1,
    .init = {
        .USART_BaudRate   = 57600,
        .USART_WordLength = USART_WordLength_8b,
        .USART_Parity     = USART_Parity_No,
        .USART_StopBits   = USART_StopBits_1,
        .USART_HardwareFlowControl             = USART_HardwareFlowControl_None,
        .USART_Mode                            = USART_Mode_Rx | USART_Mode_Tx,
    },
    .irq                                       = {
        .init                                  = {
            .NVIC_IRQChannel    = USART1_IRQn,
            .NVIC_IRQChannelPreemptionPriority = PIOS_IRQ_PRIO_HIGH,
            .NVIC_IRQChannelSubPriority        = 0,
            .NVIC_IRQChannelCmd = ENABLE,
        },
    },
    .rx                                        = {
        .gpio = GPIOA,
        .init = {
            .GPIO_Pin   = GPIO_Pin_10,
            .GPIO_Speed = GPIO_Speed_2MHz,
            .GPIO_Mode  = GPIO_Mode_IPU,
        },
    },
    .tx                                        = {
        .gpio = GPIOA,
        .init = {
            .GPIO_Pin   = GPIO_Pin_9,
            .GPIO_Speed = GPIO_Speed_2MHz,
            .GPIO_Mode  = GPIO_Mode_AF_PP,
        },
    },
};

static const struct pios_usart_cfg pios_usart_telem_flexi_cfg = {
    .regs = USART3,
    .init = {
        .USART_BaudRate   = 57600,
        .USART_WordLength = USART_WordLength_8b,
        .USART_Parity     = USART_Parity_No,
        .USART_StopBits   = USART_StopBits_1,
        .USART_HardwareFlowControl             = USART_HardwareFlowControl_None,
        .USART_Mode                            = USART_Mode_Rx | USART_Mode_Tx,
    },
    .irq                                       = {
        .init                                  = {
            .NVIC_IRQChannel    = USART3_IRQn,
            .NVIC_IRQChannelPreemptionPriority = PIOS_IRQ_PRIO_MID,
            .NVIC_IRQChannelSubPriority        = 0,
            .NVIC_IRQChannelCmd = ENABLE,
        },
    },
    .rx                                        = {
        .gpio = GPIOB,
        .init = {
            .GPIO_Pin   = GPIO_Pin_11,
            .GPIO_Speed = GPIO_Speed_2MHz,
            .GPIO_Mode  = GPIO_Mode_IPU,
        },
    },
    .tx                                        = {
        .gpio = GPIOB,
        .init = {
            .GPIO_Pin   = GPIO_Pin_10,
            .GPIO_Speed = GPIO_Speed_2MHz,
            .GPIO_Mode  = GPIO_Mode_AF_PP,
        },
    },
};

#endif /* PIOS_INCLUDE_USART */

#if defined(PIOS_INCLUDE_COM)

#include <pios_com_priv.h>

#endif /* PIOS_INCLUDE_COM */

#if defined(PIOS_INCLUDE_RTC)
/*
 * Realtime Clock (RTC)
 */
#include <pios_rtc_priv.h>

void PIOS_RTC_IRQ_Handler(void);
void RTC_IRQHandler() __attribute__((alias("PIOS_RTC_IRQ_Handler")));
static const struct pios_rtc_cfg pios_rtc_main_cfg = {
    .clksrc    = RCC_RTCCLKSource_HSE_Div128,
    .prescaler = 100,
    .irq                                       = {
        .init                                  = {
            .NVIC_IRQChannel    = RTC_IRQn,
            .NVIC_IRQChannelPreemptionPriority = PIOS_IRQ_PRIO_MID,
            .NVIC_IRQChannelSubPriority        = 0,
            .NVIC_IRQChannelCmd = ENABLE,
        },
    },
};

void PIOS_RTC_IRQ_Handler(void)
{
    PIOS_RTC_irq_handler();
}

#endif /* if defined(PIOS_INCLUDE_RTC) */

#if defined(PIOS_INCLUDE_RCVR)
#include "pios_rcvr_priv.h"

#endif /* PIOS_INCLUDE_RCVR */

#if defined(PIOS_INCLUDE_USB)
#include "pios_usb_priv.h"

static const struct pios_usb_cfg pios_usb_main_cfg = {
    .irq                                       = {
        .init                                  = {
            .NVIC_IRQChannel    = USB_LP_CAN1_RX0_IRQn,
            .NVIC_IRQChannelPreemptionPriority = PIOS_IRQ_PRIO_HIGH,
            .NVIC_IRQChannelSubPriority        = 0,
            .NVIC_IRQChannelCmd = ENABLE,
        },
    },
    .vsense                                    = {
        .gpio = GPIOA,
        .init = {
            .GPIO_Pin   = GPIO_Pin_8,
            .GPIO_Speed = GPIO_Speed_10MHz,
            .GPIO_Mode  = GPIO_Mode_AF_OD,
        },
    },
    .vsense_active_low                         = false
};

#include "pios_usb_board_data_priv.h"
#include "pios_usb_desc_hid_cdc_priv.h"
#include "pios_usb_desc_hid_only_priv.h"

#endif /* PIOS_INCLUDE_USB */

#if defined(PIOS_INCLUDE_COM_MSG)

#include <pios_com_msg_priv.h>

#endif /* PIOS_INCLUDE_COM_MSG */

#if defined(PIOS_INCLUDE_USB_HID)
#include <pios_usb_hid_priv.h>

const struct pios_usb_hid_cfg pios_usb_hid_cfg = {
    .data_if    = 2,
    .data_rx_ep = 1,
    .data_tx_ep = 1,
};
#endif /* PIOS_INCLUDE_USB_HID */

#if defined(PIOS_INCLUDE_USB_CDC)
#include <pios_usb_cdc_priv.h>

const struct pios_usb_cdc_cfg pios_usb_cdc_cfg = {
    .ctrl_if    = 0,
    .ctrl_tx_ep = 2,

    .data_if    = 1,
    .data_rx_ep = 3,
    .data_tx_ep = 3,
};
#endif /* PIOS_INCLUDE_USB_CDC */

#if defined(PIOS_INCLUDE_FLASH_EEPROM)
#include <pios_eeprom.h>

const struct pios_eeprom_cfg pios_eeprom_cfg = {
    .base_address = PIOS_FLASH_EEPROM_ADDR,
    .max_size     = PIOS_FLASH_EEPROM_LEN,
};
#endif /* PIOS_INCLUDE_FLASH_EEPROM */

#if defined(PIOS_INCLUDE_RFM22B)
#include <pios_rfm22b_priv.h>

#endif /* PIOS_INCLUDE_RFM22B */

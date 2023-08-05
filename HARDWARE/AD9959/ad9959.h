#ifndef AD9959_AD9959_H
#define AD9959_AD9959_H

#include "stdbool.h"
#include "gpio.h"

//PORTS&PINS
#define	AD9959_CS_GPIO_Port			GPIOG
#define	AD9959_CS_Pin				GPIO_PIN_13

#define	AD9959_SCLK_GPIO_Port		GPIOC
#define	AD9959_SCLK_Pin				GPIO_PIN_13
	
#define	AD9959_UPDATE_GPIO_Port	    GPIOG
#define	AD9959_UPDATE_Pin			GPIO_PIN_14

#define	AD9959_PS0_GPIO_Port		GPIOC
#define	AD9959_PS0_Pin				GPIO_PIN_12

#define	AD9959_PS1_GPIO_Port		GPIOD
#define	AD9959_PS1_Pin				GPIO_PIN_2

#define	AD9959_PS2_GPIO_Port		GPIOD
#define	AD9959_PS2_Pin				GPIO_PIN_3

#define	AD9959_PS3_GPIO_Port		GPIOG
#define	AD9959_PS3_Pin				GPIO_PIN_10

#define	AD9959_SDIO0_GPIO_Port	    GPIOE
#define	AD9959_SDIO0_Pin			GPIO_PIN_2

#define	AD9959_SDIO1_GPIO_Port	    GPIOE
#define	AD9959_SDIO1_Pin			GPIO_PIN_3

#define	AD9959_SDIO2_GPIO_Port	    GPIOE
#define	AD9959_SDIO2_Pin			GPIO_PIN_4

#define	AD9959_SDIO3_GPIO_Port	    GPIOE
#define	AD9959_SDIO3_Pin			GPIO_PIN_5

#define	AD9959_RESET_GPIO_Port	    GPIOE
#define	AD9959_RESET_Pin			GPIO_PIN_6



#ifdef STM32F407xx
#include "sys.h"
#define AD9959_SDIO0    PAout(2)
#define AD9959_SDIO0_1  AD9959_SDIO0 = 1
#define AD9959_SDIO0_0  AD9959_SDIO0 = 0


#define AD9959_SDIO1    PAout(5)
#define AD9959_SDIO1_1  AD9959_SDIO1 = 1
#define AD9959_SDIO1_0  AD9959_SDIO1 = 0

#define AD9959_SDIO2    PAout(4)
#define AD9959_SDIO2_1  AD9959_SDIO2 = 1
#define AD9959_SDIO2_0  AD9959_SDIO0 = 0

#define AD9959_SDIO3    PAout(7)
#define AD9959_SDIO3_1  AD9959_SDIO3 = 1
#define AD9959_SDIO3_0  AD9959_SDIO3 = 0

#define AD9959_P0       PCout(0)
#define AD9959_P0_1     AD9959_P0 = 1
#define AD9959_P0_0     AD9959_P0 = 0

#define AD9959_P1       PCout(1)
#define AD9959_P1_1     AD9959_P1 = 1
#define AD9959_P1_0     AD9959_P1 = 0

#define AD9959_P2       PCout(2)
#define AD9959_P2_1     AD9959_P2 = 1
#define AD9959_P2_0     AD9959_P2 = 0

#define AD9959_P3       PCout(3)
#define AD9959_P3_1     AD9959_P3 = 1
#define AD9959_P3_0     AD9959_P3 = 0

#define AD9959_UP       PAout(1)
#define AD9959_UP_1     AD9959_UP = 1
#define AD9959_UP_0     AD9959_UP = 0

#define AD9959_CS       PAout(0)
#define AD9959_CS_1     AD9959_CS = 1
#define AD9959_CS_0     AD9959_CS = 0


#define AD9959_SCK      PAout(3)
#define AD9959_SCK_1    AD9959_SCK = 1
#define AD9959_SCK_0    AD9959_SCK = 0

#define AD9959_RST      PCout(5)
#define AD9959_RST_1    AD9959_RST = 1
#define AD9959_RST_0    AD9959_RST = 0

#else

#define AD9959_CS_1 		HAL_GPIO_WritePin(AD9959_CS_GPIO_Port,AD9959_CS_Pin,GPIO_PIN_SET)
#define AD9959_SCLK_1 		HAL_GPIO_WritePin(AD9959_SCLK_GPIO_Port,AD9959_SCLK_Pin,GPIO_PIN_SET)
#define AD9959_UPDATE_1 	HAL_GPIO_WritePin(AD9959_UPDATE_GPIO_Port,AD9959_UPDATE_Pin,GPIO_PIN_SET)
#define AD9959_PS0_1        HAL_GPIO_WritePin(AD9959_PS0_GPIO_Port,AD9959_PS0_Pin,GPIO_PIN_SET)
#define AD9959_PS1_1        HAL_GPIO_WritePin(AD9959_PS1_GPIO_Port,AD9959_PS1_Pin,GPIO_PIN_SET)
#define AD9959_PS2_1        HAL_GPIO_WritePin(AD9959_PS2_GPIO_Port,AD9959_PS2_Pin,GPIO_PIN_SET)
#define AD9959_PS3_1        HAL_GPIO_WritePin(AD9959_PS3_GPIO_Port,AD9959_PS3_Pin,GPIO_PIN_SET)
#define AD9959_SDIO0_1      HAL_GPIO_WritePin(AD9959_SDIO0_GPIO_Port,AD9959_SDIO0_Pin,GPIO_PIN_SET)
#define AD9959_SDIO1_1      HAL_GPIO_WritePin(AD9959_SDIO1_GPIO_Port,AD9959_SDIO1_Pin,GPIO_PIN_SET)
#define AD9959_SDIO2_1      HAL_GPIO_WritePin(AD9959_SDIO2_GPIO_Port,AD9959_SDIO2_Pin,GPIO_PIN_SET)
#define AD9959_SDIO3_1      HAL_GPIO_WritePin(AD9959_SDIO3_GPIO_Port,AD9959_SDIO3_Pin,GPIO_PIN_SET)
//#define AD9959_PDC_1        HAL_GPIO_WritePin(AD9959_PDC_GPIO_Port,AD9959_PDC_Pin,GPIO_PIN_SET)
#define AD9959_RESET_1      HAL_GPIO_WritePin(AD9959_RESET_GPIO_Port,AD9959_RESET_Pin,GPIO_PIN_SET)

#define AD9959_CS_0         HAL_GPIO_WritePin(AD9959_CS_GPIO_Port,AD9959_CS_Pin,GPIO_PIN_RESET)
#define AD9959_SCLK_0       HAL_GPIO_WritePin(AD9959_SCLK_GPIO_Port,AD9959_SCLK_Pin,GPIO_PIN_RESET)
#define AD9959_UPDATE_0     HAL_GPIO_WritePin(AD9959_UPDATE_GPIO_Port,AD9959_UPDATE_Pin,GPIO_PIN_RESET)
#define AD9959_PS0_0        HAL_GPIO_WritePin(AD9959_PS0_GPIO_Port,AD9959_PS0_Pin,GPIO_PIN_RESET)
#define AD9959_PS1_0        HAL_GPIO_WritePin(AD9959_PS1_GPIO_Port,AD9959_PS1_Pin,GPIO_PIN_RESET)
#define AD9959_PS2_0        HAL_GPIO_WritePin(AD9959_PS2_GPIO_Port,AD9959_PS2_Pin,GPIO_PIN_RESET)
#define AD9959_PS3_0        HAL_GPIO_WritePin(AD9959_PS3_GPIO_Port,AD9959_PS3_Pin,GPIO_PIN_RESET)
#define AD9959_SDIO0_0      HAL_GPIO_WritePin(AD9959_SDIO0_GPIO_Port,AD9959_SDIO0_Pin,GPIO_PIN_RESET)
#define AD9959_SDIO1_0      HAL_GPIO_WritePin(AD9959_SDIO1_GPIO_Port,AD9959_SDIO1_Pin,GPIO_PIN_RESET)
#define AD9959_SDIO2_0      HAL_GPIO_WritePin(AD9959_SDIO2_GPIO_Port,AD9959_SDIO2_Pin,GPIO_PIN_RESET)
#define AD9959_SDIO3_0      HAL_GPIO_WritePin(AD9959_SDIO3_GPIO_Port,AD9959_SDIO3_Pin,GPIO_PIN_RESET)
//#define AD9959_PDC_0        HAL_GPIO_WritePin(AD9959_PDC_GPIO_Port,AD9959_PDC_Pin,GPIO_PIN_RESET)
#define AD9959_RESET_0      HAL_GPIO_WritePin(AD9959_RESET_GPIO_Port,AD9959_RESET_Pin,GPIO_PIN_RESET)

#endif

typedef enum {
    AD9959_CHANNEL_0 = 0x10U,
    AD9959_CHANNEL_1 = 0x20U,
    AD9959_CHANNEL_2 = 0x40U,
    AD9959_CHANNEL_3 = 0x80U,
} AD9959_CHANNEL;

typedef enum {
    AD9959_REG_CSR       =   0x00U,  //CSR 通道选择寄存器
    AD9959_REG_FR1       =   0x01U,  //FR1 功能寄存器1
    AD9959_REG_FR2       =   0x02U,  //FR2 功能寄存器2
    AD9959_REG_CFR       =   0x03U,  //CFR 通道功能寄存器
    AD9959_REG_CFTW0     =   0x04U,  //CTW0 通道频率转换字寄存器
    AD9959_REG_CPOW0     =   0x05U,  //CPW0 通道相位转换字寄存器
    AD9959_REG_ACR       =   0x06U,  //ACR 幅度控制寄存器
    AD9959_REG_LSRR      =   0x07U,  //LSR 通道线性扫描寄存器
    AD9959_REG_RDW       =   0x08U,  //RDW 通道线性向上扫描寄存器
    AD9959_REG_FDW       =   0x09U,  //FDW 通道线性向下扫描寄存器
} AD9959_REG_ADDR;


#define IS_AD9959_REG_ADDR(REG_ADDR)    (((REG_ADDR) == AD9959_REG_CSR  ) || \
                                         ((REG_ADDR) == AD9959_REG_FR1  ) || \
                                         ((REG_ADDR) == AD9959_REG_FR2  ) || \
                                         ((REG_ADDR) == AD9959_REG_CFR  ) || \
                                         ((REG_ADDR) == AD9959_REG_CFTW0) || \
                                         ((REG_ADDR) == AD9959_REG_CPOW0) || \
                                         ((REG_ADDR) == AD9959_REG_ACR  ) || \
                                         ((REG_ADDR) == AD9959_REG_LSRR ) || \
                                         ((REG_ADDR) == AD9959_REG_RDW  ) || \
                                         ((REG_ADDR) == AD9959_REG_FDW  ))

#define IS_AD9959_CHANNEL(CHANNEL)      (((CHANNEL) == AD9959_CHANNEL_0 ) || \
                                         ((CHANNEL) == AD9959_CHANNEL_1 ) || \
                                         ((CHANNEL) == AD9959_CHANNEL_2 ) || \
                                         ((CHANNEL) == AD9959_CHANNEL_3 ))

void ad9959_init(void);
void ad9959_reset(void);
void ad9959_io_init(void);
void ad9959_io_update(void);
void ad9959_write_data(AD9959_REG_ADDR register_address, uint8_t number_of_registers, const uint8_t *register_data, bool update);
void ad9959_write_phase(AD9959_CHANNEL channel, uint16_t phase);
void ad9959_write_frequency(AD9959_CHANNEL channel, uint32_t frequency);
void ad9959_write_amplitude(AD9959_CHANNEL channel, uint16_t amplitude);

#endif //AD9959_AD9959_H

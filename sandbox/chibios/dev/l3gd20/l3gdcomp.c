/**
  ******************************************************************************
  * @file    stm32f3_discovery_l3gd20.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    20-September-2012
  * @brief   This file provides a set of functions needed to manage the l3gd20
  *          MEMS accelerometer available on STM32F3-Discovery Kit.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/

#include  "ch.h"
#include  "hal.h"

#include "board.h"
#include "stm32f3_discovery_l3gd20.h"
#include "stm32f30x_gpio.h"
#include "l3gdcomp.h"

//#include "stm32f30x_spi.h"

static uint8_t 	L3GD20_SendByte(uint8_t byte);

/*****************************************************************************
  External global Variables :
*****************************************************************************/
extern SPIDriver	gSpiDriver;
extern SPIConfig        gSpiConfig;  

/*****************************************************************************
  TypeDef 				
*****************************************************************************/

#define SPI_Mode_Master              ((uint16_t)0x0104)
#define SPI_Mode_Slave               ((uint16_t)0x0000)

#define CR1_CLEAR_MASK       	     ((uint16_t)0x3040)

#define SPI_I2S_FLAG_RXNE            ((uint16_t)0x0001)
#define SPI_I2S_FLAG_TXE             ((uint16_t)0x0002)

#define VAR_OR(x)   (x.SPI_Direction | x.SPI_Mode | x.SPI_CPOL | x.SPI_CPHA  \
                  | x.SPI_NSS | x.SPI_BaudRatePrescaler | x.SPI_FirstBit) 
/*
typedef struct
{
  uint16_t 		SPI_Direction;           
  uint16_t 		SPI_Mode;
  uint16_t 		SPI_DataSize;
  uint16_t 		SPI_CPOL; 
  uint16_t 		SPI_CPHA;    
  uint16_t 		SPI_NSS; 
  uint16_t 		SPI_BaudRatePrescaler;
  uint16_t 		SPI_FirstBit;            
  uint16_t 		SPI_CRCPolynomial;
}SPI_InitTypeDef_A;
*/

/**
  * @brief  Initializes the low level interface used to drive the L3GD20
  * @param  None
  * @retval None
  */

/** 
  * @brief Serial Peripheral Interface
  */

extern void RCC_APB2PeriphClockCmd(u32 RCC_APB2Periph, FunctionalState NewState);

/**
  * @brief  Reads a block of data from the L3GD20.
  * @param  pBuffer : pointer to the buffer that receives the data read from the L3GD20.
  * @param  ReadAddr : L3GD20's internal address to read from.
  * @param  NumByteToRead : number of bytes to read from the L3GD20.
  * @retval None
  */


///////////////////////////////////////////////////////////////////////////
// 
//  Static Procedure Declaration
//

static uint8_t L3GD20_SendByte(uint8_t byte);

/**
  * @brief  Checks whether the specified SPI flag is set or not.
  * @param  SPIx: To select the SPIx/I2Sx peripheral, where x can be: 1, 2 or 3 
  *         in SPI mode or 2 or 3 in I2S mode or I2Sxext for I2S full duplex mode.  
  * @param  SPI_I2S_FLAG: specifies the SPI flag to check. 
  *   This parameter can be one of the following values:
  *     @arg SPI_I2S_FLAG_TXE: Transmit buffer empty flag.
  *     @arg SPI_I2S_FLAG_RXNE: Receive buffer not empty flag.
  *     @arg SPI_I2S_FLAG_BSY: Busy flag.
  *     @arg SPI_I2S_FLAG_OVR: Overrun flag.
  *     @arg SPI_I2S_FLAG_MODF: Mode Fault flag.
  *     @arg SPI_I2S_FLAG_CRCERR: CRC Error flag.
  *     @arg SPI_I2S_FLAG_FRE: TI frame format error flag.
  *     @arg I2S_FLAG_UDR: Underrun Error flag.
  *     @arg I2S_FLAG_CHSIDE: Channel Side flag.   
  * @retval The new state of SPI_I2S_FLAG (SET or RESET).
  */
FlagStatus SPI_I2S_GetFlagStatus(SPI_TypeDef* SPIx, uint16_t SPI_I2S_FLAG)
{
  FlagStatus bitstatus = RESET;

  /* Check the status of the specified SPI flag */
  if ((SPIx->SR & SPI_I2S_FLAG) != (uint16_t)RESET)
  {  /* SPI_I2S_FLAG is set */
    bitstatus = SET;
  }
  else
  {  /* SPI_I2S_FLAG is reset */
    bitstatus = RESET;
  }

  /* Return the SPI_I2S_FLAG status */
  return  bitstatus;
}


static void SPI_Cmd(SPI_TypeDef* SPIx, FunctionalState NewState, SPIConfig *scfg)
{
  /* Check the parameters */

  if (NewState != DISABLE)
  {
    /* Enable the selected SPI peripheral */
    SPIx->CR1 |= SPI_CR1_SPE;
  }
  else
  {
    /* Disable the selected SPI peripheral */
    SPIx->CR1 &= (uint16_t)~((uint16_t)SPI_CR1_SPE);
  }

  scfg->cr1 = SPIx->CR1; 
}


void L3GD20_Read(uint8_t* pBuffer, uint8_t *ReadAddr, uint16_t NumByteToRead)
{  
  if(NumByteToRead > 0x01)
  {
    *ReadAddr |= (uint8_t)(READWRITE_CMD | MULTIPLEBYTE_CMD);
  }
  else
  {
    *ReadAddr |= (uint8_t)READWRITE_CMD;
  }
  /* Set chip select Low at the start of the transmission */
  L3GD20_CS_LOW();

  /* Send the Address of the indexed register */
  L3GD20_SendByte(*ReadAddr);
  
  /* Receive the data that will be read from the device (MSB First) */
  while(NumByteToRead > 0x00)
  {
    /* Send dummy byte (0x00) to generate the SPI clock to L3GD20 (Slave device) */
    *pBuffer = L3GD20_SendByte(DUMMY_BYTE);
    NumByteToRead--;
    pBuffer++;
  }
  
  /* Set chip select High at the end of the transmission */ 
  L3GD20_CS_HIGH();
}  

/**
  * @brief  Reboot memory content of L3GD20
  * @param  None
  * @retval None
  */
void L3GD20_RebootCmd(void)
{
  uint8_t tmpreg, tmpval;

 tmpval = L3GD20_CTRL_REG5_ADDR;
  
  /* Read CTRL_REG5 register */
  L3GD20_Read(&tmpreg, &tmpval, 1);
  
  /* Enable or Disable the reboot memory */
 
  tmpreg |= L3GD20_BOOT_REBOOTMEMORY;

 tmpval = L3GD20_CTRL_REG5_ADDR;   
  /* Write value to MEMS CTRL_REG5 regsister */
  L3GD20_Write(&tmpreg, &tmpval, 1);
}


/**
  * @brief Set L3GD20 Interrupt configuration
  * @param  L3GD20_InterruptConfig_TypeDef: pointer to a L3GD20_InterruptConfig_TypeDef 
  *         structure that contains the configuration setting for the L3GD20 Interrupt.
  * @retval None
  */
void L3GD20_INT1InterruptConfig(L3GD20_InterruptConfigTypeDef *L3GD20_IntConfigStruct)
{
  uint8_t ctrl_cfr = 0x00, ctrl3 = 0x00;
  uint8_t tmpval; 

  tmpval = L3GD20_INT1_CFG_ADDR;
  /* Read INT1_CFG register */
  L3GD20_Read(&ctrl_cfr, &tmpval, 1);

  tmpval = L3GD20_CTRL_REG3_ADDR;
  /* Read CTRL_REG3 register */
  L3GD20_Read(&ctrl3, &tmpval, 1);
  
  ctrl_cfr &= 0x80;
  
  ctrl3 &= 0xDF;
  
  /* Configure latch Interrupt request and axe interrupts */                   
  ctrl_cfr |= (uint8_t)(L3GD20_IntConfigStruct->Latch_Request| \
                   L3GD20_IntConfigStruct->Interrupt_Axes);
                   
  ctrl3 |= (uint8_t)(L3GD20_IntConfigStruct->Interrupt_ActiveEdge);
  
  /* Write value to MEMS INT1_CFG register */
  tmpval = L3GD20_INT1_CFG_ADDR;
  L3GD20_Write(&ctrl_cfr, &tmpval , 1);
  
  /* Write value to MEMS CTRL_REG3 register */
  tmpval = L3GD20_CTRL_REG3_ADDR;
  L3GD20_Write(&ctrl3, &tmpval, 1);
}


/**
  * @brief  Enable or disable INT1 interrupt
  * @param  InterruptState: State of INT1 Interrupt 
  *      This parameter can be: 
  *        @arg L3GD20_INT1INTERRUPT_DISABLE
  *        @arg L3GD20_INT1INTERRUPT_ENABLE    
  * @retval None
  */
void L3GD20_INT1InterruptCmd(uint8_t InterruptState)
{  
  uint8_t tmpreg, tmpaddr;
  
  /* Read CTRL_REG3 register */
  tmpaddr = L3GD20_CTRL_REG3_ADDR;
  L3GD20_Read(&tmpreg, &tmpaddr, 1);
                  
  tmpreg &= 0x7F;	
  tmpreg |= InterruptState;
  
  /* Write value to MEMS CTRL_REG3 regsister */
  tmpaddr = L3GD20_CTRL_REG3_ADDR;
  L3GD20_Write(&tmpreg, &tmpaddr, 1);
}

/**
  * @brief  Enable or disable INT2 interrupt
  * @param  InterruptState: State of INT1 Interrupt 
  *      This parameter can be: 
  *        @arg L3GD20_INT2INTERRUPT_DISABLE
  *        @arg L3GD20_INT2INTERRUPT_ENABLE    
  * @retval None
  */
void L3GD20_INT2InterruptCmd(uint8_t InterruptState)
{  
  uint8_t tmpreg, tmpval;
  
  /* Read CTRL_REG3 register */
  tmpval = L3GD20_CTRL_REG3_ADDR;
  L3GD20_Read(&tmpreg, &tmpval, 1);
                  
  tmpreg &= 0xF7;	
  tmpreg |= InterruptState;
  
  /* Write value to MEMS CTRL_REG3 regsister */
  tmpval = L3GD20_CTRL_REG3_ADDR;
  L3GD20_Write(&tmpreg, &tmpval, 1);
}

/**
  * @brief  Set High Pass Filter Modality
  * @param  L3GD20_FilterStruct: pointer to a L3GD20_FilterConfigTypeDef structure 
  *         that contains the configuration setting for the L3GD20.        
  * @retval None
  */
void L3GD20_FilterConfig(L3GD20_FilterConfigTypeDef *L3GD20_FilterStruct) 
{
  uint8_t tmpreg, tmpval;
  
  /* Read CTRL_REG2 register */
  tmpval = L3GD20_CTRL_REG2_ADDR;
  L3GD20_Read(&tmpreg, &tmpval, 1);
  
  tmpreg &= 0xC0;
  
  /* Configure MEMS: mode and cutoff frquency */
  tmpreg |= (uint8_t) (L3GD20_FilterStruct->HighPassFilter_Mode_Selection |\
                      L3GD20_FilterStruct->HighPassFilter_CutOff_Frequency);                             

  /* Write value to MEMS CTRL_REG2 regsister */
  tmpval = L3GD20_CTRL_REG2_ADDR;
  L3GD20_Write(&tmpreg, &tmpval, 1);
}

/**
  * @brief  Enable or Disable High Pass Filter
  * @param  HighPassFilterState: new state of the High Pass Filter feature.
  *      This parameter can be: 
  *         @arg: L3GD20_HIGHPASSFILTER_DISABLE 
  *         @arg: L3GD20_HIGHPASSFILTER_ENABLE          
  * @retval None
  */
void L3GD20_FilterCmd(uint8_t HighPassFilterState)
 {
  uint8_t tmpreg, tmpval;
  
  /* Read CTRL_REG5 register */
  tmpval = L3GD20_CTRL_REG5_ADDR;
  L3GD20_Read(&tmpreg, &tmpval, 1);
                  
  tmpreg &= 0xEF;

  tmpreg |= HighPassFilterState;

  /* Write value to MEMS CTRL_REG5 regsister */
  tmpval = L3GD20_CTRL_REG5_ADDR;
  L3GD20_Write(&tmpreg, &tmpval, 1);
}

/**
  * @brief  Get status for L3GD20 data
  * @param  None         
  * @retval Data status in a L3GD20 Data
  */
uint8_t L3GD20_GetDataStatus(void)
{
  uint8_t tmpreg, tmpval;
  
  /* Read STATUS_REG register */
  tmpval = L3GD20_STATUS_REG_ADDR;
  L3GD20_Read(&tmpreg, &tmpval, 1);
                  
  return tmpreg;
}

__IO uint32_t  L3GD20Timeout = L3GD20_FLAG_TIMEOUT;  


extern FlagStatus SPI_I2S_GetFlagStatus(SPI_TypeDef* SPIx, uint16_t SPI_I2S_FLAG);


/**
  * @brief  Sends a Byte through the SPI interface and return the Byte received 
  *         from the SPI bus.
  * @param  Byte : Byte send.
  * @retval The received byte value
  */
static uint8_t L3GD20_SendByte(uint8_t byte)
{
  uint8_t  rxbuf;
  
  /* Loop while DR register in not empty */
  L3GD20Timeout = L3GD20_FLAG_TIMEOUT;
  //return; // check for 
  
  while (SPI_I2S_GetFlagStatus(L3GD20_SPI, SPI_I2S_FLAG_TXE) == RESET)
  {
    if((L3GD20Timeout--) == 0) return L3GD20_TIMEOUT_UserCallback();
  }

  
  spiExchange(&SPID1,1, &byte, &rxbuf); // to need checkig.

  return rxbuf;
}


/**
  * @brief  Initializes the SPIx peripheral according to the specified 
  *         parameters in the SPI_InitStruct.
  * @param  SPIx: where x can be 1, 2 or 3 to select the SPI peripheral.
  * @param  SPI_InitStruct: pointer to a SPI_InitTypeDef structure that
  *         contains the configuration information for the specified SPI peripheral.
  * @retval None
  */

void SPI_Init_A(SPI_TypeDef* SPIx, SPI_InitTypeDef_A* SPI_InitStruct, SPIConfig *scfg)
{
  uint16_t tmpreg = 0;

  /* Configuring the SPI in master mode */
  if(SPI_InitStruct->SPI_Mode == SPI_Mode_Master)
  {
    tmpreg = SPIx->CR1;
	
    tmpreg &= CR1_CLEAR_MASK;

    tmpreg |= (uint16_t)((uint16_t)(SPI_InitStruct->SPI_Direction | SPI_InitStruct->SPI_Mode) |
              (uint16_t)((uint16_t)(SPI_InitStruct->SPI_CPOL | SPI_InitStruct->SPI_CPHA) |
              (uint16_t)((uint16_t)(SPI_InitStruct->SPI_NSS | SPI_InitStruct->SPI_BaudRatePrescaler) | 
                                    SPI_InitStruct->SPI_FirstBit)));
    /* Write to SPIx CR1 */
    SPIx->CR1 = tmpreg;
    scfg->cr1 = tmpreg;
	
    /*-------------------------Data Size Configuration -----------------------*/
    /* Get the SPIx CR2 value */
    tmpreg = SPIx->CR2;
    /* Clear DS[3:0] bits */
    tmpreg &= (uint16_t)~SPI_CR2_DS;
    /* Configure SPIx: Data Size */
    tmpreg |= (uint16_t)(SPI_InitStruct->SPI_DataSize);
    /* Write to SPIx CR2 */
    SPIx->CR2 = tmpreg;
	scfg->cr2 = tmpreg;
  }

  /* Configuring the SPI in slave mode */
  else
  {
/*---------------------------- Data size Configuration -----------------------*/
    /* Get the SPIx CR2 value */
    tmpreg = SPIx->CR2;
    /* Clear DS[3:0] bits */
    tmpreg &= (uint16_t)~SPI_CR2_DS;
    /* Configure SPIx: Data Size */
    tmpreg |= (uint16_t)(SPI_InitStruct->SPI_DataSize);
    /* Write to SPIx CR2 */
    SPIx->CR2 = tmpreg;
    scfg->cr2 = tmpreg;
/*---------------------------- SPIx CR1 Configuration ------------------------*/
    /* Get the SPIx CR1 value */
    tmpreg = SPIx->CR1;
    /* Clear BIDIMode, BIDIOE, RxONLY, SSM, SSI, LSBFirst, BR, MSTR, CPOL and CPHA bits */
    tmpreg &= 0x3040;//CR1_CLEAR_MASK;
    /* Configure SPIx: direction, NSS management, first transmitted bit, BaudRate prescaler
       master/salve mode, CPOL and CPHA */
    /* Set BIDImode, BIDIOE and RxONLY bits according to SPI_Direction value */
    /* Set SSM, SSI and MSTR bits according to SPI_Mode and SPI_NSS values */
    /* Set LSBFirst bit according to SPI_FirstBit value */
    /* Set BR bits according to SPI_BaudRatePrescaler value */
    /* Set CPOL bit according to SPI_CPOL value */
    /* Set CPHA bit according to SPI_CPHA value */
    tmpreg |= (uint16_t)((uint16_t)(SPI_InitStruct->SPI_Direction | SPI_InitStruct->SPI_Mode) | 
                         (uint16_t)((uint16_t)(SPI_InitStruct->SPI_CPOL | SPI_InitStruct->SPI_CPHA) | 
                         (uint16_t)((uint16_t)(SPI_InitStruct->SPI_NSS | SPI_InitStruct->SPI_BaudRatePrescaler) | 
                         SPI_InitStruct->SPI_FirstBit)));

    /* Write to SPIx CR1 */
    SPIx->CR1 = tmpreg;
	scfg->cr1 = tmpreg;
  }

  /* Activate the SPI mode (Reset I2SMOD bit in I2SCFGR register) */
  SPIx->I2SCFGR &= (uint16_t)~((uint16_t)SPI_I2SCFGR_I2SMOD);

/*---------------------------- SPIx CRCPOLY Configuration --------------------*/
  /* Write to SPIx CRCPOLY */
  SPIx->CRCPR = SPI_InitStruct->SPI_CRCPolynomial;
}

// In order to replace SPI_Init_A, it will be reorganized.

void SPI_Initialize(SPI_TypeDef *SPIx, SPIConfig *scfg)
{
  uint16_t   reg = 0;

  reg = (gSPI_Init.SPI_Mode == SPI_MODE_MASTER)   
       ? SPIx->CR1 
       : SPIx->CR2;

  reg &= (gSPI_Init.SPI_Mode == SPI_MODE_MASTER)  
       ? CR1_CLEAR_MASK 
       : ~SPI_CR2_DS;

  reg |= (gSPI_Init.SPI_Mode == SPI_MODE_MASTER)  
       ? VAR_OR(gSPI_Init) 
       : (gSPI_Init.SPI_DataSize);

  (gSPI_Init.SPI_Mode == SPI_MODE_MASTER) 
       ? (SPIx->CR1 = scfg->cr1 = reg) 
       : (SPIx->CR2 = scfg->cr2 = reg);

  if(gSPI_Init.SPI_Mode == SPI_MODE_MASTER){
       reg = SPIx->CR2;
       reg &= (uint16_t)~SPI_CR2_DS;

	// Configure SPIx : Data Size */
       reg |= (uint16_t)(gSPI_Init.SPI_DataSize);
       SPIx->CR2 = scfg->cr2 = reg;
  } else {
       reg = SPIx->CR1;
       reg &= CR1_CLEAR_MASK;
       reg |= VAR_OR(gSPI_Init);

       SPIx->CR1 = scfg->cr1 = reg;
  }
 
  /* Activate the SPI mode (Reset I2SMOD bit in I2SCFGR register) */
  SPIx->I2SCFGR &= (uint16_t)~((uint16_t)SPI_I2SCFGR_I2SMOD);

  /* Write to SPIx CRCPOLY */
  SPIx->CRCPR = gSPI_Init.SPI_CRCPolynomial;
}


#define SPI_RxFIFOThreshold_HF          	 ((uint16_t)0x0000)
#define SPI_RxFIFOThreshold_QF          	 ((uint16_t)0x1000)
#define IS_SPI_RX_FIFO_THRESHOLD(THRESHOLD)  	(  ((THRESHOLD) == SPI_RxFIFOThreshold_HF) || \
                                             						   ((THRESHOLD) == SPI_RxFIFOThreshold_QF) )

static void  L3GD_SPI_Init(SPI_TypeDef *spi, SPIConfig *spiconfig)
{
#if 1
	SPI_InitTypeDef_A  SPI_InitStructure;

	SPI_InitStructure.SPI_Direction         = ((uint16_t)0x0000); //  2Lines_FullDuplex;
	SPI_InitStructure.SPI_DataSize          = ((uint16_t)0x0700); //SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL              = ((uint16_t)0x0000); //SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA              = ((uint16_t)0x0000); //SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS               = ((uint16_t)0x0200); // SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = ((uint16_t)0x0010); //SPI_BaudRatePrescaler_8;
	SPI_InitStructure.SPI_FirstBit          = ((uint16_t)0x0000); // SPI_FirstBit_MSB
	SPI_InitStructure.SPI_CRCPolynomial     = 7;
	SPI_InitStructure.SPI_Mode              = ((uint16_t)0x0104); //SPI_Mode_Master;
	
	SPI_Init_A(spi, &SPI_InitStructure, spiconfig);
#endif
//    VAR_OR(gSPI_Init);
//	SPI_Init_A(spi, &gSPI_Init, spiconfig);
 //   SPI_Initialize(spi, spiconfig);
    
	/* Configure the RX FIFO Threshold */
	//SPI_RxFIFOThresholdConfig(spi, SPI_RxFIFOThreshold_QF, spiconfig);

	/* Clear FRXTH bit */
	spi->CR2 &= (uint16_t)~((uint16_t)SPI_CR2_FRXTH);	
	/* Set new FRXTH bit value */
	spi->CR2 |= SPI_RxFIFOThreshold_QF;	
	spiconfig->cr2 = spi->CR2;
	
	/* Enable SPI1	*/
	SPI_Cmd(spi, ENABLE, spiconfig);
	
}

extern void SPI_I2S_DeInit(SPI_TypeDef* SPIx);

void RCC_AHBPeriphClockCmd(uint32_t RCC_AHBPeriph, FunctionalState NewState);

/*****************************************************************************
- 210313tsham,

   The following TEST1 and TEST2 has the same function between ST and ChibiOS function.
   TEST1, and TEST2 are tested reciprocally. As a result, the _pal_lld_setgroupmode made 
   problem, the testing environment from /TS_SPI/main.c cannot read chip_id. 
   So,the alternate part was changed( os/hal/platforms/pal_lld.c,
   The other parts also occurs same problems. it seems to be set register value 
   seriously damaged.

*****************************************************************************
*****************************************************************************/

static void L3GD20_LowLevel_Init(SPIDriver *drvspi, SPIConfig *spi_cfg)
{
  rccEnableAPB2(RCC_APB2RSTR_SPI1RST, TRUE);	
  rccEnableAHB(GPIOA_SPI1_SCK | GPIOA_SPI1_MOSI | GPIOA_SPI1_MISO, TRUE);
  rccEnableAHB(RCC_AHBENR_GPIOEEN, ENABLE);   // GPIOE Clock Enable
  rccEnableAHB(RCC_AHBENR_GPIOEEN, ENABLE);   // GPIOE Clock Enable
  rccEnableAHB(RCC_AHBENR_GPIOEEN, ENABLE);   // GPIOE Clock Enable

  _pal_lld_setgroupmode(GPIOA, L3GD20_SPI_SCK_AF,  L3GD20_SPI_SCK_SOURCE << ALTERNATE_SHIFT);
  _pal_lld_setgroupmode(GPIOA,L3GD20_SPI_MISO_AF, L3GD20_SPI_MISO_SOURCE << ALTERNATE_SHIFT);
  _pal_lld_setgroupmode(GPIOA,L3GD20_SPI_MOSI_AF, L3GD20_SPI_MOSI_SOURCE << ALTERNATE_SHIFT);

  _pal_lld_setgroupmode(GPIOA, L3GD20_SPI_AF, 0x04 << MODE_SHIFT);  // GPIO_Mode_AF  0x02->0x04
  _pal_lld_setgroupmode(GPIOA, L3GD20_SPI_AF, 0x00 << OTYPE_SHIFT); // GPIO_OType_PP
  _pal_lld_setgroupmode(GPIOA, L3GD20_SPI_AF, 0x00 << PUDR_SHIFT);  //GPIO_PuPd_NOPUU
  _pal_lld_setgroupmode(GPIOA, L3GD20_SPI_AF, 0x03 << OSPEED_SHIFT); // highest 50Hz

  pal_lld_writepad(GPIOA,GPIOA_SPI1_SCK, GPIO_BSRR_BS_5);  	//SCK
  pal_lld_writepad(GPIOA,GPIOA_SPI1_MOSI, GPIO_BSRR_BS_7);  	//MOSI
  pal_lld_writepad(GPIOA,GPIOA_SPI1_MISO, GPIO_BSRR_BS_6);  	//MISO

  /***  SPI Configuration -----------------------------------------------------*/
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////

  rccEnableAPB2(RCC_APB2RSTR_SPI1RST, ENABLE);  // Enable SPI_De_Init
  rccEnableAPB2(RCC_APB2RSTR_SPI1RST, DISABLE); // Disable SPI_De_Init

  spi_cfg->ssport = GPIOE;              	// spi configuration ssport allocation 
  spi_cfg->sspad  = GPIOE_SPI1_CS;		// spi sspad allocation from SPIOE_SPI1_CS
	
  ///////////////////////////////////////////////////////////////////////////////
  //
  L3GD_SPI_Init(drvspi->spi, spi_cfg); // drvspi->spi initialize, and spi_cfg initialize
	
  _pal_lld_setgroupmode(GPIOE, L3GD20_SPI_AF, 0x02 << MODE_SHIFT);   // GPIO_Mode_OUT
  _pal_lld_setgroupmode(GPIOE, L3GD20_SPI_AF, 0x00 << OTYPE_SHIFT);  // 3: GPIO_OType_PP
  _pal_lld_setgroupmode(GPIOE, L3GD20_SPI_AF, 0x03 << OSPEED_SHIFT); // 0x03:50MHz

  pal_lld_writepad(GPIOE,3, GPIO_BSRR_BS_3);
  pal_lld_setport(GPIOE, GPIO_BSRR_BS_3);   // Deselect : Chip Select high
  
  _pal_lld_setgroupmode(GPIOE, L3GD20_SPI_AF,0x01 << MODE_SHIFT);  	// GPIO_Mode_IN
  _pal_lld_setgroupmode(GPIOE, L3GD20_SPI_AF, 0x00 << OTYPE_SHIFT);
  _pal_lld_setgroupmode(GPIOE, L3GD20_SPI_AF, 0x03 << OSPEED_SHIFT);
  _pal_lld_setgroupmode(GPIOE, L3GD20_SPI_AF, 0x00 << PUDR_SHIFT);
	
  pal_lld_writepad(GPIOE,0, GPIO_BSRR_BS_0);    		// INT1 pin	
  pal_lld_writepad(GPIOE,1, GPIO_BSRR_BS_1);    		// INT2 pin
}  

/**
  * @brief  Writes one byte to the L3GD20.
  * @param  pBuffer : pointer to the buffer  containing the data to be written to the L3GD20.
  * @param  WriteAddr : L3GD20's internal address to write to.
  * @param  NumByteToWrite: Number of bytes to write.
  * @retval None
  */
void L3GD20_Write(uint8_t* pBuffer, uint8_t *WriteAddr, uint16_t NumByteToWrite)
{
  /* Configure the MS bit: 
       - When 0, the address will remain unchanged in multiple read/write commands.
       - When 1, the address will be auto incremented in multiple read/write commands.
  */
  if(NumByteToWrite > 0x01)
  {
    *WriteAddr |= (uint8_t)MULTIPLEBYTE_CMD;
  }
  /* Set chip select Low at the start of the transmission */
  L3GD20_CS_LOW();
  
  /* Send the Address of the indexed register */
  L3GD20_SendByte(*WriteAddr);
  /* Send the data that will be written into the device (MSB First) */
  while(NumByteToWrite >= 0x01)
  {
    L3GD20_SendByte(*pBuffer);
    NumByteToWrite--;
    pBuffer++;
  }
  
  /* Set chip select High at the end of the transmission */ 
  L3GD20_CS_HIGH();
}


/**
  * @brief  Set L3GD20 Initialization.
  * @param  L3GD20_InitStruct: pointer to a L3GD20_InitTypeDef structure 
  *         that contains the configuration setting for the L3GD20.
  * @retval None
  */
void L3GD20_Init(SPIDriver *lspidrive, SPIConfig *lspi_cfg)
{  
//  lspidrive->spi = SPI1;

  /* Configure the low level interface ---------------------------------------*/
  L3GD20_LowLevel_Init(lspidrive, lspi_cfg); 
}

void L3GD20_Init_Setting(L3GD20_InitTypeDef *L3GD20_InitStruct)
{
  uint8_t                ctrl1 = 0x00, ctrl4 = 0x00;
  static uint8_t   	ctrl_reg;


	/* Configure MEMS: data rate, power mode, full scale and axes */
	ctrl1 |= (uint8_t) (L3GD20_InitStruct->Power_Mode | L3GD20_InitStruct->Output_DataRate | \
					  L3GD20_InitStruct->Axes_Enable | L3GD20_InitStruct->Band_Width);
	
	ctrl4 |= (uint8_t) (L3GD20_InitStruct->BlockData_Update | L3GD20_InitStruct->Endianness | \
					  L3GD20_InitStruct->Full_Scale /* _1 | L3GD20_InitStruct->Full_Scale_0 */);
					  
	/* Write value to MEMS CTRL_REG1 regsister */
	ctrl_reg = L3GD20_CTRL_REG1_ADDR; L3GD20_Write(&ctrl1, &ctrl_reg, 1);
	
	/* Write value to MEMS CTRL_REG4 regsister */
	ctrl_reg = L3GD20_CTRL_REG4_ADDR; L3GD20_Write(&ctrl4, &ctrl_reg, 1);
}

/**
  * @brief  Basic management of the timeout situation.
  * @param  None.
  * @retval None.
  */
uint32_t L3GD20_TIMEOUT_UserCallback(void)
{
  /* Block communication and all processes */
  while (1)
  {
  
  }
}


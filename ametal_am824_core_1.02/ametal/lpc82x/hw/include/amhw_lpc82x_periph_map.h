/*******************************************************************************
*                                 AMetal
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2015 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      ametal.support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief LPC82X ����ӳ��
 *
 * \internal
 * \par Modification history
 * - 1.00 15-12-07  cyl, first implementation.
 * \endinternal
 */
 
#ifndef __AMHW_LPC82X_PERIPH_MAP_H
#define __AMHW_LPC82X_PERIPH_MAP_H

#ifdef __cplusplus
extern "C" {    
#endif 

#include "lpc82x_regbase.h"

/** 
 * \addtogroup amhw_lpc82x_if_periph_map
 * \copydoc amhw_lpc82x_periph_map.h
 * @{
 */

/**
 * \name ���趨��
 * @{
 */

/** \brief  �жϿ�����(INT)�Ĵ�����ָ��       */
#define AMHW_LPC82X_INT         ((amhw_lpc82x_int_t     *)LPC82X_NVIC_BASE   ) 

/** \brief  ϵͳ�δ�ʱ��(SYSTICK)�Ĵ�����ָ��    */
#define AMHW_LPC82X_SYSTICK     ((amhw_lpc82x_systick_t *)LPC82X_SYSTICK_BASE)     
                                                        
/** \brief  IO����(IOCON)�Ĵ�����ָ��      */                               
#define AMHW_LPC82X_IOCON       ((amhw_lpc82x_iocon_t   *)LPC82X_IOCON_BASE  )     
                                                        
/** \brief  ͨ���������(GPIO)�Ĵ�����ָ��       */                               
#define AMHW_LPC82X_GPIO        ((amhw_lpc82x_gpio_t    *)LPC82X_GPIO_BASE   )     
                                                        
/** \brief  �����ж�(PINT)�Ĵ�����ָ��  */                               
#define AMHW_LPC82X_PINT        ((amhw_lpc82x_pint_t    *)LPC82X_PINT_BASE   )     
                                                        
/** \brief  ���ؾ���(SWM)�Ĵ�����ָ��        */                               
#define AMHW_LPC82X_SWM         ((amhw_lpc82x_swm_t     *)LPC82X_SWM_BASE    )     
                                                        
/** \brief  ���븴�ÿ���(INMUX)�Ĵ�����ָ��     */                               
#define AMHW_LPC82X_INMUX       ((amhw_lpc82x_inmux_t   *)LPC82X_INMUX_BASE  )     
                                                        
/** \brief  DMA������(DMA)�Ĵ�����ָ��        */                               
#define AMHW_LPC82X_DMA         ((amhw_lpc82x_dma_t     *)LPC82X_DMA_BASE    )     
                                                        
/** \brief  ��Ƶ�ʶ�ʱ��(MRT)�Ĵ�����ָ��       */                               
#define AMHW_LPC82X_MRT         ((amhw_lpc82x_mrt_t     *)LPC82X_MRT_BASE    )    
                                                        
/** \brief ״̬�����ö�ʱ��(SCT0)�Ĵ�����ָ��       */                              
#define AMHW_LPC82X_SCT0        ((amhw_lpc82x_sct_t     *)LPC82X_SCT0_BASE   )

/** \brief ϵͳ����(SYSCON)�Ĵ�����ָ��     */
#define AMHW_LPC82X_SYSCON      ((amhw_lpc82x_syscon_t  *)LPC82X_SYSCON_BASE )      
                                                        
/** \brief ��������ӿ�(SPI0)�Ĵ�����ָ��    */                               
#define AMHW_LPC82X_SPI0        ((amhw_lpc82x_spi_t     *)LPC82X_SPI0_BASE   )      
                                                                               
/** \brief ��������ӿ�(SPI1)�Ĵ�����ָ��    */                                                      
#define AMHW_LPC82X_SPI1        ((amhw_lpc82x_spi_t     *)LPC82X_SPI1_BASE   ) 
                                                        
/** \brief I2C���߿�����(I2C0)�Ĵ�����ָ��       */                               
#define AMHW_LPC82X_I2C0        ((amhw_lpc82x_i2c_t     *)LPC82X_I2C0_BASE   )    
                                                                             
/** \brief I2C���߿�����(I2C1)�Ĵ�����ָ��        */                                                    
#define AMHW_LPC82X_I2C1        ((amhw_lpc82x_i2c_t     *)LPC82X_I2C1_BASE   )   
                                                                             
/** \brief I2C���߿�����(I2C2)�Ĵ�����ָ��        */                                                    
#define AMHW_LPC82X_I2C2        ((amhw_lpc82x_i2c_t     *)LPC82X_I2C2_BASE   )    
                                                        
/** \brief I2C���߿�����(I2C3)�Ĵ�����ָ��        */                                                      
#define AMHW_LPC82X_I2C3        ((amhw_lpc82x_i2c_t     *)LPC82X_I2C3_BASE   )  
   
/** \brief ����(UART0)�Ĵ�����ָ��      */
#define AMHW_LPC82X_USART0      ((amhw_lpc82x_usart_t   *)LPC82X_USART0_BASE )      
                                                        
/** \brief ����(UART1)�Ĵ�����ָ��      */                               
#define AMHW_LPC82X_USART1      ((amhw_lpc82x_usart_t   *)LPC82X_USART1_BASE )      
                                                        
/** \brief ����(UART2)�Ĵ�����ָ��      */                               
#define AMHW_LPC82X_USART2      ((amhw_lpc82x_usart_t   *)LPC82X_USART2_BASE )      
                                                      
/** \brief ѭ������У��(CRC)�Ĵ�����ָ��        */                             
#define AMHW_LPC82X_CRC         ((amhw_lpc82x_crc_t     *)LPC82X_CRC_BASE    )      
                                                        
/** \brief �Ի��Ѷ�ʱ��(WKT)�Ĵ�����ָ��        */                               
#define AMHW_LPC82X_WKT         ((amhw_lpc82x_wkt_t     *)LPC82X_WKT_BASE    )      
                                                        
/** \brief ��Դ����Ԫ(PMU)�Ĵ�����ָ��        */                               
#define AMHW_LPC82X_PMU         ((amhw_lpc82x_pmu_t     *)LPC82X_PMU_BASE    )      
                                                       
/** \brief FLASH������(FMC)�Ĵ�����ָ��         */                              
#define AMHW_LPC82X_FMC         ((amhw_lpc82x_fmc_t     *)LPC82X_FMC_BASE    )    
  
/** \brief ģ��ת��(ADC)�Ĵ�����ָ��        */                              
#define AMHW_LPC82X_ADC0        ((amhw_lpc82x_adc_t     *)LPC82X_ADC0_BASE   )

/** \brief ģ��Ƚ���(ACMP)�Ĵ�����ָ��         */                     
#define AMHW_LPC82X_ACMP        ((amhw_lpc82x_acmp_t    *)LPC82X_ACMP_BASE  )

/** \brief ���ڿ��Ź�(WWDT)�Ĵ�����ָ��         */
#define AMHW_LPC82X_WWDT        ((amhw_lpc82x_wwdt_t    *)LPC82X_WWDT_BASE  )      

/** @} */

/**
 * @} amhw_lpc82x_if_periph_map
 */
    
#ifdef __cplusplus
}
#endif

#endif /* __AMHW_LPC82X_REG_BASE_H */

/* end of file */

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
 * \brief I2C Ӳ�������ӿ�
 * 
 * 1. ���ж����������ӻ��ͼ�صȹ��ܣ�
 * 2. ֧�ֶ�������;��дӻ��Ķ�������
 * 3. ��Ӳ����֧�ֶ���ӻ���ַ��
 * 5. һ���ӻ���ַ����ѡ���Ե��޶�һ��λ�����һ����ַ��Χ���Ա�����I2C
 *    ���ߵ�ַ��Ӧ��
 * 6. ͨ��������ã���֧��10bits��ַ���豸��
 * 7. ֧��ϵͳ�������ߣ�
 * 8. ֧�ֿ���ģʽplus���������ʿɴ�1MHz����
 *
 * \internal
 * \par Modification History
 * - 1.02 15-12-05  cyl, modify the namespace.
 * - 1.01 15-11-30  cyl, bug fixed.
 * - 1.00 15-07-06  win, first implementation.
 * \endinternal
 */

#ifndef __AMHW_LPC82X_I2C_H
#define __AMHW_LPC82X_I2C_H

#include "ametal.h"

#ifdef __cplusplus
extern "C" {
#endif
    
/**
 * \addtogroup amhw_lpc82x_if_i2c 
 * \copydoc amhw_lpc82x_i2c.h
 * @{
 */
    
/* ʹ������������ν��� */
#if defined(__CC_ARM)
  #pragma push
  #pragma anon_unions
#elif defined(__ICCARM__)
  #pragma language=extended
#elif defined(__GNUC__)
    
  /* Ĭ��ʹ������������ */
#elif defined(__TMS470__)

  /* Ĭ��ʹ������������ */
#elif defined(__TASKING__)
  #pragma warning 586
#else
  #warning Not supported compiler t
#endif
    
/**
 * \brief I2C �Ĵ�����ṹ��
 */
typedef struct amhw_lpc82x_i2c {
    __IO uint32_t  cfg;           /**< \brief I2C ���üĴ���              */
    __IO uint32_t  stat;          /**< \brief I2C ״̬�Ĵ���              */
    __IO uint32_t  intenset;      /**< \brief I2C �ж�λʹ�ܻ��ȡ�Ĵ���  */
    __O  uint32_t  intenclr;      /**< \brief I2C �ж�λ���ܼĴ���        */
    __IO uint32_t  timeout;       /**< \brief I2C ��ʱʱ���趨�Ĵ���      */
    __IO uint32_t  clkdiv;        /**< \brief I2C ʱ�ӷ�Ƶ��ֵ�趨�Ĵ���  */
    __I  uint32_t  intstat;       /**< \brief I2C �ж�״̬                */
    __I  uint32_t  reserved;      /**< \brief ����λ                      */
    __IO uint32_t  mstctl;        /**< \brief I2C �������ƼĴ���          */
    
    /** \brief I2C ����SCL�ߵ�ƽ�͵͵�ƽʱ���趨�Ĵ��� */
    __IO uint32_t  msttime;       
    
    __IO uint32_t  mstdat;        /**< \brief I2C �������ݼĴ���          */
    __I  uint32_t  reserved1[5];  /**< \brief ����λ                      */
    __IO uint32_t  slvctl;        /**< \brief I2C �ӻ����ƼĴ���          */
    __IO uint32_t  slvdat;        /**< \brief I2C �ӻ����ݼĴ���          */
    
    union {
        __IO uint32_t slvadr[4];
        struct {
            __IO uint32_t  slvadr0;  /**< \brief �ӻ���ַ0                */
            __IO uint32_t  slvadr1;  /**< \brief �ӻ���ַ1                */
            __IO uint32_t  slvadr2;  /**< \brief �ӻ���ַ2                */
            __IO uint32_t  slvadr3;  /**< \brief �ӻ���ַ3                */
        };
    };
    
    __IO uint32_t  slvqual0;      /**< \brief �ӻ���ַ�޶��Ĵ���          */
    __I  uint32_t  reserved2[9];  /**< \brief ����λ                      */
    __I  uint32_t  monrxdat;      /**< \brief ���ģʽ���ݼĴ���          */
} amhw_lpc82x_i2c_t;


/**
 * \name I2C �ӻ���ַ���
 * \anchor grp_amhw_lpc82x_i2c_addr_num
 * @{
 */

#define AMHW_LPC82X_I2C_SLVADR_0   (0)    /**< \brief I2C �ӻ���ַ���0 */
#define AMHW_LPC82X_I2C_SLVADR_1   (1)    /**< \brief I2C �ӻ���ַ���1 */
#define AMHW_LPC82X_I2C_SLVADR_2   (2)    /**< \brief I2C �ӻ���ַ���2 */
#define AMHW_LPC82X_I2C_SLVADR_3   (3)    /**< \brief I2C �ӻ���ַ���3 */

/** @} */

/**
 * \name I2C ״̬�Ĵ�������λ��ϸ����
 * \anchor grp_amhw_lpc82x_i2c_stat
 * @{
 */

/** \brief I2C ��������״̬          */
#define AMHW_LPC82X_I2C_STAT_MSTPEND          AM_SBF(0x1, 0)  

/** \brief I2C ����״̬����          */
#define AMHW_LPC82X_I2C_MASTER_STATE_MASK     AM_SBF(0x7, 1)

/** \brief I2C ��������״̬          */
#define AMHW_LPC82X_I2C_STAT_MSTIDLE          AM_SBF(0x0, 1)

/** \brief I2C �������վ���״̬      */
#define AMHW_LPC82X_I2C_STAT_MSTRX            AM_SBF(0x1, 1)

/** \brief I2C �������;���״̬      */
#define AMHW_LPC82X_I2C_STAT_MSTTX            AM_SBF(0x2, 1)

/** \brief I2C ������ַ����Ӧ        */
#define AMHW_LPC82X_I2C_STAT_MSTNACKADDR      AM_SBF(0x3, 1)

/** \brief I2C ������������Ӧ        */
#define AMHW_LPC82X_I2C_STAT_MSTNACKTX        AM_SBF(0x4, 1)

/** \brief I2C �����ٲ�ʧ�ܱ�־      */
#define AMHW_LPC82X_I2C_STAT_MSTARBLOSS       AM_SBF(0x1, 4)

/** \brief I2C ��������/ֹͣ�����־ */
#define AMHW_LPC82X_I2C_STAT_MSTSSERR         AM_SBF(0x1, 6)


/** \brief I2C �ӻ�����״̬          */
#define AMHW_LPC82X_I2C_STAT_SLVPEND          AM_SBF(0x1, 8)

/** \brief I2C �ӻ�״̬����          */
#define AMHW_LPC82X_I2C_SLAVE_STATE_MASK      AM_SBF(0x3, 9)

/** \brief I2C �ӻ���ַ              */
#define AMHW_LPC82X_I2C_STAT_SLVADDR          AM_SBF(0x0, 9)

/** \brief I2C �ӻ����վ���״̬      */
#define AMHW_LPC82X_I2C_STAT_SLVRX            AM_SBF(0x1, 9)

/** \brief I2C �ӻ����;���״̬      */
#define AMHW_LPC82X_I2C_STAT_SLVTX            AM_SBF(0x2, 9)

/** \brief I2C �ӻ�δ���졣��ʱ�����Խ������˯�߻����ģʽ */
#define AMHW_LPC82X_I2C_STAT_SLVNOTSTR        AM_SBF(0x1, 11) 

/** \brief I2C �ӻ���ַƥ��״̬      */
#define AMHW_LPC82X_I2C_STAT_SLVIDX_MASK      AM_SBF(0x3, 12)

/** \brief I2C �ӻ�ѡ��              */
#define AMHW_LPC82X_I2C_STAT_SLVSEL           AM_SBF(0x1, 14)

/** \brief I2C �ӻ�ȡ��ѡ����־      */
#define AMHW_LPC82X_I2C_STAT_SLVDESEL         AM_SBF(0x1, 15)  


/** \brief I2C ���ģʽ���ܾ���      */
#define AMHW_LPC82X_I2C_STAT_MONRDY           AM_SBF(0x1, 16)

/** \brief I2C ���ģʽ�����־      */
#define AMHW_LPC82X_I2C_STAT_MONOV            AM_SBF(0x1, 17)

/** \brief I2C ���ģʽ�����־      */
#define AMHW_LPC82X_I2C_STAT_MONACTIVE        AM_SBF(0x1, 18)

/** \brief I2C ���ģʽ���б�־      */
#define AMHW_LPC82X_I2C_STAT_MONIDLE          AM_SBF(0x1, 19)
                               

/** \brief I2C �¼���ʱ��־          */
#define AMHW_LPC82X_I2C_STAT_EVTIMEOUT        AM_SBF(0x1, 24)  

/** \brief I2C ʱ�ӳ�ʱ��־          */
#define AMHW_LPC82X_I2C_STAT_SCLTIMEOUT       AM_SBF(0x1, 25)

/** @} */

/**
 * \name I2C �ж�ʹ�ܼĴ�������λ��ϸ����
 * \anchor grp_amhw_lpc82x_i2c_intenset
 * @{
 */

/** \brief I2C ���������ж�ʹ��          */
#define AMHW_LPC82X_I2C_INTENSET_MSTPEND      AM_SBF(0x1, 0)

/** \brief I2C �����ٲ�ʧ���ж�ʹ��      */
#define AMHW_LPC82X_I2C_INTENSET_MSTARBLOSS   AM_SBF(0x1, 4)

/** \brief I2C ��������/ֹͣ�����ж�ʹ�� */
#define AMHW_LPC82X_I2C_INTENSET_MSTSSERR     AM_SBF(0x1, 6)
                                       
                                       
/** \brief I2C �ӻ������ж�ʹ��          */
#define AMHW_LPC82X_I2C_INTENSET_SLVPEND      AM_SBF(0x1, 8) 

/** \brief I2C �ӻ�δ����ʹ��            */
#define AMHW_LPC82X_I2C_INTENSET_SLVNOTSTR    AM_SBF(0x1, 11) 

/** \brief I2C �ӻ�ȡ��ѡ���ж�ʹ��      */
#define AMHW_LPC82X_I2C_INTENSET_SLVDESEL     AM_SBF(0x1, 15)
                

/** \brief I2C ���ģʽ���վ����ж�ʹ��  */
#define AMHW_LPC82X_I2C_INTENSET_MONRX        AM_SBF(0x1, 16)

/** \brief I2C ��س����ж�ʹ��          */
#define AMHW_LPC82X_I2C_INTENSET_MONOVEN      AM_SBF(0x1, 17)

/** \brief I2C ���ģʽ�����ж�ʹ��      */
#define AMHW_LPC82X_I2C_INTENSET_MONIDLE      AM_SBF(0x1, 19)
                                       
                                       
/** \brief I2C �¼���ʱ�ж�ʹ��          */
#define AMHW_LPC82X_I2C_INTENSET_EVTIMEOUT    AM_SBF(0x1, 24)

/** \brief I2C ʱ�ӳ�ʱ�ж�ʹ��          */
#define AMHW_LPC82X_I2C_INTENSET_SCLTIMEOUT   AM_SBF(0x1, 25)

/** @} */

/**
 * \name I2C �жϽ��ܼĴ�������λ��ϸ����
 * \anchor grp_amhw_lpc82x_i2c_intenclr
 * @{
 */

/** \brief I2C ���������жϽ���          */
#define AMHW_LPC82X_I2C_INTENCLR_MSTPEND      AM_SBF(0x1, 0)

/** \brief I2C �����ٲ�ʧ���жϽ���      */
#define AMHW_LPC82X_I2C_INTENCLR_MSTARBLOSS   AM_SBF(0x1, 4)

/** \brief I2C ��������/ֹͣ�����жϽ��� */
#define AMHW_LPC82X_I2C_INTENCLR_MSTSSERR     AM_SBF(0x1, 6)
                                                  

/** \brief I2C �ӻ������жϽ���          */
#define AMHW_LPC82X_I2C_INTENCLR_SLVPEND      AM_SBF(0x1, 8)

/** \brief I2C �ӻ�δ�����жϽ���        */
#define AMHW_LPC82X_I2C_INTENCLR_SLVNOTSTR    AM_SBF(0x1, 11)

/** \brief I2C �ӻ�ȡ��ѡ���жϽ���      */
#define AMHW_LPC82X_I2C_INTENCLR_SLVDESEL     AM_SBF(0x1, 15)
                                                   
                                                   
/** \brief I2C ���ģʽ���վ����жϽ���  */
#define AMHW_LPC82X_I2C_INTENCLR_MONRX        AM_SBF(0x1, 16)

/** \brief I2C ��س����жϽ���          */
#define AMHW_LPC82X_I2C_INTENCLR_MONOVEN      AM_SBF(0x1, 17)

/** \brief I2C ���ģʽ�����жϽ���      */
#define AMHW_LPC82X_I2C_INTENCLR_MONIDLE      AM_SBF(0x1, 19) 
                                 

/** \brief I2C �¼���ʱ�жϽ���          */
#define AMHW_LPC82X_I2C_INTENCLR_EVTIMEOUT    AM_SBF(0x1, 24)

/** \brief I2C ʱ�ӳ�ʱ�жϽ���          */
#define AMHW_LPC82X_I2C_INTENCLR_SCLTIMEOUT   AM_SBF(0x1, 25)

/** @} */

/**
 * \name I2C �������ƼĴ���λ����
 * \anchor grp_amhw_lpc82x_i2c_mstctl
 * @{
 */

/** \brief I2C ������������Ӧ������λ */ 
#define AMHW_LPC82X_I2C_CTL_MSTCONTINUE       AM_SBF(0x1, 0)

/** \brief I2C ������������λ         */
#define AMHW_LPC82X_I2C_CTL_MSTSTART          AM_SBF(0x1, 1)

/** \brief I2C ����ֹͣ����λ         */
#define AMHW_LPC82X_I2C_CTL_MSTSTOP           AM_SBF(0x1, 2)

/** \brief I2C ����DMA����λ          */
#define AMHW_LPC82X_I2C_CTL_MSTDMA            AM_SBF(0x1, 3)

/** @} */

/**
 * \name I2C �ӻ����ƼĴ���λ����
 * \anchor grp_amhw_lpc82x_i2c_slvctl
 * @{
 */ 

/** \brief I2C �ӻ���������Ӧ������λ */
#define AMHW_LPC82X_I2C_CTL_SLVCONTINUE       AM_SBF(0x1, 0)

/** \brief I2C �ӻ�����Ӧ����λ       */
#define AMHW_LPC82X_I2C_CTL_SLVNACK           AM_SBF(0x1, 1)

/** \brief I2C �ӻ�DMA����λ          */
#define AMHW_LPC82X_I2C_CTL_SLVDMA            AM_SBF(0x1, 3)

/** @} */

/**
 * \name I2C ������ݼĴ���λ����
 * \anchor grp_amhw_lpc82x_i2c_monrxdat
 * @{
 */

/** \brief ��⵽I2C�����ϵ������¼�     */
#define AMHW_LPC82X_I2C_MONRXDAT_START        AM_SBF(0x1, 8)

/** \brief ��⵽I2C�����ϵ��ظ������¼� */
#define AMHW_LPC82X_I2C_MONRXDAT_RESTART      AM_SBF(0x1, 9)

/** \brief �������Ӧ��                  */
#define AMHW_LPC82X_I2C_MONRXDAT_NACK         AM_SBF(0x1, 10)

/** @} */

/**
 * \anchor I2C_mode
 * \brief I2C ģʽ���壨�ο�I2C���üĴ����� 
 */
typedef enum amhw_lpc82x_i2c_mode {
    
    /**< \brief ����ģʽʹ��         */
    AMHW_LPC82X_I2C_MASTER_MODE    = AM_SBF(0x1, 0), 
    /**< \brief �ӻ�ģʽʹ��         */
    AMHW_LPC82X_I2C_SLAVE_MODE     = AM_SBF(0x1, 1), 
    /**< \brief ���ģʽʹ��         */
    AMHW_LPC82X_I2C_MONITOR_MODE   = AM_SBF(0x1, 2), 
    /**< \brief ��ʱʹ��             */
    AMHW_LPC82X_I2C_TIMEOUT_MODE   = AM_SBF(0x1, 3), 
    /**< \brief ���ģʽʱ������ʹ�� */
    AMHW_LPC82X_I2C_MONCLKSTR_MODE = AM_SBF(0x1, 4)  
    
} amhw_lpc82x_i2c_mode_t;

/**
 * \brief  ʹ���趨��I2Cģʽ
 *
 * \param[in] p_hw_i2c : ָ��I2C�Ĵ������ָ��
 * \param[in] mode     : ��Ҫʹ�ܵ�I2C ģʽ,�ο� \ref I2C_mode "I2C ģʽ����"
 *
 * \return    ��
 */
am_static_inline
void amhw_lpc82x_i2c_mode_enable (amhw_lpc82x_i2c_t      *p_hw_i2c, 
                                  amhw_lpc82x_i2c_mode_t  mode)
{
    AM_BIT_SET_MASK(p_hw_i2c->cfg, mode);
}

/**
 * \brief  �����趨��I2Cģʽ
 *
 * \param[in] p_hw_i2c : ָ��I2C�Ĵ������ָ��
 * \param[in] mode     : ��Ҫ���ܵ�I2C ģʽ,�ο� \ref I2C_mode "I2C ģʽ����"
 *
 * \return    ��
 */
am_static_inline 
void amhw_lpc82x_i2c_mode_disable (amhw_lpc82x_i2c_t      *p_hw_i2c, 
                                   amhw_lpc82x_i2c_mode_t  mode)
{
    AM_BIT_CLR_MASK(p_hw_i2c->cfg, mode);
}

/**
 * \brief ��ȡ I2C ״̬�Ĵ�����ֵ
 * \param[in] p_hw_i2c : ָ��I2C�Ĵ������ָ��
 * \return I2C ״̬�Ĵ���ֵ
 */
am_static_inline
uint32_t amhw_lpc82x_i2c_stat_get (amhw_lpc82x_i2c_t *p_hw_i2c)
{
    return (p_hw_i2c->stat);
}
    
/**
 * \brief ��� I2C ״̬�Ĵ�����־
 * 
 * \param[in] p_hw_i2c : ָ��I2C�Ĵ������ָ��
 * \param[in] flags    : ��Ҫ�����״̬��־��AMHW_LPC82X_I2C_STAT_* ���
 *                       ���AMHW_LPC82X_I2C_STAT_*��
 *
 * \return ��
 */
am_static_inline
void amhw_lpc82x_i2c_stat_clr (amhw_lpc82x_i2c_t *p_hw_i2c, uint32_t flags)
{
    p_hw_i2c->stat = flags;
}

/**
 * \brief I2C �ж�ʹ��
 *
 * \param[in] p_hw_i2c : ָ��I2C�Ĵ������ָ��
 * \param[in] flags    : AMHW_LPC82X_I2C_INTENSET_* ��ֵ���� 
 *                       AMHW_LPC82X_I2C_INTENSET_*��� ��OR��
 *                       ֵ(#AMHW_LPC82X_I2C_INTENSET_MSTPEND)
 *
 * \return ��
 */
am_static_inline
void amhw_lpc82x_i2c_int_enable (amhw_lpc82x_i2c_t *p_hw_i2c, uint32_t flags)
{
    AM_BIT_SET_MASK(p_hw_i2c->intenset, flags);
}

/**
 * \brief I2C �жϽ���
 *
 * \param[in] p_hw_i2c : ָ��I2C�Ĵ������ָ��
 * \param[in] flags    : AMHW_LPC82X_I2C_INTENSET_* ��ֵ���� 
 *                       AMHW_LPC82X_I2C_INTENSET_*��� ��OR��
 *                       ֵ(#AMHW_LPC82X_I2C_INTENSET_MSTPEND)
 *
 * \return ��
 */
am_static_inline
void amhw_lpc82x_i2c_int_disable (amhw_lpc82x_i2c_t *p_hw_i2c, uint32_t flags)
{
    AM_BIT_SET_MASK(p_hw_i2c->intenclr, flags);
}

/**
 * \brief I2C ��ʱֵ����
 * 
 * \param[in] p_hw_i2c : ָ��I2C�Ĵ������ָ��
 * \param[in] value    : I2Cʱ�Ӿ���((value + 1) * 16)�μ�����ʱһ��
 *
 * \return ��
 * \note �����趨ֵͨ��״̬��־��/���жϽ���֪ͨ����Ҫ��I2C����ʱ�ı��
 *       ��ֵ������������г�ʱ����TIMEOUT�Ĵ���д����ֵ������ʹ�ܳ�ʱ��
 */
am_static_inline
void amhw_lpc82x_i2c_timeout_set (amhw_lpc82x_i2c_t *p_hw_i2c, uint32_t value)
{
    uint32_t time_out;
    
    if (value <= 0xFFF) {
        time_out = value;
    } else {
        time_out = 0;
    }
    
    p_hw_i2c->timeout = (AM_SBF(time_out, 4) | 0xF); 
}

/**
 * \brief I2C ��ʱֵ��ȡ
 * \param[in] p_hw_i2c : ָ��I2C�Ĵ������ָ��
 * \return I2C ��ʱֵ
 */
am_static_inline
uint32_t amhw_lpc82x_i2c_timeout_get (amhw_lpc82x_i2c_t *p_hw_i2c)
{
    return (uint32_t)AM_BITS_GET(p_hw_i2c->timeout, 4, 12);
}

/**
 * \brief ��ȡʱ�ӷ�Ƶ�Ĵ���
 * \param[in] p_hw_i2c : ָ��I2C�Ĵ������ָ��
 * \return  ʱ�ӷ�Ƶ�Ĵ�����ֵ
 */
am_static_inline 
uint32_t amhw_lpc82x_i2c_clkdiv_get (amhw_lpc82x_i2c_t *p_hw_i2c)
{
    return (p_hw_i2c->clkdiv);
}

/**
 * \brief �趨ʱ�ӷ�Ƶ����
 *
 * \param[in] p_hw_i2c : ָ��I2C�Ĵ������ָ��
 * \param[in] div      : ʱ�ӷ�Ƶ����
 *
 * \return  ��
 */
am_static_inline 
void amhw_lpc82x_i2c_clkdiv_set (amhw_lpc82x_i2c_t *p_hw_i2c, uint32_t div)
{
    uint32_t clkdiv;
    
    if ((div >= 1) && (div <= 65536)) {
        clkdiv = div - 1;
    } else {
        clkdiv = 0;
    }
    
    p_hw_i2c->clkdiv = clkdiv;
}

/**
 * \brief ��ȡ��ǰ�ж�״̬�ĺ���
 * \param[in] p_hw_i2c : ָ��I2C�Ĵ������ָ��
 * \return  �ж�״̬�Ĵ�����ֵ 
 */
am_static_inline
uint32_t amhw_lpc82x_i2c_intstat_get (amhw_lpc82x_i2c_t *p_hw_i2c)
{
    return (p_hw_i2c->intstat);
}

/**
 * \brief I2C �����������ƺ���
 * \param[in] p_hw_i2c : ָ��I2C�Ĵ������ָ��
 * \return  ��
 */
am_static_inline 
void amhw_lpc82x_i2c_mst_start (amhw_lpc82x_i2c_t *p_hw_i2c)
{
    p_hw_i2c->mstctl = AMHW_LPC82X_I2C_CTL_MSTSTART;  
}

/**
 * \brief I2C ��������������������ӦACK��
 * \param[in] p_hw_i2c : ָ��I2C�Ĵ������ָ��
 * \return    ��
 */
am_static_inline 
void amhw_lpc82x_i2c_mst_continue (amhw_lpc82x_i2c_t *p_hw_i2c)
{
    p_hw_i2c->mstctl = AMHW_LPC82X_I2C_CTL_MSTCONTINUE;  
}

/**
 * \brief I2C ����ֹͣ���ƺ���
 * \param[in] p_hw_i2c : ָ��I2C�Ĵ������ָ��
 * \return  ��
 */
am_static_inline 
void amhw_lpc82x_i2c_mst_stop (amhw_lpc82x_i2c_t *p_hw_i2c)
{
    p_hw_i2c->mstctl = AMHW_LPC82X_I2C_CTL_MSTSTOP;
}

/**
 * \brief I2C ���� DMA ʹ��
 * \param[in] p_hw_i2c : ָ��I2C�Ĵ������ָ��
 * \return  ��
 */
am_static_inline
void amhw_lpc82x_i2c_mst_dma_enable (amhw_lpc82x_i2c_t *p_hw_i2c)
{
    p_hw_i2c->mstctl = AMHW_LPC82X_I2C_CTL_MSTDMA;
}    

/**
 * \brief I2C ���� DMA ����
 * \param[in] p_hw_i2c : ָ��I2C�Ĵ������ָ��
 * \return  ��
 */
am_static_inline
void amhw_lpc82x_i2c_mst_dma_disable (amhw_lpc82x_i2c_t *p_hw_i2c)
{
    p_hw_i2c->mstctl = ~AMHW_LPC82X_I2C_CTL_MSTDMA;
} 

/**
 * \brief �趨MSTTIME��SCL�ߵ�ƽ��SCL�͵�ƽʱ�䣩
 *
 * \param[in] p_hw_i2c : ָ��I2C�Ĵ������ָ��
 * \param[in] scl_h    : SCL�ߵ�ƽʱ��Ϊ scl_h �� I2C Ԥ��Ƶʱ������
 * \param[in] scl_l    : SCL�͵�ƽʱ��Ϊ scl_l �� I2C Ԥ��Ƶʱ������
 *
 * \return  ��
 * \note MSTTIME ʱ�����÷�ΧΪ2 ~ 9 �� I2C Ԥ��Ƶʱ������
 */
am_static_inline 
void amhw_lpc82x_i2c_msttime_set (amhw_lpc82x_i2c_t *p_hw_i2c, 
                                  uint32_t           scl_h, 
                                  uint32_t           scl_l)
{
    p_hw_i2c->msttime = AM_SBF(scl_h - 2, 4) | (scl_l - 2);
}

/**
 * \brief ��ȡMSTTIME��SCL�ߵ�ƽ��SCL�͵�ƽ��
 *
 * \param[in]  p_hw_i2c : ָ��I2C�Ĵ������ָ��
 * \param[out] p_scl_h  : ����SCL�ߵ�ƽʱ��
 * \param[out] p_scl_l  : ����SCL�͵�ƽʱ��
 *
 * \return ��
 */
am_static_inline
void amhw_lpc82x_i2c_msttime_get (amhw_lpc82x_i2c_t *p_hw_i2c,
                                  uint32_t          *p_scl_h,
                                  uint32_t          *p_scl_l)
{
    *p_scl_l = AM_BITS_GET(p_hw_i2c->msttime, 0, 3) + 2;
    *p_scl_h = AM_BITS_GET(p_hw_i2c->msttime, 4, 3) + 2;
}

/**
 * \brief I2C д���ݵ��������ݼĴ���
 *
 * \param[in] p_hw_i2c : ָ��I2C�Ĵ������ָ��
 * \param[in] value    : д���������ݼĴ���������
 *
 * \return  ��
 */
am_static_inline 
void amhw_lpc82x_i2c_mstdat_write (amhw_lpc82x_i2c_t *p_hw_i2c, uint32_t value)
{
    p_hw_i2c->mstdat = value;  
}

/**
 * \brief I2C ��ȡ���ݴ��������ݼĴ���
 * \param[in] p_hw_i2c : ָ��I2C�Ĵ������ָ��
 * \return  �������ݼĴ�����ֵ
 */
am_static_inline 
uint32_t amhw_lpc82x_i2c_mstdat_read (amhw_lpc82x_i2c_t *p_hw_i2c)
{
    return (p_hw_i2c->mstdat);  
}

/**
 * \brief I2C �ӻ�������������ӦACK��
 * \param[in] p_hw_i2c : ָ��I2C�Ĵ������ָ��
 * \return  ��
 */
am_static_inline 
void amhw_lpc82x_i2c_slv_continue (amhw_lpc82x_i2c_t *p_hw_i2c)
{
    p_hw_i2c->slvctl = AMHW_LPC82X_I2C_CTL_SLVCONTINUE;
}

/**
 * \brief I2C �ӻ���Ӧ��(����ͨ��)
 * \param[in] p_hw_i2c : ָ��I2C�Ĵ������ָ��
 * \return  ��
 */
am_static_inline
void amhw_lpc82x_i2c_slv_nack (amhw_lpc82x_i2c_t *p_hw_i2c)
{
    p_hw_i2c->slvctl = AMHW_LPC82X_I2C_CTL_SLVNACK;
}

/**
 * \brief I2C �ӻ�DAMʹ��
 * \param[in] p_hw_i2c : ָ��I2C�Ĵ������ָ��
 * \return  ��
 */
am_static_inline
void amhw_lpc82x_i2c_slv_dma_enable (amhw_lpc82x_i2c_t *p_hw_i2c)
{
    p_hw_i2c->slvctl = AMHW_LPC82X_I2C_CTL_SLVDMA;
}

/**
 * \brief I2C �ӻ�DAM����
 * \param[in] p_hw_i2c : ָ��I2C�Ĵ������ָ��
 * \return  ��
 */
am_static_inline
void amhw_lpc82x_i2c_slv_dma_disable (amhw_lpc82x_i2c_t *p_hw_i2c)
{
    p_hw_i2c->slvctl = ~AMHW_LPC82X_I2C_CTL_SLVDMA;
}

/**
 * \brief д���ݵ��ӻ����ݼĴ���
 *
 * \param[in] p_hw_i2c : ָ��I2C�Ĵ������ָ��
 * \param[in] value    : д��ӻ����ݼĴ���������
 *
 * \return  ��
 */
am_static_inline 
void amhw_lpc82x_i2c_slvdat_write (amhw_lpc82x_i2c_t *p_hw_i2c, uint32_t value)
{
    p_hw_i2c->slvdat = value;  
}

/**
 * \brief I2C ��ȡ���ݴӴӻ����ݼĴ���
 * \param[in] p_hw_i2c : ָ��I2C�Ĵ������ָ��
 * \return  �ӻ����ݼĴ�����ֵ
 */
am_static_inline 
uint32_t amhw_lpc82x_i2c_slvdat_read (amhw_lpc82x_i2c_t *p_hw_i2c)
{
    return (p_hw_i2c->slvdat);  
}

/**
 * \brief I2C �ӻ���ַ���� 
 *
 * \param[in] p_hw_i2c : ָ��I2C�Ĵ������ָ��
 * \param[in] addr_num : �ӻ���ַ��ţ�AMHW_LPC82X_I2C_SLVADR_*��
 * \param[in] sla_addr : 7λ�ӻ���ֵַ
 *
 * \return ��
 */
am_static_inline
void amhw_lpc82x_i2c_slv_addr_set (amhw_lpc82x_i2c_t *p_hw_i2c,
                                   uint32_t           addr_num,
                                   uint32_t           sla_addr)
{
    p_hw_i2c->slvadr[addr_num & 0x3] = AM_SBF(sla_addr, 1);
}

/**
 * \brief ���� I2C �ӻ���ַ��֤�Ĵ���0��ֵ
 * 
 * \param[in] p_hw_i2c : ָ��I2C�Ĵ������ָ��
 * \param[in] flags    : ��Ҫ���õ�ֵ
 *
 * \return ��
 */
am_static_inline
void amhw_lpc82x_i2c_slvqual_set (amhw_lpc82x_i2c_t *p_hw_i2c, uint32_t flags)
{
    p_hw_i2c->slvqual0 = flags;
}

/**
 * \brief ��ȡ I2C �ӻ���ַ��֤�Ĵ���0��ֵ 
 * \param[in] p_hw_i2c : ָ��I2C�Ĵ������ָ��
 * \return �ӻ���ַ��֤�Ĵ���0��ֵ 
 */
am_static_inline
uint32_t amhw_lpc82x_i2c_slvqual_get (amhw_lpc82x_i2c_t *p_hw_i2c)
{
    return (p_hw_i2c->slvqual0);
}

/**
 * \brief ��ȡ I2C ���ģʽ���ݼĴ�����ֵ
 * \param[in] p_hw_i2c : ָ��I2C�Ĵ������ָ��
 * \return ������ݼĴ�����ֵ
 */
am_static_inline
uint32_t amhw_lpc82x_i2c_mon_dat_get (amhw_lpc82x_i2c_t *p_hw_i2c)
{
    return (p_hw_i2c->monrxdat);
}



/* ʹ������������ν��� */
#if defined(__CC_ARM)
  #pragma pop
#elif defined(__ICCARM__)
 
  /* ��������������ʹ�� */
#elif defined(__GNUC__)

  /* Ĭ��ʹ������������ */
#elif defined(__TMS470__)

  /* Ĭ��ʹ������������ */
#elif defined(__TASKING__)
  #pragma warning restore
#else
  #warning Not supported compiler t
#endif


/**
 *@} 
 */

#ifdef __cplusplus
}
#endif

#endif /* __AMHW_LPC82X_I2C_H */

/* end of file */

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
 * \brief ״̬�ɱ�̶�ʱ��(State Configurable Timer) Ӳ�������ӿ�����
 *
 * 1. SCT��ʱ�����ж��ֹ���, ����:
 *       - ���PWM���Σ�
 *       - ���벶��
 *       - ƥ�������
 * 2. 32λ���������Թ�����2��16λ������ģʽ����һ��32λ������ģʽ��
 * 3. �������ļ���ʱ�ӿ�����ϵͳʱ�ӣ�Ҳ������ѡ����ⲿ�ܽ������źš�
 * 4. ���������Թ��������ϼ���ģʽ��Ҳ���Թ���������-���¼���ģʽ��
 * 5. ����������Զ���Ϊһ���¼�:
 *     - ������ƥ��״̬��
 *     - ָ�������� ������������ܽŵĵ�ƽ������أ�״̬��
 *     - ������ƥ��״̬������/���״̬�Ľ�ϣ���/�򣩡�
 * 6. �¼�������״̬�仯��������жϺ�DMA����
 *     - ƥ��Ĵ���0���������Զ��߽����ƣ���Ϊ�������ļ����߽磻
 *     - ��˫��ģʽ��, �¼����Ը��ݼ������򴥷���
 *     - ƥ���¼����Ա���ֱ������һ��ָ�����¼�������
 * 7. �¼��������ƣ�limit������ֹ��halt��, ������start��, ֹͣ��stop����������
 * 8. ֧��:
 *       - 4 ·���룻
 *       - 6 ·�����
 *       - 8 ��ƥ��/����Ĵ�����
 *       - 8 ���¼���
 *       - 8 ��״̬��
 * 9.  4 ·����������Ԥ�ȶ���õĹܽŻ��źš�
 * 10. 6 ·���
 *       - ���0 �� ���5�����������ⲿ�ܽţ�
 *       - ���3 ����������ADC�Ĵ������븴�þ���
 *
 * \internal
 * \par Modification history
 * - 1.01 15-11-24  hgo, replace macro definition.
 * - 1.00 15-07-07  oce, first implementation.
 * \endinternal
 */

#ifndef __AMHW_LPC82X_SCT_H
#define __AMHW_LPC82X_SCT_H

#include "ametal.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup amhw_lpc82x_if_sct
 * \copydoc amhw_lpc82x_sct.h
 * @{
 */

/**
 * \name SCTģ������
 * @{
 */

#define AMHW_LPC82X_SCT_EVT_NUM           (8)  /**< \brief ֧�ֵ��¼�����           */
#define AMHW_LPC82X_SCT_MATCAP_REG_NUM    (8)  /**< \brief ƥ��/����Ĵ�������  */
#define AMHW_LPC82X_SCT_STATE_NUM         (8)  /**< \brief ֧�ֵ�״̬����           */
#define AMHW_LPC82X_SCT_OUT_NUM           (6)  /**< \brief ֧�ֵ����·��           */
#define AMHW_LPC82X_SCT_IN_NUM            (4)  /**< \brief ֧�ֵ�����·��           */
#define AMHW_LPC82X_SCT_DMA_REQ_NUM       (2)  /**< \brief ֧�ֵ�DMA������         */

/** @} */

/* ʹ�������ṹ�������������Ŀ�ʼ */
#if defined(__CC_ARM)
    #pragma push
    #pragma anon_unions
#elif defined(__ICCARM__)
    #pragma language=extended
#elif defined(__GNUC__)
#elif defined(__TMS470__)
#elif defined(__TASKING__)
    #pragma warning 586
#else
    #warning Not supported compiler t
#endif

/**
 * \brief ״̬�ɱ�̶�ʱ���Ĵ�����ṹ��
 */
typedef struct amhw_lpc82x_sct {
    __IO uint32_t config;          /**< \brief ���üĴ���                     */
                                                                              
    union {                                                                   
        __IO uint32_t ctrl_u;      /**< \brief ���ƼĴ���                     */
        struct {                                                              
            __IO uint16_t ctrl_l;  /**< \brief ���ƼĴ�����16λ               */
            __IO uint16_t ctrl_h;  /**< \brief ���ƼĴ�����16λ               */
        };                                                                    
    };                                                                        
                                                                              
    __IO uint16_t limit_l;         /**< \brief ���ƼĴ�����16λ               */
    __IO uint16_t limit_h;         /**< \brief ���ƼĴ�����16λ               */
    __IO uint16_t halt_l;          /**< \brief ��ֹ�Ĵ�����16λ               */
    __IO uint16_t halt_h;          /**< \brief ��ֹ�Ĵ�����16λ               */
    __IO uint16_t stop_l;          /**< \brief ֹͣ�Ĵ�����16λ               */
    __IO uint16_t stop_h;          /**< \brief ֹͣ�Ĵ�����16λ               */
    __IO uint16_t start_l;         /**< \brief �����Ĵ�����16λ               */
    __IO uint16_t start_h;         /**< \brief �����Ĵ�����16λ               */
                                                                              
    uint32_t reserved1[10];        /**< \brief ��������Ӧʹ��                 */
                                                                              
    union {                                                                   
        __IO uint32_t count_u;     /**< \brief �������Ĵ���                   */
        struct {                                                              
            __IO uint16_t count_l; /**< \brief �������Ĵ�����16λ             */
            __IO uint16_t count_h; /**< \brief �������Ĵ�����16λ             */
        };                                                                    
    };                                                                        
                                                                              
    __IO uint16_t state_l;         /**< \brief ״̬�Ĵ�����16λ               */
    __IO uint16_t state_h;         /**< \brief ״̬�Ĵ�����16λ               */
    __I  uint32_t input;           /**< \brief ����Ĵ���,ֻ��                */
                                   
    __IO uint16_t regmode_l;       /**< \brief ƥ��/����ģʽѡ��Ĵ�����16λ  */
    __IO uint16_t regmode_h;       /**< \brief ƥ��/����ģʽѡ��Ĵ�����16λ  */
    __IO uint32_t output;          /**< \brief ����Ĵ���                     */
    __IO uint32_t outputdirctrl;   /**< \brief ���������ƼĴ���        */
                                   
    __IO uint32_t res;             /**< \brief ��ͻ����Ĵ���                 */

    union {

        __IO uint32_t dmareq[2];   /**< \brief DMA����Ĵ�����            */
        struct {
            __IO uint32_t dmareq0; /**< \brief DMA0 ����Ĵ���            */
            __IO uint32_t dmareq1; /**< \brief DMA1 ����Ĵ���            */
        };
    };

         uint32_t reserved2[35];   /**< \brief ��������Ӧʹ��                 */

    __IO uint32_t even;            /**< \brief �¼�ʹ�ܼĴ���                 */
    __IO uint32_t evflag;          /**< \brief �¼���־�Ĵ���                 */
    __IO uint32_t conen;           /**< \brief "�ޱ仯��ͻ�¼�"�ж�ʹ�ܼĴ��� */
    __IO uint32_t conflag;         /**< \brief "�ޱ仯��ͻ�¼�"״̬�Ĵ���     */

    union {

        __IO union {               /**< \brief ƥ��Ĵ���                     */
            __IO uint32_t u;       /**< \brief ƥ��Ĵ�������״̬32λ         */
            struct {
                __IO uint16_t l;   /**< \brief ƥ��Ĵ�����16λ               */
                __IO uint16_t h;   /**< \brief ƥ��Ĵ�����16λ               */
            };

        } match[AMHW_LPC82X_SCT_MATCAP_REG_NUM];

        __I union {                /**< \brief ����Ĵ���                     */
            __IO uint32_t u;       /**< \brief ����Ĵ�������״̬32λ         */
            struct {               
                __IO uint16_t l;   /**< \brief ����Ĵ�����16λ               */
                __IO uint16_t h;   /**< \brief ����Ĵ�����16λ               */
            };
        } cap[AMHW_LPC82X_SCT_MATCAP_REG_NUM];
    };
    /** \brief ���� */
    uint32_t reserved3[48 + (16 - AMHW_LPC82X_SCT_MATCAP_REG_NUM)];

    union {

        __IO union {              /**< \brief ƥ����ת�ؼĴ���                */
            __IO uint32_t u;      /**< \brief ƥ����ת�ؼĴ�������״̬32λ    */

            struct {
                __IO uint16_t l;  /**< \brief ƥ����ת�ؼĴ�����16λ          */
                __IO uint16_t h;  /**< \brief ƥ����ת�ؼĴ�����16λ          */
            };

        } matchrel[AMHW_LPC82X_SCT_MATCAP_REG_NUM];

        __IO union {              /**< \brief ������ƼĴ���                  */
            __IO uint32_t u;      /**< \brief ������ƼĴ�������״̬32λ      */

            struct {
                __IO uint16_t l;  /**< \brief ������ƼĴ�����16λ            */
                __IO uint16_t h;  /**< \brief ������ƼĴ�����16λ            */
            };
        } capctrl[AMHW_LPC82X_SCT_MATCAP_REG_NUM];

    };
    /** \brief ���� */
    uint32_t reserved6[48 + (16 - AMHW_LPC82X_SCT_MATCAP_REG_NUM)];

    __IO struct {
        __IO uint32_t state;      /**< \brief �¼�״̬�Ĵ���                  */
        __IO uint32_t ctrl;       /**< \brief �¼����ƼĴ���                  */
    } event[AMHW_LPC82X_SCT_EVT_NUM];  /**< \brief �¼�״̬�����ƼĴ�����          */

    /** \brief ����        */
    uint32_t reserved9[128 - 2 * AMHW_LPC82X_SCT_EVT_NUM];

    __IO struct {
        __IO uint32_t set;        /**< \brief �����λ�Ĵ���                  */
        __IO uint32_t clr;        /**< \brief �������Ĵ���                  */
    } out[AMHW_LPC82X_SCT_OUT_NUM];   /**< \brief �����λ������Ĵ�����          */

} amhw_lpc82x_sct_t;

/**
 * \name SCT��Դʹ������
 * @{
 */

/** \brief ���ڶ����¼���,nȡֵΪ��0 ~ AMHW_LPC82X_SCT_EVT_NUM - 1 */
#define AMHW_LPC82X_SCT_EVT(n)       (n)

/** \brief ���ڶ���״̬��,nȡֵΪ��0 ~ AMHW_LPC82X_SCT_STATE_NUM - 1  */
#define AMHW_LPC82X_SCT_STATE(n)     (n)

/** \brief ����ʹ�õĲ���Ĵ����ţ�nȡֵΪ��0 ~ AMHW_LPC82X_SCT_MATCAP_REG_NUM - 1 */
#define AMHW_LPC82X_SCT_CAP(n)       (n)

/** \brief ����ʹ�õ�ƥ��Ĵ����ţ�nȡֵΪ��0 ~ AMHW_LPC82X_SCT_MATCAP_REG_NUM - 1 */
#define AMHW_LPC82X_SCT_MAT(n)       (n)

/** \brief �������ͨ����,nȡֵΪ��0 ~ AMHW_LPC82X_SCT_OUT_NUM - 1          */
#define AMHW_LPC82X_SCT_OUT(n)       (n)

/** \brief ��������ͨ����,nȡֵΪ��0 ~ AMHW_LPC82X_SCT_IN_NUM - 1           */
#define AMHW_LPC82X_SCT_IN(n)        (n)

/**
 * @}
 */

/**
 * \name SCT���ú�
 * @{
 */

/** \brief ��ʱ��������������2��16λ��ʱ����ģʽ�� */
#define AMHW_LPC82X_SCT_CFG_16BIT_COUNTER                   0x00000000

/** \brief ��ʱ��������������1��32λ��ʱ����ģʽ�� */
#define AMHW_LPC82X_SCT_CFG_32BIT_COUNTER                   0x00000001

/**
  * \brief ��ֹƥ��Ĵ����ĸ�16λ�͵�16λ��ָ������ת�ؼĴ�������ת��
  *        ������������������1��32λ��ʱ����ģʽ��ʹ�á�
  */
#define AMHW_LPC82X_SCT_CFG_NORELOAD_U                      AM_SBF(1UL, 7)

/**
  * \brief ��ֹƥ��Ĵ����ĵ�16λ��ָ������ת�ؼĴ�������ת��
  *        ������������������2��16λ��ʱ����ģʽ��ʹ�á�
  */
#define AMHW_LPC82X_SCT_CFG_NORELOAD_L                      AM_SBF(1UL, 7)

/**
  * \brief ��ֹƥ��Ĵ����ĸ�16λ��ָ������ת�ؼĴ�������ת��
  *        ������������������2��16λ��ʱ����ģʽ��ʹ�á�
  */
#define AMHW_LPC82X_SCT_CFG_NORELOAD_H                      AM_SBF(1UL, 8)

/** \brief ϵͳʱ����ΪSCT����ʱ�ӣ�ʱ��ģʽ0��                               */
#define AMHW_LPC82X_SCT_CFG_CLKMODE_SYSCLK                  AM_SBF(0UL, 1)

/** \brief Ԥ��Ƶ��ϵͳʱ����ΪSCT����ʱ�� ��ʱ��ģʽ1��                      */
#define AMHW_LPC82X_SCT_CFG_CLKMODE_PRESCALED_SYSCLK        AM_SBF(1UL, 1)

/** \brief SCT���루��CKSELѡ������룩��ΪSCT����ʱ�ӣ�ʱ��ģʽ2��           */
#define AMHW_LPC82X_SCT_CFG_CLKMODE_SCT_INPUT               AM_SBF(2UL, 1)

/** \brief Ԥ��Ƶ��SCT���루��CKSELѡ������룩��ΪSCT����ʱ�ӣ�ʱ��ģʽ3��   */
#define AMHW_LPC82X_SCT_CFG_CLKMODE_PRESCALED_SCT_INPUT     AM_SBF(3UL, 1)

/**
 * @}
 */

/**
 * \name Ϊʱ��ģʽ2��ʱ��ģʽ3ѡ��ʱ������
 * @{
 */
/** \brief ʱ������ѡ��SCT����ͨ��0��������   */
#define AMHW_LPC82X_SCT_CFG_CKSEL_RISING_IN_0    AM_SBF(0UL, 3)

/** \brief ʱ������ѡ��SCT����ͨ��0���½���   */
#define AMHW_LPC82X_SCT_CFG_CKSEL_FALLING_IN_0   AM_SBF(1UL, 3)

/** \brief ʱ������ѡ��SCT����ͨ��1��������   */
#define AMHW_LPC82X_SCT_CFG_CKSEL_RISING_IN_1    AM_SBF(2UL, 3)

/** \brief ʱ������ѡ��SCT����ͨ��1���½���   */
#define AMHW_LPC82X_SCT_CFG_CKSEL_FALLING_IN_1   AM_SBF(3UL, 3)

/** \brief ʱ������ѡ��SCT����ͨ��2��������   */
#define AMHW_LPC82X_SCT_CFG_CKSEL_RISING_IN_2    AM_SBF(4UL, 3)

/** \brief ʱ������ѡ��SCT����ͨ��2���½���   */
#define AMHW_LPC82X_SCT_CFG_CKSEL_FALLING_IN_2   AM_SBF(5UL, 3)

/** \brief ʱ������ѡ��SCT����ͨ��3��������   */
#define AMHW_LPC82X_SCT_CFG_CKSEL_RISING_IN_3    AM_SBF(6UL, 3)

/** \brief ʱ������ѡ��SCT����ͨ��3���½���   */
#define AMHW_LPC82X_SCT_CFG_CKSEL_FALLING_IN_3   AM_SBF(7UL, 3)

/** @} */


/**
 * \name ͬ�����룬�����������¼������ź�֮ǰ��������SCTʱ��ͬ��
 *
 * ��ʱ��ģʽ2��ʱ��ģʽ3�£�������δ��ʹ�á�
 * @{
 */
/** \brief ͬ������0        */
#define AMHW_LPC82X_SCT_CFG_INSYNC_0     AM_SBF(1UL, 9)

/** \brief ͬ������1        */
#define AMHW_LPC82X_SCT_CFG_INSYNC_1     AM_SBF(1UL, 10)

/** \brief ͬ������2        */
#define AMHW_LPC82X_SCT_CFG_INSYNC_2     AM_SBF(1UL, 11)

/** \brief ͬ������3        */
#define AMHW_LPC82X_SCT_CFG_INSYNC_3     AM_SBF(1UL, 12)

/** \brief ͬ����������     */
#define AMHW_LPC82X_SCT_CFG_INSYNC_ALL   AM_SBF(0xFUL, 9)

/** @} */

/**
 * \name ƥ��Ĵ���0�ϵ�ƥ��״̬��Ϊ��ʱ�����������Զ�����״̬
 * @{
 */
/** \brief 32λ������ģʽ�£��������Զ��޶�     */
#define AMHW_LPC82X_SCT_CFG_AUTOLIMIT_U  AM_SBF(1UL, 17)

/** \brief 16λ������ģʽ�£������������Զ��޶�     */
#define AMHW_LPC82X_SCT_CFG_AUTOLIMIT_L  AM_SBF(1UL, 17)

/** \brief 16λ������ģʽ�£������������Զ��޶�     */
#define AMHW_LPC82X_SCT_CFG_AUTOLIMIT_H  AM_SBF(1UL, 18)

/** @} */

/**
 * \name ����SCT���Ƶĺ궨��
 * @{
 */
/** \brief ֹͣ��16λ��ʱ��������״̬32λ��ʱ�� */
#define AMHW_LPC82X_SCT_CTRL_STOP_L    AM_SBF(1UL, 1)

/** \brief ��ֹ��16λ��ʱ��������״̬32λ��ʱ�� */
#define AMHW_LPC82X_SCT_CTRL_HALT_L    AM_SBF(1UL, 2)

/** \brief �����16λ������������״̬32λ������ */
#define AMHW_LPC82X_SCT_CTRL_CLRCTR_L  AM_SBF(1UL, 3)

/**
 * \brief ��16λ��ʱ��������״̬32λ��ʱ�����ϼ�����������״̬��Ȼ������
 */
#define AMHW_LPC82X_SCT_CTRL_BIDIR_L_COUNT_UP       AM_SBF(0UL, 4)

/**
 * \brief ��16λ��ʱ��������״̬32λ��ʱ�����ϼ�����������״̬��Ȼ�����¼�����0
 */
#define AMHW_LPC82X_SCT_CTRL_BIDIR_L_COUNT_UP_DOWN  AM_SBF(1UL, 4)

/** \brief ��16λ��ʱ��������״̬32λ��ʱ����ʱ��Ԥ��Ƶֵ */
#define AMHW_LPC82X_SCT_CTRL_PRE_L(x)               AM_SBF(((x) & 0xFF), 5)

/** \brief ֹͣ��16λ��ʱ��    */
#define AMHW_LPC82X_SCT_CTRL_STOP_H     AM_SBF(1UL, 17)

/** \brief ��ֹ��16λ��ʱ��    */
#define AMHW_LPC82X_SCT_CTRL_HALT_H     AM_SBF(1UL, 18)

/** \brief �����16λ������    */
#define AMHW_LPC82X_SCT_CTRL_CLRCTR_H   AM_SBF(1UL, 19)

/**
 * \brief ��16λ��ʱ�����ϼ�����������״̬��Ȼ������
 */
#define AMHW_LPC82X_SCT_CTRL_BIDIR_H_CONT_UP        AM_SBF(0UL, 20)

/**
 * \brief ��16λ��ʱ�����ϼ�����������״̬�����¼�����0
 */
#define AMHW_LPC82X_SCT_CTRL_BIDIR_H_CONT_UP_DOWN   AM_SBF(1UL, 20)

/** \brief ��16λ��ʱ����ʱ��Ԥ��Ƶֵ  */
#define AMHW_LPC82X_SCT_CTRL_PRE_H(x)               AM_SBF((x) & 0xFF, 21)

/**
 * @}
 */

/**
 * \name �����¼����ƼĴ����ĺ궨��
 * @{
 */

/** \brief ѡ�����ӵ����¼���ƥ��Ĵ���                                */
#define AMHW_LPC82X_SCT_EVT_CTRL_MATCHSEL(reg)   AM_SBF((reg) & 0xF, 0)

/** \brief ָ�����¼�����L�飬ʹ��״̬�Ĵ����ĵ�16λ��ƥ��Ĵ�����16λ */
#define AMHW_LPC82X_SCT_EVT_CTRL_HEVENT_L        AM_SBF(0UL, 4)

/** \brief ָ�����¼�����H�飬ʹ��״̬�Ĵ����ĸ�16λ��ƥ��Ĵ�����16λ */
#define AMHW_LPC82X_SCT_EVT_CTRL_HEVENT_H        AM_SBF(1UL, 4)

/** \brief IO�¼�ΪIOSELѡ��������ź�     */
#define AMHW_LPC82X_SCT_EVT_CTRL_OUTSEL_INPUT    AM_SBF(0UL, 5)

/** \brief IO�¼�ΪIOSELѡ�������ź�     */
#define AMHW_LPC82X_SCT_EVT_CTRL_OUTSEL_OUTPUT   AM_SBF(1UL, 5)

/** \brief ѡ������¼��������IO�ź�      */
#define AMHW_LPC82X_SCT_EVT_CTRL_IOSEL(signal)   AM_SBF((signal) & 0xF, 6)

/** \brief ���¼�ͨ�������㣨����STATEVֵ���ı�״ֵ̬  */
#define AMHW_LPC82X_SCT_EVT_CTRL_STATELD_ADD     AM_SBF(0UL, 14)

/** \brief ���¼�ͨ��ֱ��ת��STATEVֵ�ı�״ֵ̬        */
#define AMHW_LPC82X_SCT_EVT_CTRL_STATELD_LOAD    AM_SBF(1UL, 14)

/**
 * \brief ���� STATEV ֵ
 *
 *  �������¼��ǵ�ǰ״̬�����������¼����¼������ʱ���ſ���ͨ����ֵ�ı�״ֵ̬
 *  ��� STATELD �� STATEV ֵ��Ϊ0, ��״̬���ᷢ���ı�
 */
#define AMHW_LPC82X_SCT_EVT_CTRL_STATEV(x)       AM_SBF((x) & 0x1F, 15)

/**
 * \name �����¼��ļ���״̬
 * @{
 */

/**
 * \brief ���ʹ���˸ñ�־�������¼�����Դ�а�����ƥ���¼��������򵱼���������ʱ��
 *        ֻҪ����ֵ���ڻ������Ӧƥ��Ĵ���ֵ����ƥ���¼�����Ϊ��Ч̬���򵱼�����
 *        ���¼���ʱ������ֵС�ڻ������Ӧƥ��Ĵ���ֵ��ƥ���¼�����Ϊ��Ч̬��
 */
#define AMHW_LPC82X_SCT_EVT_CTRL_MATCHMEM_1                AM_SBF(1UL, 20)

/**
 * \brief ���ʹ���˸ñ�־�������¼�����Դ�а�����ƥ���¼�����. ���ҽ�������ֵ
 *        ������Ӧƥ��Ĵ���ֵʱ��ƥ���¼��ű���Ϊ����Ч̬��
 */
#define AMHW_LPC82X_SCT_EVT_CTRL_MATCHMEM_0                AM_SBF(0UL, 20)

/** @} */

/** \brief �¼����������Զ�ʱ���ļ������� */
#define AMHW_LPC82X_SCT_EVT_CTRL_DIRECTION_INDEPENDENT     AM_SBF(0UL, 21)

/** \brief ��˫�����ģʽ�У��������ϼ���ʱ�����¼��Żᱻ����  */
#define AMHW_LPC82X_SCT_EVT_CTRL_DIRECTION_UP              AM_SBF(1UL, 21)

/** \brief ��˫�����ģʽ�У��������¼���ʱ�����¼��Żᱻ����  */
#define AMHW_LPC82X_SCT_EVT_CTRL_DIRECTION_DOWN            AM_SBF(2UL, 21)

/**
 * @}
 */

/**
 * \name Ϊ�¼�ѡ��IO����״̬
 * @{
 */
/** \brief �͵�ƽ����  */
#define AMHW_LPC82X_SCT_EVT_CTRL_IOCOND_LOW      AM_SBF(0UL, 10)

/** \brief �����ش���  */
#define AMHW_LPC82X_SCT_EVT_CTRL_IOCOND_RISE     AM_SBF(1UL, 10)

/** \brief �½��ش���  */
#define AMHW_LPC82X_SCT_EVT_CTRL_IOCOND_FALL     AM_SBF(2UL, 10)

/** \brief �ߵ�ƽ����  */
#define AMHW_LPC82X_SCT_EVT_CTRL_IOCOND_HIGH     AM_SBF(3UL, 10)

/** @} */

/**
 * \name ָ��ƥ��״̬��IO״̬�����¼�����Ϸ�ʽ
 * @{
 */
 
/** \brief ��ʽ������һ��״̬�����������¼� */
#define AMHW_LPC82X_SCT_EVT_CTRL_COMBMODE_OR      AM_SBF(0UL, 12)

/** \brief ��ƥ��״̬�����Ŵ����¼�           */
#define AMHW_LPC82X_SCT_EVT_CTRL_COMBMODE_MATCH   AM_SBF(1UL, 12)

/** \brief ��IO״̬�����Ŵ����¼�             */
#define AMHW_LPC82X_SCT_EVT_CTRL_COMBMODE_IO      AM_SBF(2UL, 12)

/** \brief ƥ��״̬��IO״̬�������Ŵ����¼�   */
#define AMHW_LPC82X_SCT_EVT_CTRL_COMBMODE_AND     AM_SBF(3UL, 12)

/** @} */

/**
 * \name SCT��ͻ����궨��
 * @{
 */
#define AMHW_LPC82X_SCT_RES_OUT_NOCHANGE    (0)    /**< \brief ��ͻʱ�ޱ仯     */
#define AMHW_LPC82X_SCT_RES_OUT_SET         (1)    /**< \brief ��ͻʱ��λ���   */
#define AMHW_LPC82X_SCT_RES_OUT_CLR         (2)    /**< \brief ��ͻʱ�������   */
#define AMHW_LPC82X_SCT_RES_OUT_TOG         (3)    /**< \brief ��ͻʱ��ת���   */
/**
 * @}
 */

/**
 * \name SCT˫�����ʱ���������
 * @{
 */

/** \brief ��λ��������Զ�ʱ���ļ�������                                     */
#define AMHW_LPC82X_SCT_OUTPUTDIRCTRL_INDEPENDENT   (0)

/** \brief ��λ�������ǵߵ��ģ�����16λ��ʱ��������״̬32λ��ʱ�����¼���ʱ   */
#define AMHW_LPC82X_SCT_OUTPUTDIRCTRL_L_REVERSED    (1)

/** \brief ��λ�������ǵߵ��ģ�����16λ��ʱ�����¼���ʱ                       */
#define AMHW_LPC82X_SCT_OUTPUTDIRCTRL_H_REVERSED    (2)

/** @} */

/**
 * \brief SCT ģʽ��־
 *
 * �ñ�־��������ĳЩ�Ĵ���ʱ��ָ����ǰ���õ�SCT����ģʽ����SCT������1��32λ��ʱ
 * ��ģʽ��ʱ��ģʽ��־��Ϊ:AMHW_LPC82X_SCT_MODE_UNIFY����SCT������2��16λ��ʱ��ģʽ��
 * ʱ�������üĴ���ʱ����Ҫָ���������ǵ�16λ(AMHW_LPC82X_SCT_MODE_SEPARATE_L)���Ǹ�
 * 16λ(AMHW_LPC82X_SCT_MODE_SEPARATE_H)
 */
typedef enum amhw_lpc82x_sct_mode {
    AMHW_LPC82X_SCT_MODE_UNIFY      = 0, /**< \brief ����ģʽ                     */
    AMHW_LPC82X_SCT_MODE_SEPARATE_L,     /**< \brief 2��16λ��ʱ��ģʽʱ����16λ  */
    AMHW_LPC82X_SCT_MODE_SEPARATE_H      /**< \brief 2��16λ��ʱ��ģʽʱ����16λ  */
} amhw_lpc82x_sct_mode_t;

/**
 * \brief ƥ��/���� ģʽ
 *
 *  ��������ĳЩ�Ĵ�����ģʽ��ָ���乤����ƥ��״̬���ǲ���״̬
 */
typedef enum amhw_lpc82x_sct_matcap_mode {
    AMHW_LPC82X_SCT_MATCAP_MATCH  = 0,   /**< \brief �Ĵ���������ƥ��״̬ */
    AMHW_LPC82X_SCT_MATCAP_CAPTURE       /**< \brief �Ĵ��������ڲ���״̬ */
} amhw_lpc82x_sct_matcap_mode_t;


/**
 * \brief ����SCT
 *
 * \param[in] p_hw_sct  : ָ��SCT�Ĵ������ָ��
 * \param[in] cfg_flags : AMHW_LPC82X_SCT_CFG_* ��ֵ����
 *                        AMHW_LPC82X_SCT_CFG_*��Ļ�
 *                        (OR)ֵ(#AMHW_LPC82X_SCT_CFG_32BIT_COUNTER)
 * \return ��
 */
am_static_inline
void amhw_lpc82x_sct_config (amhw_lpc82x_sct_t *p_hw_sct, uint32_t cfg_flags)
{
    p_hw_sct->config = cfg_flags;
}

/**
 * \brief ����SCT
 *
 * \param[in] p_hw_sct   : ָ��SCT�Ĵ������ָ��
 * \param[in] ctrl_flags : AMHW_LPC82X_SCT_CTRL_*��ֵ����
 *                         AMHW_LPC82X_SCT_CTRL_*��Ļ�
 *                         (OR)ֵ(#AMHW_LPC82X_SCT_CTRL_HALT_L)
 * \return ��
 *
 * \note ��SCTֹͣ����ֹʱ������λ�����Խ���д����������SCT����ʱ��ֻ��STOP��HALTλ
 *       ���Բ����������������λ����Ҫ�Ƚ�HALTλ��1.
 */
am_static_inline
void amhw_lpc82x_sct_ctrl (amhw_lpc82x_sct_t *p_hw_sct, uint32_t ctrl_flags)
{
    p_hw_sct->ctrl_u = ctrl_flags;
}

/**
 * \brief ���SCT�Ƿ�����ֹ(HALT)״̬
 *
 * \param[in] p_hw_sct : ָ��SCT�Ĵ������ָ��
 * \param[in] mode     : SCTģʽ
 *
 * \retval  TRUE  : ������ֹ(HALT)״̬
 * \retval  FALSE : û�д�����ֹ(HALT)״̬
 */
am_static_inline
bool_t amhw_lpc82x_sct_halt_check (amhw_lpc82x_sct_t     *p_hw_sct,
                                   amhw_lpc82x_sct_mode_t mode)
{
    if (mode == AMHW_LPC82X_SCT_MODE_SEPARATE_H) {
        return (bool_t)((p_hw_sct->ctrl_u & AMHW_LPC82X_SCT_CTRL_HALT_H) != 0);
    } else {
        return (bool_t)((p_hw_sct->ctrl_u & AMHW_LPC82X_SCT_CTRL_HALT_L) != 0);
    }
}

/**
 * \brief ���SCT�Ƿ���ֹͣ(STOP)״̬
 *
 * \param[in] p_hw_sct : ָ��SCT�Ĵ������ָ��
 * \param[in] mode     : SCTģʽ
 *
 * \retval  TRUE  : ����ֹͣ(STOP)״̬
 * \retval  FALSE : û�д���ֹͣ(STOP)״̬
 */
am_static_inline
bool_t amhw_lpc82x_sct_stop_check (amhw_lpc82x_sct_t      *p_hw_sct,
                                   amhw_lpc82x_sct_mode_t  mode)
{
    if (mode == AMHW_LPC82X_SCT_MODE_SEPARATE_H) {
        return (bool_t)((p_hw_sct->ctrl_u & AMHW_LPC82X_SCT_CTRL_STOP_H) != 0);
    } else {
        return (bool_t)((p_hw_sct->ctrl_u & AMHW_LPC82X_SCT_CTRL_STOP_L) != 0);
    }
}

/**
 * \brief ���ÿ��ƼĴ�������ӦλΪ1
 *
 * \param[in] p_hw_sct : ָ��SCT�Ĵ������ָ��
 * \param[in] value    : �����ֵ��λnΪ1����Ὣ���ƼĴ�������Ӧλ��1��0��Ӱ��
 *
 * \return ��
 */
am_static_inline
void amhw_lpc82x_sct_ctrl_set (amhw_lpc82x_sct_t *p_hw_sct, uint32_t value)
{
    AM_BIT_SET_MASK(p_hw_sct->ctrl_u, value);
}

/**
 * \brief ������ƼĴ�������Ӧλ
 *
 * \param[in] p_hw_sct : ָ��SCT�Ĵ������ָ��
 * \param[in] value    : �����ֵ��λnΪ1����Ὣ���ƼĴ�������Ӧλ���㡣0��Ӱ��
 *
 * \return ��
 */
am_static_inline
void amhw_lpc82x_sct_ctrl_clr (amhw_lpc82x_sct_t *p_hw_sct, uint32_t value)
{
    AM_BIT_CLR_MASK(p_hw_sct->ctrl_u, value);
}

/**
 * \brief ��SCT����ֹ(HALT)̬�л���ֹͣ(STOP)̬(�����ᴦ������̬)
 *
 * \param[in] p_hw_sct : ָ��SCT�Ĵ������ָ��
 * \param[in] mode     : SCTģʽ
 *
 * \return ��
 */
void amhw_lpc82x_sct_ctrl_halt_to_stop (amhw_lpc82x_sct_t      *p_hw_sct,
                                        amhw_lpc82x_sct_mode_t  mode);

/**
 * \brief ����SCT��Ԥ��Ƶֵ
 *
 * \param[in] p_hw_sct : ָ��SCT�Ĵ������ָ��
 * \param[in] mode     : SCTģʽ
 * \param[in] prescale : ��������ʱ��Ƶ����SCTʱ��Ƶ�ʳ���(prescale + 1)
 *
 * \return ��
 */
void amhw_lpc82x_sct_prescale_set (amhw_lpc82x_sct_t     *p_hw_sct,
                                   amhw_lpc82x_sct_mode_t mode,
                                   uint8_t                prescale);

/**
 * \brief ��ȡSCT��Ԥ��Ƶֵ
 *
 * \param[in] p_hw_sct : ָ��SCT�Ĵ������ָ��
 * \param[in] mode     : SCTģʽ
 *
 * \return SCT��Ԥ��Ƶֵ
 */
uint8_t amhw_lpc82x_sct_prescale_get (amhw_lpc82x_sct_t *p_hw_sct,
                                      amhw_lpc82x_sct_mode_t mode);

/**
 * \brief ����SCT��һ���¼�
 *
 * \param[in] p_hw_sct : ָ��SCT�Ĵ������ָ��
 * \param[in] evt_num  : �¼��ţ�0 ~ AMHW_LPC82X_SCT_EVT_NUM - 1
 * \param[in] flags    : AMHW_LPC82X_SCT_EVT_CTRL_*��ֵ����
 *                       AMHW_LPC82X_SCT_EVT_CTRL_*��Ļ�(OR)
 *                       ֵ��(#AMHW_LPC82X_SCT_EVT_CTRL_COMBMODE_MATCH)
 *                       
 * \return ��
 */
am_static_inline
void amhw_lpc82x_sct_evt_ctrl (amhw_lpc82x_sct_t *p_hw_sct,
                               uint32_t           evt_num,
                               uint32_t           flags)
{
    p_hw_sct->event[evt_num].ctrl = flags;
}

/**
 * \brief ����ʹ��һ���¼���ĳЩ״̬�з���
 *
 * \param[in] p_hw_sct  : ָ��SCT�Ĵ������ָ��
 * \param[in] evt_num   : �¼��ţ�0 ~ AMHW_LPC82X_SCT_EVT_NUM - 1
 * \param[in] states    : ״̬������states��bit0����״̬0��bit0=1ʱ���¼���״̬0ʹ�ܣ�
 *                        bit1=1ʱ���¼���״̬1ʹ�ܣ�����λ������
 *
 * \return ��
 */
am_static_inline
void amhw_lpc82x_sct_evt_state_cfg (amhw_lpc82x_sct_t *p_hw_sct,
                                    uint32_t           evt_num,
                                    uint32_t           states)
{
    p_hw_sct->event[evt_num].state = states;
}

/**
 * \brief ʹ��һ���¼���ָ��״̬�з���
 *
 * \param[in] p_hw_sct  : ָ��SCT�Ĵ������ָ��
 * \param[in] evt_num   : �¼��ţ�0 ~ AMHW_LPC82X_SCT_EVT_NUM - 1
 * \param[in] state_num : ״̬�ţ�0 ~ AMHW_LPC82X_SCT_STATE_NUM - 1
 *
 * \return ��
 */
am_static_inline
void amhw_lpc82x_sct_evt_state_enable (amhw_lpc82x_sct_t *p_hw_sct,
                                       uint32_t           evt_num,
                                       uint32_t           state_num)
{
    AM_BIT_SET(p_hw_sct->event[evt_num].state, state_num);
}

/**
 * \brief ����һ���¼���ָ��״̬�з���
 *
 * \param[in] p_hw_sct  : ָ��SCT�Ĵ������ָ��
 * \param[in] evt_num   : �¼��ţ�0 ~ AMHW_LPC82X_SCT_EVT_NUM - 1
 * \param[in] state_num : ״̬�ţ�0 ~ AMHW_LPC82X_SCT_STATE_NUM - 1
 *
 * \return ��
 */
am_static_inline
void amhw_lpc82x_sct_evt_state_disable (amhw_lpc82x_sct_t *p_hw_sct,
                                        uint32_t           evt_num,
                                        uint32_t           state_num)
{
    AM_BIT_CLR(p_hw_sct->event[evt_num].state, state_num);
}

/**
 * \brief ʹ��һ���¼���Ϊ������������״̬
 *
 * \param[in] p_hw_sct : ָ��SCT�Ĵ������ָ��
 * \param[in] mode     : SCTģʽ
 * \param[in] evt_num  : �¼��ţ�0 ~ AMHW_LPC82X_SCT_EVT_NUM - 1
 *
 * \return ��
 */
am_static_inline
void amhw_lpc82x_sct_evt_limit_enable (amhw_lpc82x_sct_t     *p_hw_sct,
                                       amhw_lpc82x_sct_mode_t mode,
                                       uint32_t               evt_num)
{
    if (mode == AMHW_LPC82X_SCT_MODE_SEPARATE_H) {
        AM_BIT_SET(p_hw_sct->limit_h, evt_num);
    } else {
        AM_BIT_SET(p_hw_sct->limit_l, evt_num);
    }
}

/**
 * \brief ����һ���¼���Ϊ������������״̬
 *
 * \param[in] p_hw_sct : ָ��SCT�Ĵ������ָ��
 * \param[in] mode     : SCTģʽ
 * \param[in] evt_num  : �¼��ţ�0 ~ AMHW_LPC82X_SCT_EVT_NUM - 1
 *
 * \return ��
 */
am_static_inline
void amhw_lpc82x_sct_evt_limit_disable (amhw_lpc82x_sct_t     *p_hw_sct,
                                        amhw_lpc82x_sct_mode_t mode,
                                        uint32_t               evt_num)
{
    if (mode == AMHW_LPC82X_SCT_MODE_SEPARATE_H) {
        AM_BIT_CLR(p_hw_sct->limit_h, evt_num);
    } else {
        AM_BIT_CLR(p_hw_sct->limit_l, evt_num);
    }
}

/**
 * \brief ʹ��һ���¼�����ʱ��ֹ��ʱ��
 *
 * \param[in] p_hw_sct : ָ��SCT�Ĵ������ָ��
 * \param[in] mode     : SCTģʽ
 * \param[in] evt_num  : �¼��ţ�0 ~ AMHW_LPC82X_SCT_EVT_NUM - 1
 *
 * \return ��
 */
am_static_inline
void amhw_lpc82x_sct_evt_halt_enable (amhw_lpc82x_sct_t      *p_hw_sct,
                                      amhw_lpc82x_sct_mode_t  mode,
                                      uint32_t                evt_num)
{
    if (mode == AMHW_LPC82X_SCT_MODE_SEPARATE_H) {
        AM_BIT_SET(p_hw_sct->halt_h, evt_num);
    } else {
        AM_BIT_SET(p_hw_sct->halt_l, evt_num);
    }
}

/**
 * \brief ����һ���¼�����ʱ��ֹ��ʱ��
 *
 * \param[in] p_hw_sct : ָ��SCT�Ĵ������ָ��
 * \param[in] mode     : SCTģʽ
 * \param[in] evt_num  : �¼��ţ�0 ~ AMHW_LPC82X_SCT_EVT_NUM - 1
 *
 * \return ��
 */
am_static_inline
void amhw_lpc82x_sct_evt_halt_disable (amhw_lpc82x_sct_t      *p_hw_sct,
                                       amhw_lpc82x_sct_mode_t  mode,
                                       uint32_t                evt_num)
{
    if (mode == AMHW_LPC82X_SCT_MODE_SEPARATE_H) {
        AM_BIT_CLR(p_hw_sct->halt_h, evt_num);
    } else {
        AM_BIT_CLR(p_hw_sct->halt_l, evt_num);
    }
}

/**
 * \brief ʹ��һ���¼�����ʱֹͣ��ʱ��
 *
 * \param[in] p_hw_sct : ָ��SCT�Ĵ������ָ��
 * \param[in] mode     : SCTģʽ
 * \param[in] evt_num  : �¼��ţ�0 ~ AMHW_LPC82X_SCT_EVT_NUM - 1
 *
 * \return ��
 */
am_static_inline
void amhw_lpc82x_sct_evt_stop_enable (amhw_lpc82x_sct_t      *p_hw_sct,
                                      amhw_lpc82x_sct_mode_t  mode,
                                      uint32_t                evt_num)
{
    if (mode == AMHW_LPC82X_SCT_MODE_SEPARATE_H) {
        AM_BIT_SET(p_hw_sct->stop_h, evt_num);
    } else {
        AM_BIT_SET(p_hw_sct->stop_l, evt_num);
    }
}

/**
 * \brief ����һ���¼�����ʱֹͣ��ʱ��
 *
 * \param[in] p_hw_sct : ָ��SCT�Ĵ������ָ��
 * \param[in] mode     : SCTģʽ
 * \param[in] evt_num  : �¼��ţ�0 ~ AMHW_LPC82X_SCT_EVT_NUM - 1
 *
 * \return ��
 */
am_static_inline
void amhw_lpc82x_sct_evt_stop_disable (amhw_lpc82x_sct_t      *p_hw_sct,
                                       amhw_lpc82x_sct_mode_t  mode,
                                       uint32_t                evt_num)
{
    if (mode == AMHW_LPC82X_SCT_MODE_SEPARATE_H) {
        AM_BIT_CLR(p_hw_sct->stop_h, evt_num);
    } else {
        AM_BIT_CLR(p_hw_sct->stop_l, evt_num);
    }
}

/**
 * \brief ʹ��һ���¼�����ʱ������ʱ��
 *
 * \param[in] p_hw_sct : ָ��SCT�Ĵ������ָ��
 * \param[in] mode     : SCTģʽ
 * \param[in] evt_num  : �¼��ţ�0 ~ AMHW_LPC82X_SCT_EVT_NUM - 1
 *
 * \return ��
 */
am_static_inline
void amhw_lpc82x_sct_evt_start_enable (amhw_lpc82x_sct_t      *p_hw_sct,
                                       amhw_lpc82x_sct_mode_t  mode,
                                       uint32_t                evt_num)
{
    if (mode == AMHW_LPC82X_SCT_MODE_SEPARATE_H) {
        AM_BIT_SET(p_hw_sct->start_h, evt_num);
    } else {
        AM_BIT_SET(p_hw_sct->start_l, evt_num);
    }
}

/**
 * \brief ����һ���¼�����ʱ������ʱ��
 *
 * \param[in] p_hw_sct : ָ��SCT�Ĵ������ָ��
 * \param[in] mode     : SCTģʽ
 * \param[in] evt_num  : �¼��ţ�0 ~ AMHW_LPC82X_SCT_EVT_NUM - 1
 *
 * \return ��
 */
am_static_inline
void amhw_lpc82x_sct_evt_start_disable (amhw_lpc82x_sct_t      *p_hw_sct,
                                        amhw_lpc82x_sct_mode_t  mode,
                                        uint32_t                evt_num)
{
    if (mode == AMHW_LPC82X_SCT_MODE_SEPARATE_H) {
        AM_BIT_CLR(p_hw_sct->start_h, evt_num);
    } else {
        AM_BIT_CLR(p_hw_sct->start_l, evt_num);
    }
}

/**
 * \brief ����SCT�ļ���(count)ֵ
 *
 * \param[in] p_hw_sct : ָ��SCT�Ĵ������ָ��
 * \param[in] mode     : SCTģʽ.
 * \param[in] value    : ����(count)ֵ
 *
 * \note ���ü���ֵʱ������ȷ����Ӧ�ļ���������HALT״̬�����SCTģʽ��
 *       #AMHW_LPC82X_SCT_MODE_SEPARATE_L ���� #AMHW_LPC82X_SCT_MODE_SEPARATE_H
 *       ����(count)ֵ������16λ��Ч
 */
void amhw_lpc82x_sct_count_set (amhw_lpc82x_sct_t     *p_hw_sct,
                                amhw_lpc82x_sct_mode_t mode,
                                uint32_t               value);

/**
 * \brief ��ȡ��ʱ���ĵ�ǰ����ֵ
 *
 * \param[in] p_hw_sct : ָ��SCT�Ĵ������ָ��
 * \param[in] mode     : SCTģʽ
 *
 * \return ��ʱ���ĵ�ǰ����ֵ�����SCTģʽ��#AMHW_LPC82X_SCT_MODE_SEPARATE_L
 *         ���� #AMHW_LPC82X_SCT_MODE_SEPARATE_H�� ����(count)ֵ������16λ��Ч
 */
uint32_t amhw_lpc82x_sct_count_get (amhw_lpc82x_sct_t     *p_hw_sct,
                                    amhw_lpc82x_sct_mode_t mode);

/**
 * \brief ��ȡ��ǰ��״ֵ̬
 *
 * \param[in] p_hw_sct : ָ��SCT�Ĵ������ָ��
 * \param[in] mode     : SCTģʽ
 *
 * \return ��ǰ��״ֵ̬
 */
am_static_inline
uint16_t amhw_lpc82x_sct_state_get (amhw_lpc82x_sct_t     *p_hw_sct,
                                    amhw_lpc82x_sct_mode_t mode)
{
    if (mode == AMHW_LPC82X_SCT_MODE_SEPARATE_H) {
        return p_hw_sct->state_h;
    } else {
        return p_hw_sct->state_l;
    }
}

/**
 * \brief ����״ֵ̬
 *
 * \param[in] p_hw_sct : ָ��SCT�Ĵ������ָ��
 * \param[in] mode     : SCTģʽ
 * \param[in] value    : ״ֵ̬
 *
 * \note ����״ֵ̬�ǣ�����ȷ����Ӧ�ļ�����������ֹ(HALT)״̬
 */
am_static_inline
void amhw_lpc82x_sct_state_set (amhw_lpc82x_sct_t     *p_hw_sct,
                                amhw_lpc82x_sct_mode_t mode,
                                uint16_t               value)
{
    if (mode == AMHW_LPC82X_SCT_MODE_SEPARATE_H) {
        p_hw_sct->state_h = value;
    } else {
        p_hw_sct->state_l = value;
    }
}

/**
 * \brief ��ȡSCT������״̬
 *
 * \param[in] p_hw_sct : ָ��SCT�Ĵ������ָ��
 *
 * \return ����״̬�� λ0~λ7   : AIN0 ~ AIN7(ֱ�Ӷ�ȡ������״̬)
 *                    λ16~λ23 : SIN0 ~ SIN7(����״̬)
 *
 * \note SIN0 ~ SIN7:
 *        - ������üĴ����У�INSYNC λΪ1������Ӧ������״̬SIN��ͬ���������״̬��
 *          ����3��SCTʱ��ͬ������˻�õ�һ����ԭ�ź��ӳ�3��SCTʱ�ӵ��ȶ��źš�
 *        - ������üĴ����У�INSYNC λΪ0������Ӧ������״̬SIN��ֱ�Ӷ�ȡ������
 *          ״̬AIN��ͬ��
 */
am_static_inline
uint32_t amhw_lpc82x_sct_in_state_get (amhw_lpc82x_sct_t *p_hw_sct)
{
     return p_hw_sct->input;
}

/**
 * \brief ��ȡSCT�����״̬
 *
 * \param[in] p_hw_sct : ָ��SCT�Ĵ������ָ��
 *
 * \return ���״̬. λ0 ~ λ7: ���0 ~ ���7
 *           - ������״̬Ϊ�ߵ�ƽ������ӦλΪ1
 *           - ������״̬Ϊ�͵�ƽ������ӦλΪ0
 */
am_static_inline
uint32_t amhw_lpc82x_sct_out_state_get (amhw_lpc82x_sct_t *p_hw_sct)
{
     return p_hw_sct->output;
}

/**
 * \brief ����SCT�����״̬
 *
 * \param[in] p_hw_sct : ָ��SCT�Ĵ������ָ��
 * \param[in] state    : λ n Ϊ1�������ͨ��n����ߵ�ƽ
 *                       λ n Ϊ0�������ͨ��n����͵�ƽ
 *
 * \return ��
 *
 * \note дSCT����Ĵ�������ȷ�����еļ�����������ֹ(HALT)״̬
 */
am_static_inline
void amhw_lpc82x_sct_out_state_config (amhw_lpc82x_sct_t *p_hw_sct,
                                       uint8_t            state)
{
     p_hw_sct->output = state;
}

/**
 * \brief ����ָ�������ͨ������ߵ�ƽ
 *
 * \param[in] p_hw_sct : ָ��SCT�Ĵ������ָ��
 * \param[in] out_num  : ���ͨ���ţ�0 ~ AMHW_LPC82X_SCT_OUT_NUM - 1
 *
 * \return  ��
 *
 * \note дSCT����Ĵ�������ȷ�����еļ�����������ֹ(HALT)״̬
 */
am_static_inline
void amhw_lpc82x_sct_out_state_set (amhw_lpc82x_sct_t *p_hw_sct,
                                    uint8_t            out_num)
{
     AM_BIT_SET(p_hw_sct->output, out_num);
}

/**
 * \brief ����ָ�������ͨ������͵�ƽ
 *
 * \param[in] p_hw_sct   : ָ��SCT�Ĵ������ָ��
 * \param[in] out_num    : ���ͨ���ţ�0 ~ AMHW_LPC82X_SCT_OUT_NUM - 1
 *
 * \return  ��
 *
 * \note дSCT����Ĵ�������ȷ�����еļ�����������ֹ(HALT)״̬
 */
am_static_inline
void amhw_lpc82x_sct_out_state_clr (amhw_lpc82x_sct_t *p_hw_sct,
                                    uint8_t            out_num)
{
     AM_BIT_CLR(p_hw_sct->output, out_num);
}

/**
 * \brief SCT˫�����ʱ���������
 *
 * \param[in] p_hw_sct : ָ��SCT�Ĵ������ָ��
 * \param[in] out_num  : ���ͨ���ţ�0 ~ AMHW_LPC82X_SCT_OUT_NUM - 1
 * \param[in] value    : ����ֵ��
 *                         - #AMHW_LPC82X_SCT_OUTPUTDIRCTRL_INDEPENDENT
 *                         - #AMHW_LPC82X_SCT_OUTPUTDIRCTRL_L_REVERSED
 *                         - #AMHW_LPC82X_SCT_OUTPUTDIRCTRL_R_REVERSED
 *
 * \return  ��
 */
void amhw_lpc82x_sct_out_dir_ctrl (amhw_lpc82x_sct_t *p_hw_sct,
                                   uint8_t            out_num,
                                   uint8_t            value);

/**
 * \brief ���������ͻ���
 *
 * \param[in] p_hw_sct   : ָ��SCT�Ĵ������ָ��
 * \param[in] out_num : ���ͨ���ţ�0 ~ AMHW_LPC82X_SCT_OUT_NUM - 1
 * \param[in] value      : ��ͻ�����ʽ��
 *                          - #AMHW_LPC82X_SCT_RES_OUT_NOCHANGE : ����ޱ仯
 *                          - #AMHW_LPC82X_SCT_RES_OUT_SET      : ����ߵ�ƽ
 *                          - #AMHW_LPC82X_SCT_RES_OUT_CLR      : ����͵�ƽ
 *                          - #AMHW_LPC82X_SCT_RES_OUT_TOG      : ��ת�����ƽ
 * \return ��
 */
void amhw_lpc82x_sct_conflict_res_config (amhw_lpc82x_sct_t *p_hw_sct,
                                          uint8_t            out_num,
                                          uint8_t            value);


/**
 * \brief ʹ���¼���DMA����
 *
 * \param[in] p_hw_sct : ָ��SCT�Ĵ������ָ��
 * \param[in] req_num  : DMA�����,0 ~ AMHW_LPC82X_SCT_DMA_REQ_NUM - 1
 * \param[in] evt_num  : �¼��ţ�0 ~ AMHW_LPC82X_SCT_EVT_NUM - 1
 *
 * \return  ��
 */
am_static_inline
void amhw_lpc82x_sct_dma_req_evt_enable (amhw_lpc82x_sct_t *p_hw_sct,
                                         uint8_t            req_num,
                                         uint32_t           evt_num)
{
    AM_BIT_SET(p_hw_sct->dmareq[req_num], evt_num);
}

/**
 * \brief �����¼���DMA����
 *
 * \param[in] p_hw_sct : ָ��SCT�Ĵ������ָ��
 * \param[in] req_num  : DMA�����,0 ~ AMHW_LPC82X_SCT_DMA_REQ_NUM - 1
 * \param[in] evt_num  : �¼��ţ�0 ~ AMHW_LPC82X_SCT_EVT_NUM - 1
 *
 * \return  ��
 */
am_static_inline
void amhw_lpc82x_sct_dma_req_evt_disable (amhw_lpc82x_sct_t *p_hw_sct,
                                          uint8_t            req_num,
                                          uint32_t           evt_num)
{
    AM_BIT_CLR(p_hw_sct->dmareq[req_num], evt_num);
}

/**
 * \brief ��ƥ��Ĵ���ֵ����Ӧ����װ�ؼĴ�������ת��ʱ������DMA����
 *
 * \param[in] p_hw_sct : ָ��SCT�Ĵ������ָ��
 * \param[in] req_num  : DMA�����,0 ~ AMHW_LPC82X_SCT_DMA_REQ_NUM - 1
 *
 * \return  ��
 */
am_static_inline
void amhw_lpc82x_sct_dma_req_reload_enable (amhw_lpc82x_sct_t *p_hw_sct,
                                            uint8_t            req_num)
{
    AM_BIT_SET(p_hw_sct->dmareq[req_num], 30);
}

/**
 * \brief ��ƥ��Ĵ���ֵ����Ӧ����װ�ؼĴ�������ת��ʱ��������DMA����
 *
 * \param[in] p_hw_sct : ָ��SCT�Ĵ������ָ��
 * \param[in] req_num  : DMA�����,0 ~ AMHW_LPC82X_SCT_DMA_REQ_NUM - 1
 *
 * \return  ��
 */
am_static_inline
void amhw_lpc82x_sct_dma_req_reload_disable (amhw_lpc82x_sct_t *p_hw_sct,
                                             uint8_t            req_num)
{
    AM_BIT_CLR(p_hw_sct->dmareq[req_num], 30);
}

/**
 * \brief ���DMA�����־�Ƿ���λ
 *
 * \param[in] p_hw_sct : ָ��SCT�Ĵ������ָ��
 * \param[in] req_num  : DMA�����,0 ~ AMHW_LPC82X_SCT_DMA_REQ_NUM - 1
 *
 * \retval  TRUE  : DMA�����־��λ
 * \retval  FALSE : DMA�����־δ��λ
 */
am_static_inline
bool_t amhw_lpc82x_sct_dma_req_check (amhw_lpc82x_sct_t *p_hw_sct,
                                      uint8_t            req_num)
{
    return (bool_t)(AM_BIT_ISSET(p_hw_sct->dmareq[req_num], 31) != 0);
}

/**
 * \brief ʹ��ָ���¼����ж�����
 *
 * \param[in] p_hw_sct : ָ��SCT�Ĵ������ָ��
 * \param[in] evt_num  : �¼��ţ�0 ~ AMHW_LPC82X_SCT_EVT_NUM - 1
 *
 * \return  ��
 */
am_static_inline
void amhw_lpc82x_sct_evt_int_enable (amhw_lpc82x_sct_t *p_hw_sct,
                                     uint32_t           evt_num)
{
    AM_BIT_SET(p_hw_sct->even, evt_num);
}

/**
 * \brief ����ָ���¼����ж�����
 *
 * \param[in] p_hw_sct : ָ��SCT�Ĵ������ָ��
 * \param[in] evt_num  : �¼��ţ�0 ~ AMHW_LPC82X_SCT_EVT_NUM - 1
 *
 * \return  ��
 */
am_static_inline
void amhw_lpc82x_sct_evt_int_disable (amhw_lpc82x_sct_t *p_hw_sct,
                                      uint32_t           evt_num)
{
    AM_BIT_CLR(p_hw_sct->even, evt_num);
}

/**
 * \brief ��ȡ�¼���־�Ĵ���ֵ
 * \param[in] p_hw_sct : ָ��SCT�Ĵ������ָ��
 * \return �¼���־�Ĵ���ֵ����ӦλΪ1��������Ӧ�¼�����
 */
am_static_inline
uint32_t amhw_lpc82x_sct_evt_flag_get (amhw_lpc82x_sct_t *p_hw_sct)
{
    return p_hw_sct->evflag;
}

/**
 * \brief ���ָ���¼��Ƿ���
 *
 * \param[in] p_hw_sct : ָ��SCT�Ĵ������ָ��
 * \param[in] evt_num  : �¼��ţ�0 ~ AMHW_LPC82X_SCT_EVT_NUM - 1
 *
 * \retval  TRUE  : �¼�����
 * \retval  FALSE : �¼�δ����
 */
am_static_inline
bool_t amhw_lpc82x_sct_evt_flag_chk (amhw_lpc82x_sct_t *p_hw_sct,
                                     uint32_t           evt_num)
{
    return (bool_t)(AM_BIT_ISSET(p_hw_sct->evflag, evt_num) != 0);
}

/**
 * \brief ���ָ���¼��ķ�����־
 *
 * \param[in] p_hw_sct : ָ��SCT�Ĵ������ָ��
 * \param[in] evt_num  : �¼��ţ�0 ~ AMHW_LPC82X_SCT_EVT_NUM - 1
 *
 * \return  ��
 */
am_static_inline
void amhw_lpc82x_sct_evt_flag_clr (amhw_lpc82x_sct_t *p_hw_sct,
                                   uint32_t           evt_num)
{
    AM_BIT_SET(p_hw_sct->evflag, evt_num);
}

/**
 * \brief ʹ�������ͻʱ�����ж�
 *
 * \param[in] p_hw_sct   : ָ��SCT�Ĵ������ָ��
 * \param[in] out_num    : ���ͨ���ţ�0 ~ AMHW_LPC82X_SCT_OUT_NUM - 1
 * \param[in] out_num    : ���ͨ���ţ�0 ~ AMHW_LPC82X_SCT_OUT_NUM - 1
 *
 * \return  ��
 */
am_static_inline
void amhw_lpc82x_sct_conflict_int_enable (amhw_lpc82x_sct_t *p_hw_sct,
                                          uint8_t            out_num)
{
    AM_BIT_SET(p_hw_sct->conen, out_num);
}

/**
 * \brief ���������ͻʱ�����ж�
 *
 * \param[in] p_hw_sct   : ָ��SCT�Ĵ������ָ��
 * \param[in] out_num    : ���ͨ���ţ�0 ~ AMHW_LPC82X_SCT_OUT_NUM - 1
 *
 * \return  ��
 */
am_static_inline
void amhw_lpc82x_sct_conflict_int_disable (amhw_lpc82x_sct_t *p_hw_sct,
                                           uint8_t            out_num)
{
    AM_BIT_CLR(p_hw_sct->conen, out_num);
}

/**
 * \brief ��ȡ��ͻ��־
 * \param[in] p_hw_sct  : ָ��SCT�Ĵ������ָ��
 * \return ��ͻ��־. ��������n�����ޱ仯��ͻ�¼�����(��������ĳ�ͻ�������Ϊ
           �ޱ仯)��λ n (0 ~ 7) Ϊ1
 *
 * \note  λ30Ϊ����λ����������ߴ��󣨵���16λ������������״̬�µ�32λ������δ
 *        ����HALT״̬ʱ,�Բ�Ӧ��д������ļĴ�������STATE,MATCH��������д���������
 *        ���λΪ1��
 *        λ31Ϊ����λ����������ߴ��󣨵���16λ������δ����HALT״̬ʱ,�Բ�Ӧ��
 *        д������ļĴ�������STATE,MATCH��������д������������λΪ1��
 */
am_static_inline
uint32_t amhw_lpc82x_sct_conflict_flag_get (amhw_lpc82x_sct_t *p_hw_sct)
{
    return p_hw_sct->conflag;
}

/**
 * \brief �����ͻ��־
 *
 * \param[in] p_hw_sct : ָ��SCT�Ĵ������ָ��
 * \param[in] flags    : λnΪ1���������λ�ϵ���Ӧ��־
 *
 * \return ��
 */
am_static_inline
void amhw_lpc82x_sct_conflict_flag_clr (amhw_lpc82x_sct_t *p_hw_sct,
                                        uint32_t           flags)
{
    AM_BIT_SET(p_hw_sct->conflag, flags);
}

/**
 * \brief SCTƥ��/����Ĵ���ģʽ����
 *
 * \param[in] p_hw_sct    : ָ��SCT�Ĵ������ָ��
 * \param[in] sct_mode    : SCTģʽ
 * \param[in] matcap_num  : ƥ��/����Ĵ����ţ�0~AMHW_LPC82X_SCT_MATCAP_REG_NUM-1
 * \param[in] matcap_mode : ���õ�ģʽ��
 *                           - AMHW_LPC82X_SCT_MATCAP_MATCH,   ����Ϊƥ��ģʽ
 *                           - AMHW_LPC82X_SCT_MATCAP_CAPTURE, ����Ϊ����ģʽ
 * \return  ��
 */
void amhw_lpc82x_sct_regmode_config (amhw_lpc82x_sct_t            *p_hw_sct,
                                     amhw_lpc82x_sct_mode_t        sct_mode,
                                     uint32_t                      matcap_num,
                                     amhw_lpc82x_sct_matcap_mode_t matcap_mode);


/**
 * \brief ����ƥ��Ĵ���ֵ
 *
 * \param[in] p_hw_sct : ָ��SCT�Ĵ������ָ��
 * \param[in] mode     : SCTģʽ
 * \param[in] mat_num  : ƥ��Ĵ����ţ� 0 ~ AMHW_LPC82X_SCT_MATCAP_REG_NUM - 1
 * \param[in] value    : ���õ�ֵ
 *
 * \return  ��
 *
 * \note ����ֵ֮ǰ������ȷ����ģʽ����Ӧ����������HALT״̬�����SCTģʽΪ
 *       AMHW_LPC82X_SCT_MODE_SEPARATE_L ����AMHW_LPC82X_SCT_MODE_SEPARATE_H
 *       valueֵ����16Ϊ��Ч
 */
void amhw_lpc82x_sct_mat_val_set (amhw_lpc82x_sct_t     *p_hw_sct,
                                  amhw_lpc82x_sct_mode_t mode,
                                  uint32_t               mat_num,
                                  uint32_t               value);

/**
 * \brief ����ƥ����ת�ؼĴ�����ֵ
 *
 * \param[in] p_hw_sct : ָ��SCT�Ĵ������ָ��
 * \param[in] mode     : SCTģʽ
 * \param[in] mat_num  : ƥ����ת�ؼĴ����ţ� 0 ~ AMHW_LPC82X_SCT_MATCAP_REG_NUM-1
 * \param[in] value    : ���õ�ֵ
 *
 * \return ��
 *
 * \note ����ֵ֮ǰ������ȷ����ģʽ����Ӧ����������HALT״̬�����SCTģʽΪ
 *       AMHW_LPC82X_SCT_MODE_SEPARATE_L ����AMHW_LPC82X_SCT_MODE_SEPARATE_H
 *       valueֵ����16Ϊ��Ч
 */
void amhw_lpc82x_sct_mat_reload_val_set (amhw_lpc82x_sct_t     *p_hw_sct,
                                         amhw_lpc82x_sct_mode_t mode,
                                         uint32_t               mat_num,
                                         uint32_t               value);
/**
 * \brief ���ò���ʹ�ܻ�����¼���������Ĵ���ֵ������ֵװ��
 *
 * \param[in] p_hw_sct : ָ��SCT�Ĵ������ָ��
 * \param[in] sct_mode : SCTģʽ
 * \param[in] cap_num  : ����Ĵ����ţ�0 ~ AMHW_LPC82X_SCT_MATCAP_REG_NUM - 1
 * \param[in] evt_num  : ����������¼��ţ�0 ~ AMHW_LPC82X_SCT_EVT_NUM - 1
 *
 * \param[in] active   : TRUE=>  ʹ���¼���������
 *                       FALSE=> �����¼���������
 * \return  ��
 */
void amhw_lpc82x_sct_cap_ctrl (amhw_lpc82x_sct_t      *p_hw_sct,
                               amhw_lpc82x_sct_mode_t  sct_mode,
                               uint32_t                cap_num,
                               uint32_t                evt_num,
                               bool_t                  active);

/**
 * \brief ��ȡ����ֵ
 *
 * \param[in] p_hw_sct : ָ��SCT�Ĵ������ָ��
 * \param[in] sct_mode : SCTģʽ
 * \param[in] cap_num  : ����Ĵ����ţ�0 ~ AMHW_LPC82X_SCT_MATCAP_REG_NUM - 1
 *
 * \return ����ֵ
 *         ���SCTģʽ�� AMHW_LPC82X_SCT_MODE_SEPARATE_L ����
 *                       AMHW_LPC82X_SCT_MODE_SEPARATE_H
 *         �򷵻�ֵ������16λ��Ч
 */
uint32_t amhw_lpc82x_sct_cap_val_get (amhw_lpc82x_sct_t      *p_hw_sct,
                                      amhw_lpc82x_sct_mode_t  sct_mode,
                                      uint32_t                cap_num);

/**
 * \brief ʹ��ָ���¼���ָ�����ͨ����Ϊ�ߵ�ƽ
 *
 * \param[in] p_hw_sct   : ָ��SCT�Ĵ������ָ��
 * \param[in] evt_num    : �¼��ţ�    0 ~ AMHW_LPC82X_SCT_EVT_NUM - 1
 * \param[in] out_num    : ���ͨ���ţ�0 ~ AMHW_LPC82X_SCT_OUT_NUM - 1
 *
 * \return ��
 */
am_static_inline
void amhw_lpc82x_sct_out_set_enable (amhw_lpc82x_sct_t *p_hw_sct,
                                     uint8_t           out_num,
                                     uint32_t          evt_num)
{
    AM_BIT_SET(p_hw_sct->out[out_num].set, evt_num);
}

/**
 * \brief ����ָ���¼���ָ�����ͨ����Ϊ�ߵ�ƽ
 *
 * \param[in] p_hw_sct   : ָ��SCT�Ĵ������ָ��
 * \param[in] evt_num    : �¼��ţ�    0 ~ AMHW_LPC82X_SCT_EVT_NUM - 1
 * \param[in] out_num    : ���ͨ���ţ�0 ~ AMHW_LPC82X_SCT_OUT_NUM - 1
 *
 * \return ��
 */
am_static_inline
void amhw_lpc82x_sct_out_set_disable (amhw_lpc82x_sct_t *p_hw_sct,
                                      uint8_t            out_num,
                                      uint32_t           evt_num)
{
    AM_BIT_CLR(p_hw_sct->out[out_num].set, evt_num);
}

/**
 * \brief ʹ��ָ���¼���ָ�����ͨ����Ϊ�͵�ƽ
 *
 * \param[in] p_hw_sct   : ָ��SCT�Ĵ������ָ��
 * \param[in] evt_num    : �¼��ţ�    0 ~ AMHW_LPC82X_SCT_EVT_NUM - 1
 * \param[in] out_num    : ���ͨ���ţ�0 ~ AMHW_LPC82X_SCT_OUT_NUM - 1
 *
 * \return ��
 */
am_static_inline
void amhw_lpc82x_sct_out_clr_enable (amhw_lpc82x_sct_t *p_hw_sct,
                                     uint8_t            out_num,
                                     uint32_t           evt_num)
{
    AM_BIT_SET(p_hw_sct->out[out_num].clr, evt_num);
}

/**
 * \brief ����ָ���¼���ָ�����ͨ����Ϊ�͵�ƽ
 *
 * \param[in] p_hw_sct : ָ��SCT�Ĵ������ָ��
 * \param[in] evt_num  : �¼��ţ�    0 ~ AMHW_LPC82X_SCT_EVT_NUM - 1
 * \param[in] out_num  : ���ͨ���ţ�0 ~ AMHW_LPC82X_SCT_OUT_NUM - 1
 *
 * \return ��
 */
am_static_inline
void amhw_lpc82x_sct_out_clr_disable (amhw_lpc82x_sct_t *p_hw_sct,
                                      uint8_t            out_num,
                                      uint32_t           evt_num)
{
    AM_BIT_CLR(p_hw_sct->out[out_num].clr, evt_num);
}


/* ʹ�������ṹ�������������Ľ��� */
#if defined(__CC_ARM)
    #pragma pop
#elif defined(__ICCARM__)
#elif defined(__GNUC__)
#elif defined(__TMS470__)
#elif defined(__TASKING__)
    #pragma warning restore
#else
    #warning Not supported compiler t
#endif

/**
 * @} amhw_lpc82x_if_sct
 */

#ifdef __cplusplus
}
#endif

#endif  /* __AMHW_LPC82X_SCT_H */

/* end of file */



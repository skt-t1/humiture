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
 * \brief �����ʱ���ӿں���ʵ��
 * 
 * \internal
 * \par Modification history
 * - 1.00 15-08-03  tee, first implementation.
 * \endinternal
 */

#include "am_softimer.h"
#include "am_common.h"
#include "am_int.h"

/** \brief ���������ʱ����Ӳ����ʱ������Ƶ�ʣ���������ʱ��Ƶ�ʲ���Ϊ0 */
static unsigned int __g_hwtimer_freq = 0;

/** \brief �����ʱ�������ͷ */
static struct am_list_head g_softimer_head;

/******************************************************************************/
static unsigned int __ms_to_ticks (unsigned int ms)
{
    if (__g_hwtimer_freq != 0) {
        return  AM_DIV_ROUND_UP(__g_hwtimer_freq * ms, 1000u);
    }
    return 0;
}

/******************************************************************************/
static void __softimer_add (am_softimer_t *p_timer, unsigned int ticks)
{
    struct am_list_head  *p;
    am_softimer_t        *p_timer_iterator = NULL;
    
    am_list_for_each(p, &g_softimer_head) {
        p_timer_iterator = am_list_entry(p, am_softimer_t, node);
        if (ticks >= p_timer_iterator->ticks ) {  /* ��ͬʱ��Ӧ�����ڽڵ���� */
            ticks -= p_timer_iterator->ticks;
        } else {
            break;
        }
    }
    p_timer->ticks = ticks;

    am_list_add_tail( &p_timer->node, p);         /* ����ȷ��λ�ò��붨ʱ��   */
    
    if (p != &g_softimer_head ) {                 /* p��ָ��ʱ����ʱҪ����    */
        p_timer_iterator->ticks -= ticks;
    }
}

/******************************************************************************/
static void __softimer_remove (am_softimer_t *p_timer)
{
    am_softimer_t *p_next = NULL;

    if (am_list_empty(&p_timer->node) ) {         /* �ڵ�δ����������         */
        return ;
    }
    
    /* ��ǰ�ڵ㲻�����һ���ڵ�, ɾ���ýڵ�ǰ������һ���ڵ����ʱtick���� */
    if ( (&p_timer->node)->next != &g_softimer_head ) {
        p_next = am_list_entry((&p_timer->node)->next, am_softimer_t, node);
        p_next->ticks += p_timer->ticks;
    }
    
    am_list_del_init(&p_timer->node);
}

/******************************************************************************/
static void __softimer_hwtimer_callback (void *p_arg)
{
    int old = am_int_cpu_lock();
 
    if ( !am_list_empty( &g_softimer_head ) ) {

        struct am_list_head *p;
        am_softimer_t       *p_timer;
        
        /* ָ���һ����ЧԪ�ؽڵ� */
        p       = (&g_softimer_head)->next;
        p_timer = am_list_entry(p, am_softimer_t, node);
        
        /* 
         * �����׽ڵ�ֵ��1
         */
        if (p_timer->ticks) {
            p_timer->ticks--;
        }

        while (!am_list_empty(&g_softimer_head)) {
            
            /* 
             * �����һ����ЧԪ��,�����ý���Ȼ�����ڵ�һ�����
             * (�����ڵ�һ����㣬��ֵҲ����Ϊ 0�����´�ɨ��ͻ��˳�)
             */
            if (p_timer->ticks == 0) {

                /* �ýڵ㱾�ζ�ʱʱ�䵽��ɾ���ýڵ�                  */
                am_list_del_init(&p_timer->node);
                
                /* �����ڻص�������ֹͣ������Ƚ���������ӽ�������  */
                __softimer_add(p_timer, p_timer->repeat_ticks); 
                
                if (p_timer->timeout_callback ) {
                    p_timer->timeout_callback(p_timer->p_arg);
                }
                
                /* ������һ���ڵ㣬�����ж���һ���׽ڵ�ʱ���Ƿ�Ϊ0 */
                p       = (&g_softimer_head)->next;
                p_timer = am_list_entry(p, am_softimer_t, node);
                
                
            /* ֻҪ������Ϊ0�Ľ�㣬���˳�ѭ�� */
            } else {
                break;
            }
        }
    }
    am_int_cpu_unlock(old);
}

/******************************************************************************/
int am_softimer_module_init (am_timer_handle_t handle,
                             uint8_t           chan,
                             unsigned int      freq)
{
    am_timer_info_t hwtimer_info;
    
    AM_INIT_LIST_HEAD(&g_softimer_head);
 
    if (handle == NULL) {
        return -AM_EINVAL;
    }
    
    if (am_timer_info_get(handle, &hwtimer_info) != AM_OK) {
        return -AM_EINVAL;
    }
    
    /*  ��֧�ֵ�ͨ��     */
    if (chan > AM_TIMER_CHAN_NUM_GET(hwtimer_info.features)) {
        return -AM_EINVAL;
    }
    
    /* ��֧�ֵ�����Ƶ��  */
    if (freq < hwtimer_info.min_frequency || 
        freq > hwtimer_info.max_frequency) {
        return -AM_EINVAL;
    }
    
    /* �����жϻص����� */
    am_timer_callback_set(handle, chan, __softimer_hwtimer_callback, NULL);

    __g_hwtimer_freq = (hwtimer_info.clk_frequency) / \
                       (hwtimer_info.clk_frequency  / freq); 
    
    return am_timer_enable(handle, chan, hwtimer_info.clk_frequency / freq);
}
    
/******************************************************************************/
int am_softimer_init (am_softimer_t *p_timer, 
                      am_pfnvoid_t   p_func,
                      void          *p_arg)
{
    int old;
    
    if (__g_hwtimer_freq == 0) {                      /* Ӳ����ʱ��δ�������� */
        return -AM_EPERM;
    }
    
    AM_INIT_LIST_HEAD(&p_timer->node);

    old = am_int_cpu_lock();
    
    p_timer->timeout_callback = p_func;
    p_timer->p_arg            = p_arg;
    
    am_int_cpu_unlock(old);
    
    return AM_OK;
}

/******************************************************************************/
void am_softimer_start (am_softimer_t *p_timer, unsigned int ms)
{
    int old;
    unsigned int ticks =  __ms_to_ticks(ms);
    old = am_int_cpu_lock();
    
    /* ticks��СֵΪ1 */
    ticks = ticks > 0 ? ticks : 1;
    
    p_timer->repeat_ticks = ticks;
    
    __softimer_remove(p_timer);
    __softimer_add(p_timer, ticks);
    am_int_cpu_unlock(old);
}

/******************************************************************************/
void am_softimer_stop (am_softimer_t *p_timer)
{
    int old;
    old = am_int_cpu_lock();
    __softimer_remove(p_timer);
    am_int_cpu_unlock(old);
}

/* end of file */

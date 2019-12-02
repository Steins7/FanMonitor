#ifndef _TIMER_H_
#define _TIMER_H_

#include "../target/stm32f103xb.h"
#include "../config.h"
#include "rcc.h"

#ifdef __cplusplus
extern "C" {
#endif 

typedef void (*OnTick)(void);

/***************************************************************************/
/* timer_wait_ms
 *   wait for ms millisecoonds function
 */
int timer_wait_ms(TIM_TypeDef *tmr, uint32_t ms, OnTick cb);

/* timer_wait_us
 *   wait for us microsecoonds function
 */
int timer_wait_us(TIM_TypeDef *tmr, uint32_t us, OnTick cb);

/***************************************************************************/
/* timer_tick_init
 *   setup timer to call cb function periodically, each tick_ms
 */
int timer_tick_init(TIM_TypeDef *tmr, uint32_t tick_ms, OnTick cb);

/* timer_tick_period
 *   change the tick_ms period
 */
int timer_tick_period(TIM_TypeDef *tmr, uint32_t tick_ms);

/* timer_start
 *   start counting to generate ticks
 */
void timer_start(TIM_TypeDef *tmr);

/* timer_stop
 *   stop and reset counting
 */
void timer_stop(TIM_TypeDef *tmr);

/***************************************************************************/
#define PWM_CHANNEL_1		0
#define PWM_CHANNEL_2		1
#define PWM_CHANNEL_3		2
#define PWM_CHANNEL_4		3

/* pwm_init
 *   setup pwm timer period, each tick_ms
 */
int pwm_init(TIM_TypeDef *pwm, uint32_t period_ms, OnTick cb);

/* pwm_channel_enable
 *   set up pwm channel
 */
int pwm_channel_enable(TIM_TypeDef *pwm, uint32_t channel, uint32_t dutycycle, uint32_t oe);

/* pwm_channel_disable
 *   disable pwm channel
 */
int pwm_channel_disable(TIM_TypeDef *pwm, uint32_t channel);

/* pwm_channel_set
 *   set up dutycycle for pwm channel
 */
int pwm_channel_set(TIM_TypeDef *pwm, uint32_t channel, uint32_t dutycycle);

/* pwm_start
 *   start counting
 */
#define pwm_start(pwm)	timer_start(pwm)

/* pwm_stop
 *   stop and reset counting
 */
#define pwm_stop(pwm)	timer_stop(pwm)

#ifdef __cplusplus
}
#endif
#endif

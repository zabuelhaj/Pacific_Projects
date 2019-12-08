/*
 * ppb.h
 *
 *  Created on: Oct 4, 2017
 *      Author: zabuelhaj
 */

#ifndef PPB_H_
#define PPB_H_

#define PPB ((volatile uint32_t *) 0xe000e000)

enum {
  PPB_STCTRL = (0x010 >> 2),
#define   PPB_STCTRL_COUNT   (1<<16)
#define   PPB_STCTRL_CLK_SRC (1<<2)
#define   PPB_STCTRL_INTEN   (1<<1)
#define   PPB_STCTRL_ENABLE  (1<<0)
  PPB_STRELOAD = (0x014 >> 2),
  PPB_STCURRENT = (0x018 >> 2),
  PPB_NVIC_EN0 = (0x100 >> 2),
#define   PPB_NVIC_EN0_BIT23 (1<<23)
  PPB_EN1 = (0x104>>2),
#define PPB_EN1_TIMER3A (1<<3)
  PPB_PRIx = (0x420>>2),
#define PPB_PRI_INTy_S  29
#define PPB_PRI_INTy_M  (0b111 << 29)
};

#endif /* PPB_H_ */

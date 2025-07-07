#ifndef _NPK_PH_DEV_H
#define _NPK_PH_DEV_H

#include <stdint.h>

struct npk_ph_dev {
    uint8_t devErrCode;
    uint8_t data_num;
    uint16_t n;                                 /* 氮 */
    uint16_t p;                                 /* 磷 */
    uint16_t k;                                 /* 钾 */
    uint16_t ph;                                /* PH */
		uint16_t n_calibration;                     /* 氮校准值 */
		uint16_t p_calibration;                     /* 磷校准值 */
		uint16_t k_calibration;                     /* 钾校准值 */
		uint16_t ph_calibration;                    /* PH校准值 */
};

/* 氮磷钾 PH的地址 */
#define NPK_PH_DEV0_ADDR                0x0BU

/* 定义寄存器地址 */
#define NPK_PH_GET_N             0x0004U         /*  */
#define NPK_PH_GET_P             0x0005U
#define NPK_PH_GET_K             0x0006U
#define NPK_PH_GET_PH            0x0003U
#define NPK_PH_ADJ_N             0x04EAU         /*氮校准值寄存器地址 */
#define NPK_PH_ADJ_P             0x04F4U         /*磷校准值寄存器地址 */
#define NPK_PH_ADJ_K             0x04FEU         /*钾校准值寄存器地址 */
#define NPK_PH_ADJ_PH            0x0053U         /*PH校准值寄存器地址 */

int read_npk_ph_dev(uint8_t addr, struct npk_ph_dev *data);

#endif

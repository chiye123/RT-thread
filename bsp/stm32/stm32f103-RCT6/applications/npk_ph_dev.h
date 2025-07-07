#ifndef _NPK_PH_DEV_H
#define _NPK_PH_DEV_H

#include <stdint.h>

struct npk_ph_dev {
    uint8_t devErrCode;
    uint8_t data_num;
    uint16_t n;                                 /* �� */
    uint16_t p;                                 /* �� */
    uint16_t k;                                 /* �� */
    uint16_t ph;                                /* PH */
		uint16_t n_calibration;                     /* ��У׼ֵ */
		uint16_t p_calibration;                     /* ��У׼ֵ */
		uint16_t k_calibration;                     /* ��У׼ֵ */
		uint16_t ph_calibration;                    /* PHУ׼ֵ */
};

/* ���׼� PH�ĵ�ַ */
#define NPK_PH_DEV0_ADDR                0x0BU

/* ����Ĵ�����ַ */
#define NPK_PH_GET_N             0x0004U         /*  */
#define NPK_PH_GET_P             0x0005U
#define NPK_PH_GET_K             0x0006U
#define NPK_PH_GET_PH            0x0003U
#define NPK_PH_ADJ_N             0x04EAU         /*��У׼ֵ�Ĵ�����ַ */
#define NPK_PH_ADJ_P             0x04F4U         /*��У׼ֵ�Ĵ�����ַ */
#define NPK_PH_ADJ_K             0x04FEU         /*��У׼ֵ�Ĵ�����ַ */
#define NPK_PH_ADJ_PH            0x0053U         /*PHУ׼ֵ�Ĵ�����ַ */

int read_npk_ph_dev(uint8_t addr, struct npk_ph_dev *data);

#endif

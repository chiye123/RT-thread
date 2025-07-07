#ifndef _MY_FLASH_H
#define _MY_FLASH_H

#include <stdint.h>

/* defined the FLASH_WP pin: PC4 */
#define FLASH_WP_PIN        GET_PIN(C, 4)
#define FLASH_WP_DISABLE    rt_pin_write(FLASH_WP_PIN, PIN_LOW);
#define FLASH_WP_ENABLE     rt_pin_write(FLASH_WP_PIN, PIN_HIGH);

/* defined the FLASH_CS pin: PA4 */
#define FLASH_CS_PIN_GROUP  GPIOA
#define FLASH_CS_PIN        GPIO_PIN_4


int write_to_flash(const char *partition_name, uint32_t addr, uint8_t *buf, uint32_t len);
int read_from_flash(const char *partition_name, uint32_t addr, uint8_t *buf, uint32_t len);


#endif

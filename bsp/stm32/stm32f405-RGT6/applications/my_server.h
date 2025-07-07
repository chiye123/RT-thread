#ifndef _MY_SERVER_H
#define _MY_SERVER_H

extern struct tmp_hum_dev tmp_hum_dev_data;

void periodic_reporting(void);
int start_my_server(void);

#endif


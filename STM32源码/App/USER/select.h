#ifndef __SELECT_H
#define __SELECT_H

#include "stm32f10x.h"
#include "vhfmod.h"

#define BUFF_SIZE 255

#define IDLE                   0
#define START_TX               1
#define SET_FREQ               2
#define SET_NUM_CONFIGURE      3
#define SET_GROUP_CONFIGURE    4
#define SET_DISPLAY            5
#define SET_DELAY1             6
#define SET_DELAY2             7
#define LOAD_NUM_CONFIG        8
#define LOAD_GROUP_CONFIG      9
#define HERD_CALL              10
#define GROUP_CALL             11
#define SET_NUM_FREQ           12
#define WIFI_SET_SSID          13
#define WIFI_SET_PSK           14
#define WIFI_STA_ON            15
#define WIFI_STA_OFF           16
#define WIFI_SHOW_IP           17
#define WIFI_SHOW_RSSI         18
#define WIFI_GET_IP            19
#define WIFI_GET_RSSI          20
#define SET_MESSAGE            21
#define WIFI_OK                22
#define WIFI_FAIL              23
#define SET_VHFMODULE_POWER    24
#define WIFI_SET_ONOFF         25
#define WIFI_SAVE_SSID         26
#define WIFI_SAVE_PSK          27

#define FACTORY_RESET          253
#define INPUT_ERROR            255

#define TYPE_NUM               100
#define TYPE_GROUP             101

extern u8 select;
extern unsigned char StartTx_Flag;//????????????
extern char pocsag_freq[9];

void Select_Fun(void);
void start_tx(void);
void set_freq(void);
void set_num_configure(void);
void set_group_configure(void);
void set_dispaly(void);
void set_PTTdelay(void);
void load_num_config(void);
void load_group_config(void);
void herd_call(void);
void group_call(void);
void set_num_freq(void);
void input_error(void);
void load_config(u8 type);
void wifi_set_ssid(void);
void wifi_set_psk(void);
void wifi_sta_on(void);
void wifi_sta_off(void);
void wifi_show_ip(void);
void wifi_show_rssi(void);
void wifi_get_ip(void);
void wifi_get_rssi(void);
void set_message(void);
void wifi_ok(void);
void wifi_fail(void);
void set_vhfmodule_power(void);
void wifi_set_onoff(void);
void factory_reset(void);
void wifi_save_ssid(void);
void wifi_save_psk(void);

#endif

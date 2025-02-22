#ifndef __ATK_M750_H
#define __ATK_M750_H

#define DTU_RX_CMD_BUF_SIZE (512)

#define DTU_ONENETDATE_RX_BUF (512)
#define DTU_ONNETDATE_TX_BUF (64)

typedef enum
{
    DTU_WORKMODE_NET = 0,  /*网络透传模式*/
    DTU_WORKMODE_HTTP,     /*http透传模式*/
    DTU_WORKMODE_MQTT,     /*mqtt透传模式*/
    DTU_WORKMODE_ALIYUN,   /*阿里云透传模式*/
    DTU_WORKMODE_ONENET,   /*OneNET透传模式*/
    DTU_WORKMODE_BAIDUYUN, /*百度云透传模式*/
} _dtu_work_mode_eu;

typedef struct
{
    uint32_t timeout; /*指令等待超时时间，单位：100ms*/
    char *read_cmd;   /*查询参数指令      请参考DTU AT用户手册说明进行填写*/
    char *write_cmd;  /*配置参数指令      请参考DTU AT用户手册说明进行填写*/
} _dtu_atcmd_st;

typedef struct {
    uint8_t year;    
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint8_t timezone;  
}ST_Time;

typedef struct {
	uint8_t Onenet_Off_flag;
	uint8_t Dtu_At_Clkflag;
	uint8_t Network_size;
	uint8_t dtu_rxbuf[DTU_ONENETDATE_RX_BUF];
	uint8_t st_flag;
	uint32_t dtu_rxlen;
}_dtu_4g_device;


void dtu_get_urc_info(uint8_t ch);

void send_data_to_dtu(uint8_t *data, uint32_t size);

int dtu_config_init(_dtu_work_mode_eu work_mode);

int dtu_enter_configmode(void);
int dtu_enter_transfermode(void);

uint8_t DTU_AT_CSQ_DataAnalyze(char CSQDat[][64]);

#endif

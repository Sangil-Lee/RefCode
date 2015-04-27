#ifndef __BENDING__
#define __BENDING__


#define BMT_SLAVE 		11

/************************************************************/
/* AI Record Define                                         */
/************************************************************/
#define LIMIT_CURR		0
#define PRO_VOUT 		1
#define CTRL_IOUT 		2
#define VOUT 			3
#define IOUT 			4
#define DC_VOLT 		5
#define DC_CURR 		6
#define INV_CURR 		7
#define REQ_VOUT_SET  		8
#define REQ_IOUT_SET 		9
#define REQ_VOUT_UNDER_VOLT 	10
#define REQ_VOUT_UNDER_CURR 	11
#define SET_IOUT_UNDER 		12
#define SET_VOUT_UNDER		13
#define SET_VOUT 		14
#define SET_IOUT 		15
#define FAULT_CODE		16

/************************************************************/
/* AO Record Define                                         */
/************************************************************/
#define SET_IOUT_UNDER_AO	0
#define SET_VOUT_UNDER_AO	1
#define SET_VOUT_AO		2
#define SET_IOUT_AO		3

/************************************************************/
/* BO Record Define                                         */
/************************************************************/
#define REMOTE_BTN		0
#define LOCAL_BTN		1
#define RUN_BTN			2
#define STOP_BTN		3
#define ESTOP_BTN		4
#define SET_BTN			5

/************************************************************/
/* BO Record Define                                         */
/************************************************************/
#define RUN_STATUS		0
#define IOUT_STATUS		1
#define FAULT_STATUS		2

/************************************************************/
/* Modbus Command Define                                    */
/************************************************************/
#define TOUCH_CMD_WRITE		16
#define IOUT_UNDER_WRITE	17
#define VOUT_UNDER_WRITE	18
#define VOUT_WRITE		19
#define IOUT_WRITE		20

#define MAX_BEND		3

typedef struct EUROTHERM_DATA {
    int sock;
    char ipaddr[256];
    int auto_connect;
    int isWrite;

    short status;                 /* 40001 */
    short fault_set;              /* 40002 */
    short limit_curr;             /* 40003 */
    short pro_vout;               /* 40004 */
    short ctrl_iout;              /* 40005 */
    short vout;                   /* 40006 */
    short iout;                   /* 40007 */
    short dc_volt;                /* 40008 */
    short dc_curr;                /* 40009 */
    short inv_curr;               /* 40010 */
    short fault_status;           /* 40011 */
    short req_vout_set;           /* 40012 */
    short req_iout_set;           /* 40013 */
    short req_vout_under_volt;     /* 40014 */
    short req_vout_under_curr;     /* 40015 */
    short touch_status;           /* 40016 */
    short set_iout_under;         /* 40017 */
    short set_vout_under;         /* 40018 */
    short set_vout;               /* 40019 */
    short set_iout;               /* 40020 */
    short fault_code;			  /* 40021 */

    short wr_vout_under;
    short wr_iout_under;
    short wr_vout;
    short wr_iout;

    short btn_val;
} Bending_Data;

extern Bending_Data bend_data[MAX_BEND];

#endif

///////////////////////////////////////////////////////////
//  ControllerHandler.h
//  Created on:      09-4-2013 ���� 7:24:23
//  Original author: ysw
///////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

const int ctrl_comm_init_serial (const char *pszDevName);
const int ctrl_comm_init_udp (const char *pszIpAddr, const short sPortNo);

const int ctrl_comm_open ();
const int ctrl_comm_close ();
const int ctrl_comm_read (void *buf, const int size);
const int ctrl_comm_write (void *buf, const int size);

void ctrl_comm_handler (void *param);

#ifdef __cplusplus
}
#endif

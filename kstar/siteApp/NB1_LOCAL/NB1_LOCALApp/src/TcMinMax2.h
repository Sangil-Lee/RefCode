#ifndef __TCMINMAX2__
#define __TCMINMAX2__

#define MINMAX_CONFIG2 "/usr/local/epics/siteApp/NB1_LOCAL/ioc_config/tcminmax2.cfg"
#define TC_MAX_CHAN2	400

typedef struct {
    int  ok[TC_MAX_CHAN2];
    char minmax[TC_MAX_CHAN2][50];
} TCMINMAX2;

extern int m_TcnMaxInd2;
extern int m_TcnMinInd2;
extern float m_TcfMax2;
extern float m_TcfMin2;

extern TCMINMAX2 tcMinMax2;


#endif

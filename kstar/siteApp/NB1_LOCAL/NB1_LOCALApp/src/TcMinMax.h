#ifndef __TCMINMAX__
#define __TCMINMAX__

#define MINMAX_CONFIG "/usr/local/epics/siteApp/NB1_LOCAL/ioc_config/tcminmax.cfg"
#define MINMAX_CONFIG2 "/usr/local/epics/siteApp/NB1_LOCAL/ioc_config/tcminmax2.cfg"
#define TC_MAX_CHAN	400

typedef struct {
    int  ok[TC_MAX_CHAN];
    char minmax[TC_MAX_CHAN][50];
} TCMINMAX;

extern int m_TcnMaxInd;
extern int m_TcnMinInd;
extern float m_TcfMax;
extern float m_TcfMin;

extern TCMINMAX tcMinMax;


#endif

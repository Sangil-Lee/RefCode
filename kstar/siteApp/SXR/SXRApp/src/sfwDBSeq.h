#ifndef SFW_DB_SEQUENCE_H
#define SFW_DB_SEQUENCE_H

#ifdef __cplusplus
extern "C" {
#endif

#if 1
#define str_CCS_SHOTSEQ_START    "CCS_SHOTSEQ_START" 
#define str_CCS_CTU_shotStart   "CCS_CTU_shotStart"
#else
#define str_CCS_SHOTSEQ_START    "ATCA_SHOTSEQ_START"
#define str_CCS_CTU_shotStart   "ATCA_CTU_shotStart"
#endif
#define ID_SHOTSEQ_START   0
#define ID_CCS_CTU_shotStart   1

#define SIZE_CNT_CA_ACCESS      2



int make_KSTAR_CA_seq();

int DBproc_put(const char *pname, const char *pvalue);
int DBproc_get(const char *pname, char *pvalue);
int CAproc_get(const char *pname, char *pvalue);
int CAproc_put(const char *pname, const char *pvalue);
int make_Admin_CA();




#ifdef __cplusplus
}
#endif

#endif




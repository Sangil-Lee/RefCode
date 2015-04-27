/*
 * NET-SNMP demo
 *
 * This program demonstrates different ways to query a list of hosts
 * for a list of variables.
 *
 * It would of course be faster just to send one query for all variables,
 * but the intention is to demonstrate the difference between synchronous
 * and asynchronous operation.
 *
 * Niels Baggesen (Niels.Baggesen@uni-c.dk), 1999.
 */

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>

#ifdef HAVE_WINSOCK_H
#include <winsock.h>
#endif

#include <string>
#include <vector>
#include <cstring>

using namespace std;
/*
 * a list of hosts to query
 */
struct host {
  const char *name;
  const char *community;
} hosts[] = {
  { "203.230.120.214","rkstar" },
  { NULL }
};
static unsigned long oval, curval;

/*
 * a list of variables to query for
 * HW supported OID or MIB browsed OID: Network Switch/Router, 
 * TS3500(Tape Library), DS8100(Main Strogate), EMC Backup Storage.
 */
struct OID {
  const char *Name;
  oid Oid[MAX_OID_LEN];
  //leesi++
  size_t OidLen;
} oids[] = {
  //{ "system.sysDescr.0" },
  { ".1.3.6.1.2.1.2.2.1.16.1001" },
  { ".1.3.6.1.2.1.2.2.1.16.1002" },
  { ".1.3.6.1.2.1.2.2.1.16.1003" },
  { ".1.3.6.1.2.1.2.2.1.16.1004" },
  { ".1.3.6.1.2.1.2.2.1.16.1005" },
  { ".1.3.6.1.2.1.2.2.1.16.1006" },
  { ".1.3.6.1.2.1.2.2.1.16.1007" },
  { ".1.3.6.1.2.1.2.2.1.16.1008" },
  { ".1.3.6.1.2.1.2.2.1.16.1009" },
  { ".1.3.6.1.2.1.2.2.1.16.1010" },
  { ".1.3.6.1.2.1.2.2.1.16.1011" },
  { ".1.3.6.1.2.1.2.2.1.16.1012" },
  { ".1.3.6.1.2.1.2.2.1.16.1013" },
  { ".1.3.6.1.2.1.2.2.1.16.1014" },
  { ".1.3.6.1.2.1.2.2.1.16.1015" },
  { ".1.3.6.1.2.1.2.2.1.16.1016" },
  { ".1.3.6.1.2.1.2.2.1.16.1017" },
  { ".1.3.6.1.2.1.2.2.1.16.1018" },
  { ".1.3.6.1.2.1.2.2.1.16.1019" },
  { ".1.3.6.1.2.1.2.2.1.16.1020" },
  { ".1.3.6.1.2.1.2.2.1.16.1021" },
  { ".1.3.6.1.2.1.2.2.1.16.1022" },
  { ".1.3.6.1.2.1.2.2.1.16.1023" },
  { ".1.3.6.1.2.1.2.2.1.16.1024" },
  { ".1.3.6.1.2.1.2.2.1.16.1025" },
  { ".1.3.6.1.2.1.2.2.1.16.1026" },
  { ".1.3.6.1.2.1.2.2.1.16.1027" },
  { ".1.3.6.1.2.1.2.2.1.16.1028" },
  { ".1.3.6.1.2.1.2.2.1.16.1029" },
  { ".1.3.6.1.2.1.2.2.1.16.1030" },
  { ".1.3.6.1.2.1.2.2.1.16.1031" },
  { ".1.3.6.1.2.1.2.2.1.16.1032" },
  { ".1.3.6.1.2.1.2.2.1.16.1033" },
  { ".1.3.6.1.2.1.2.2.1.16.1034" },
  { ".1.3.6.1.2.1.2.2.1.16.1035" },
  { ".1.3.6.1.2.1.2.2.1.16.1036" },
  { ".1.3.6.1.2.1.2.2.1.16.1037" },
  { ".1.3.6.1.2.1.2.2.1.16.1038" },
  { ".1.3.6.1.2.1.2.2.1.16.1039" },
  { ".1.3.6.1.2.1.2.2.1.16.1040" },
  { ".1.3.6.1.2.1.2.2.1.16.1041" },
  { ".1.3.6.1.2.1.2.2.1.16.1042" },
  { ".1.3.6.1.2.1.2.2.1.16.1043" },
  { ".1.3.6.1.2.1.2.2.1.16.1044" },
  { ".1.3.6.1.2.1.2.2.1.16.1045" },
  { ".1.3.6.1.2.1.2.2.1.16.1046" },
  { ".1.3.6.1.2.1.2.2.1.16.1047" },
  { ".1.3.6.1.2.1.2.2.1.16.1048" },
  //{ ".1.3.6.1.2.1.2.2.1.16.1002" },		
  //{ ".1.3.6.1.2.1.16.1.1.1.5.1001" },	
  //{ ".1.3.6.1.2.1.16.1.1.1.5.1002" },	
  //{ "interfaces.ifNumber.1" },
  //{ "interfaces.ifNumber.0" },
  { NULL }
};

/*
 * initialize
 */
void initialize (void)
{
  struct OID *op = oids;
  /* Win32: init winsock */
  SOCK_STARTUP;
  /* initialize library */
  init_snmp("asynchapp");
  /* parse the oids */
  while (op->Name) 
  {
    op->OidLen = sizeof(op->Oid)/sizeof(op->Oid[0]);
    if (!read_objid(op->Name, op->Oid, &op->OidLen)) {
      snmp_perror("read_objid");
      exit(1);
    };
    op++;
  };
}

/*
 * simple printing of returned data
 */
int print_result (int status, struct snmp_session *sp, struct snmp_pdu *pdu)
{
  char buf[1024];
  struct variable_list *vp;
  int ix;
  struct timeval now;
  struct timezone tz;
  struct tm *tm;

  gettimeofday(&now, &tz);
  tm = localtime(&now.tv_sec);
  fprintf(stdout, "%.2d:%.2d:%.2d.%.6d ", tm->tm_hour, tm->tm_min, tm->tm_sec,
          now.tv_usec);
  switch (status) 
  {
	  case STAT_SUCCESS:
		  vp = pdu->variables;
		  if (pdu->errstat == SNMP_ERR_NOERROR) 
		  {
			  while (vp) {
				  //leesi
				  snprint_variable(buf, sizeof(buf), vp->name, vp->name_length, vp);
				  string snmpdata = buf;
				  size_t pos = snmpdata.rfind(":");
				  string strVal = snmpdata.substr(pos+2);
				  oval = curval;
				  curval = atol(strVal.c_str());
				  fprintf(stdout, "PeerName:(%s), Received DataBuffer:(%s), strVal:(%s)\n", sp->peername, snmpdata.c_str(), strVal.c_str());
				  vp = vp->next_variable;
				  //int packets = labs(oval-curval);
				  //printf("1-Port: Packets:(%d), (%.2f)Bytes \n",packets, (packets*512)/5.0);
				  //printf("1-Port: Total:(%d bytes), AVG:(%.2f bytes) \n",packets, packets/5.0);
			  };
		  }
		  else {
			  for (ix = 1; vp && ix != pdu->errindex; vp = vp->next_variable, ix++)
				  ;
			  if (vp) snprint_objid(buf, sizeof(buf), vp->name, vp->name_length);
			  else strcpy(buf, "(none)");

			  fprintf(stdout, "%s: %s: %s\n", sp->peername, buf, snmp_errstring(pdu->errstat));
		  }
		  return 1;
	  case STAT_TIMEOUT:
		  fprintf(stdout, "%s: Timeout\n", sp->peername);
		  return 0;
	  case STAT_ERROR:
		  snmp_perror(sp->peername);
		  return 0;
  }
  return 0;
}

/*****************************************************************************/

/*
 * simple synchronous loop
 */
void synchronous (void)
{
  struct host *hp;

  for (hp = hosts; hp->name; hp++) 
  {
    struct snmp_session ss, *sp;
    struct OID *op;

    snmp_sess_init(&ss);			/* initialize session */
    ss.version = SNMP_VERSION_2c;
    ss.peername = strdup(hp->name);
	printf("synchronous\n");
	/*leesi++*/
    //ss.community = strdup(hp->community);
    //ss.community_len = strlen(ss.community);
    ss.community = (u_char*)strdup(hp->community);
    ss.community_len = strlen((const char*)ss.community);
	/*leesi--*/

    if (!(sp = snmp_open(&ss))) {
      snmp_perror("snmp_open");
      continue;
    }
	while(true)
	{
		for (op = oids; op->Name; op++) 
		{
			struct snmp_pdu *req, *resp;
			int status;
			req = snmp_pdu_create(SNMP_MSG_GET);
			printf("OpName:%s, Oid:%ld\n", op->Name, op->Oid);
			snmp_add_null_var(req, op->Oid, op->OidLen);
			status = snmp_synch_response(sp, req, &resp);
			if (!print_result(status, sp, resp)) break;
			snmp_free_pdu(resp);
		};
		sleep(5);
	};
    snmp_close(sp);
  }
}

/*****************************************************************************/

/*
 * poll all hosts in parallel
 */
struct session {
  struct snmp_session *sess;		/* SNMP session data */
  struct OID *current_oid;		/* How far in our poll are we */
} sessions[sizeof(hosts)/sizeof(hosts[0])];

int active_hosts;			/* hosts that we have not completed */

/*
 * response handler
 */
int asynch_response(int operation, struct snmp_session *sp, int reqid,
		    struct snmp_pdu *pdu, void *magic)
{
  struct session *host = (struct session *)magic;
  struct snmp_pdu *req;

  if (operation == NETSNMP_CALLBACK_OP_RECEIVED_MESSAGE) {
    if (print_result(STAT_SUCCESS, host->sess, pdu)) {
      host->current_oid++;			/* send next GET (if any) */
      if (host->current_oid->Name) {
	req = snmp_pdu_create(SNMP_MSG_GET);
	snmp_add_null_var(req, host->current_oid->Oid, host->current_oid->OidLen);
	if (snmp_send(host->sess, req))
	  return 1;
	else {
	  snmp_perror("snmp_send");
	  snmp_free_pdu(req);
	}
      }
    }
  }
  else
    print_result(STAT_TIMEOUT, host->sess, pdu);

  /* something went wrong (or end of variables) 
   * this host not active any more
   */
  active_hosts--;
  return 1;
}

void asynchronous(void)
{
  struct session *hs;
  struct host *hp;

  /* startup all hosts */

  for (hs = sessions, hp = hosts; hp->name; hs++, hp++) 
  {
    struct snmp_pdu *req;
    struct snmp_session sess;
    snmp_sess_init(&sess);			/* initialize session */
    sess.version = SNMP_VERSION_2c;
    sess.peername = strdup(hp->name);

	/*leesi++*/
    //sess.community = strdup(hp->community);
    //sess.community_len = strlen(sess.community);
    sess.community = (u_char*)strdup(hp->community);
    sess.community_len = strlen((const char*)sess.community);
	/*leesi--*/

    sess.callback = asynch_response;		/* default callback */
    sess.callback_magic = hs;
    if (!(hs->sess = snmp_open(&sess))) {
      snmp_perror("snmp_open");
      continue;
    }
    hs->current_oid = oids;
    req = snmp_pdu_create(SNMP_MSG_GET);	/* send the first GET */
    snmp_add_null_var(req, hs->current_oid->Oid, hs->current_oid->OidLen);
    if (snmp_send(hs->sess, req))
      active_hosts++;
    else {
      snmp_perror("snmp_send");
      snmp_free_pdu(req);
    }
  }

  /* loop while any active hosts */

  while (active_hosts) {
    int fds = 0, block = 1;
    fd_set fdset;
    struct timeval timeout;

    FD_ZERO(&fdset);
    snmp_select_info(&fds, &fdset, &timeout, &block);
    fds = select(fds, &fdset, NULL, NULL, block ? NULL : &timeout);
    if (fds < 0) {
        perror("select failed");
        exit(1);
    }
    if (fds)
        snmp_read(&fdset);
    else
        snmp_timeout();
  }

  /* cleanup */

  for (hp = hosts, hs = sessions; hp->name; hs++, hp++) {
    if (hs->sess) snmp_close(hs->sess);
  }
}

/*****************************************************************************/

int main (int argc, char **argv)
{
  initialize();

  //printf("---------- synchronous -----------\n");
  //synchronous();

  printf("---------- asynchronous -----------\n");
  asynchronous();

  return 0;
}

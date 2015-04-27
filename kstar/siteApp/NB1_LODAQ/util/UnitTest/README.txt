1. sfwEnv

	EPICS_CA_ADDR_LIST="127.0.0.1 172.17.102.120"

2. sfwSequence.c

	str_CCS_SHOTSEQ_START : CCS_SHOTSEQ_START -> YUNSW_SHOTSEQ_START
	str_CCS_CTU_shotStart : CCS_CTU_shotStart -> YUNSW_CTU_shotStart

3. first IOC on different host

  1) DB
	// dbSubExample.db
	record(bi, "YUNSW_SHOTSEQ_START") {
		field(DESC, "CCS_SHOTSEQ_START test")
			field(DTYP, "Soft Channel")
			field(ZNAM, "STOP")
			field(ONAM, "START")
	}

	record(bi, "YUNSW_CTU_shotStart") {
		field(DESC, "CCS_CTU_shotStart")
		field(PINI, "YES")
		field(DTYP, "Soft Channel")
		field(ZNAM, "flip(0)")
		field(ONAM, "flop(1)")
	}

  2) Run st.cmd


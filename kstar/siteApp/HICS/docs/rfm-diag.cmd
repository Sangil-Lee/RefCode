< How to use the rfm2g_util >

# -------------------------------------------------
# 1. Summary
# -------------------------------------------------

# cd /root/Downloads/rfm/rfm2g/diags
# ./rfm2g_util

  Please enter device number: 0

  UTIL0 > read  0x02000000 4 5
  UTIL0 > read  0x02000200 4 5
  UTIL0 > exit


# -------------------------------------------------
# 2. Quick test
# -------------------------------------------------
#   1) initializing memory section for EC1_ANT
#   2) enabling pcs control : pcs_mode
#   3) starting pcs control : pcs_run
#   4) changing angle count : count_cmd
#   5) reading status from RT5
# -------------------------------------------------

  # initializing memory for command and status : memset(0)
  UTIL0 > write 0 0x02000000 4 5
  UTIL0 > write 0 0x02000200 4 5

  # enabling pcs control : mode & run bits
  UTIL0 > write 1 0x02000004 4 1
  UTIL0 > write 3 0x02000004 4 1

  # changing angle count
  UTIL0 > write     0 0x02000008 4 1
  UTIL0 > write   100 0x02000008 4 1
  UTIL0 > write   500 0x02000008 4 1
  UTIL0 > write  1000 0x02000008 4 1
  UTIL0 > write  2000 0x02000008 4 1
  UTIL0 > write  5000 0x02000008 4 1
  UTIL0 > write  8500 0x02000008 4 1
  UTIL0 > write 10000 0x02000008 4 1
  UTIL0 > write 17000 0x02000008 4 1
  UTIL0 > write 18000 0x02000008 4 1

  # reading command and status
  UTIL0 > read  0x02000000 4 5
  UTIL0 > read  0x02000200 4 5

  # disabling pcs control : mode & run
  UTIL0 > write 1 0x02000004 4 1
  UTIL0 > write 0 0x02000004 4 1


# -------------------------------------------------
# 3. Details
# -------------------------------------------------

[root@pcsrt5 ~]# cd /root/Downloads/rfm/rfm2g/diags
[root@pcsrt5 diags]# ./rfm2g_util

  PCI RFM2g Commandline Diagnostic Utility

  Available devices:

    0.  /dev/rfm2g0  (VMIPCI-5565, Node 218)
Please enter device number: 0
UTIL0 > help

  COMMAND           PARAMETERS
  -----------------------------------------------------
  boardid
  cancelwait        event
  checkring
  clearevent        event
  cleareventcount   event
  clearowndata
  config
  devname
  disableevent      event
  disablecallback   event
  dllversion
  dmatest           loopcount length offset
  randmatest        loopcount
  driverversion
  drvspecific
  dump              offset width length
  enableevent       event
  enablecallback    event
  errormsg          ErrorCode
  exit
  first
  getdarkondark
  getdebug
  getdmabyteswap
  geteventcount     event
  getled
  getmemoryoffset
  getloopback
  getparityenable
  getpiobyteswap
  getslidingwindow
  getthreshold
  gettransmit
  help              [command]
  mapuser           offset pages
  mapuserbytes      offset bytes
  memop             pattern offset width length verify float patterntype
  nodeid
  peek8             offset
  peek16            offset
  peek32            offset
  peek64            offset
  performancetest
  poke8             value offset
  poke16            value offset
  poke32            value offset
  poke64            value offset
  quit
  read              offset width length display
  repeat            [-p] count cmd [arg...]
  send              event tonode [ext_data]
  setdarkondark     state
  setdebug          flag
  setdmabyteswap    state
  setled            state
  setloopback       state
  setmemoryoffset   offset
  setparityenable   state
  setpiobyteswap    state
  setslidingwindow  offset
  setthreshold      value
  settransmit       state
  size
  unmapuser
  unmapuserbytes
  wait              event timeout
  write             value offset width length

UTIL0 > read 0x02000000 4 6
DMA will not be used.

System memory buffer contents after RFM2gRead

                    0        1        2        3
0x02000000:  006919A0 00000000 00000003 20290000 |..i. .... .... ..) |
0x02000010:  A74AA5A5 A512A5A5                   |..J. ....|


UTIL0 > read 0x02000200 4 3
DMA will not be used.

System memory buffer contents after RFM2gRead

                    0        1        2        3
0x02000200:  00000000 000100FF A568A5A5          |.... .... ..h.|


UTIL0 > exit
Exit? (y/n) : y

[root@pcsrt5 diags]#


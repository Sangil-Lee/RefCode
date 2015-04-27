#! /bin/bash

trap bashtrap INT
if [ $# -eq 0 ]; then
 echo "Usage -> sdn-tune-services.sh ethernet-device "
 exit
fi

service NetworkManager stop
service sendmail stop
service auditd stop
service cpuspeed stop
service ptpd-nisync stop
service pcscd stop
service gpm stop
service rtctl status
route add -net 224.0.0.0/4 dev $1
# increase arpa cache timeout 
echo 360000 > /proc/sys/net/ipv4/neigh/$1/gc_stale_time
#check what are the services running 
chkconfig --list
#Increase the interval between scheduled system route flushes. For example:
echo 360000 > /proc/sys/net/ipv4/route/secret_interval
# enable icmp ping 
#echo "0" > /proc/sys/net/ipv4/icmp_echo_ignore_broadcasts

# The following line is for MRG-R ($1 irq name is different in Vanilla)
#tuna --sockets 1 --isolate --irqs $1 --cpus 1,3,5,7 --move --irqs $1*[0-3]* --cpus 1,3 --move --irqs $1*[4-7]* --cpus 5,7 --move --cpus 1,3,5,7 --threads cxgb3 --move --threads cxgb3 --priority FIFO:70
tuna --cpus 1,3,5,7 --isolate --irqs $1 --cpus 1,3,5,7 --move --irqs $1*[0-3]* --cpus 1,3 --move --cpus 1,3,5,7 --threads cxgb3 --move --threads cxgb3 --priority FIFO:70 --irqs $1*[4-7]* --cpus 5,7 --move 



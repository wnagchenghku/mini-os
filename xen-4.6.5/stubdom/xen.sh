#!/bin/sh
DOMAINNAME=MiniOS

LOG_FILE="/tmp/tmp.txt"
exec 3<> $LOG_FILE
> $LOG_FILE

xl destroy $DOMAINNAME
xl create -p domain_config >&3

DOMID=$( xl list | grep "^${DOMAINNAME}" | awk '{ print $2 }' )

CONDEV=$( xenstore-read /local/domain/${DOMID}/console/tty )
echo " Domain console: $CONDEV"

python xen.py $DOMID $CONDEV $LOG_FILE

exec 3>&-
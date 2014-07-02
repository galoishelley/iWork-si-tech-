WORKDIR=/iboss1/offon/datacenter/datatest; export WORKDIR
ETCDIR=/iboss1/offon/datacenter/datatest/cfg; export ETCDIR

if ps -ef | grep datacenter | grep datatest.cfg | grep -v grep 
then
	echo "`date`datacenter is aliave "
else
	echo "`date`will run datacenter "
	$HOME/offon/datacenter/datatest/bin/datacenter datatest.cfg 
fi

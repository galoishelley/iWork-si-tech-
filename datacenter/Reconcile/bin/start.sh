WORKDIR=/iboss1/offon/datacenter/Reconcile; export WORKDIR
ETCDIR=/iboss1/offon/datacenter/Reconcile/cfg; export ETCDIR

if ps -ef | grep dataReconcile | grep jlReconcile.cfg | grep -v grep 
then
	echo "`date`dataReconcile is aliave "
else
	echo "`date`will run dataReconcile "
	$HOME/offon/datacenter/Reconcile/bin/dataReconcile jlReconcile.cfg 
fi

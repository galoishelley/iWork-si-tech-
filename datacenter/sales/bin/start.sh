WORKDIR=/iboss1/offon/datacenter/sales; export WORKDIR
ETCDIR=/iboss1/offon/datacenter/sales/cfg; export ETCDIR

if ps -ef | grep SalesFeedBack | grep jlsales.cfg | grep -v grep 
then
	echo "`date`SalesFeedBack is aliave "
else
	echo "`date`will run SalesFeedBack "
	$HOME/offon/datacenter/sales/bin/SalesFeedBack jlsales.cfg 1 0 
fi

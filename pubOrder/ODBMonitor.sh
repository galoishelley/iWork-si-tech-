. ~/.profile
WORK_DIR=${HOME}/${ORDERMONITOR}
monitor_path=${WORK_DIR}/log/monitor.log
RTNUM=`ps -ef | grep orderDealBoss | grep -v grep | wc -l`
if [ ! $RTNUM -eq 11 ]
then
	date>>$monitor_path
	echo "ProcessNum="$RTNUM" Restart orderDealBoss!!!">>$monitor_path
	${WORK_DIR}/bin/orderStop orderDealBoss
	${WORK_DIR}/bin/orderDealBoss
else
	date>>$monitor_path
	echo "orderDealBoss is alive!">>$monitor_path
fi

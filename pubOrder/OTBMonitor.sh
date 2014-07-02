. ~/.profile
WORK_DIR=${HOME}/${ORDERMONITOR}
monitor_path=${WORK_DIR}/log/monitor.log
RTNUM=`ps -ef | grep orderTransferBoss | grep -v grep | wc -l`
if [ ! $RTNUM -eq 11 ]
then
	date>>$monitor_path
	echo "ProcessNum="$RTNUM" Restart orderTransferBoss!!!">>$monitor_path
	${WORK_DIR}/bin/orderStop orderTransferBoss
	${WORK_DIR}/bin/orderTransferBoss
else
	date>>$monitor_path
	echo "orderTransferBoss is alive!">>$monitor_path
fi

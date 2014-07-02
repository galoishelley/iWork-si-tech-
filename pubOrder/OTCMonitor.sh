. ~/.profile
WORK_DIR=${HOME}/${ORDERMONITOR}
monitor_path=${WORK_DIR}/log/monitor.log
RTNUM=`ps -ef | grep orderTransferCrm | grep -v grep | wc -l`
if [ ! $RTNUM -eq 11 ]
then
	date>>$monitor_path
	echo "ProcessNum="$RTNUM" Restart orderTransferCrm!!!">>$monitor_path
	${WORK_DIR}/bin/orderStop orderTransferCrm
	${WORK_DIR}/bin/orderTransferCrm
else
	date>>$monitor_path
	echo "orderTransferCrm is alive!">>$monitor_path
fi

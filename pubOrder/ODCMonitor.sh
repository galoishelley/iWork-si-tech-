. ~/.profile
WORK_DIR=${HOME}/${ORDERMONITOR}
monitor_path=${WORK_DIR}/log/monitor.log
RTNUM=`ps -ef | grep orderDealCrm | grep -v grep | wc -l`
if [ ! $RTNUM -eq 11 ]
then
	date>>$monitor_path
	echo "ProcessNum="$RTNUM" Restart orderDealCrm!!!">>$monitor_path
	${WORK_DIR}/bin/orderStop orderDealCrm
	${WORK_DIR}/bin/orderDealCrm
else
	date>>$monitor_path
	echo "orderDealCrm is alive!">>$monitor_path
fi

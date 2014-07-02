echo "will stop dataReconcile "
ps -ef | grep dataReconcile |grep jlReconcil |grep -v grep | awk '{print $2}'|xargs kill
echo "dataReconcile end"

echo "will stop datacenter "
ps -ef | grep datacenter |grep datatest.cfg |grep -v grep | awk '{print $2}'|xargs kill
echo "datacenter end"

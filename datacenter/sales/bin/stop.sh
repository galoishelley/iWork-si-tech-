echo "will stop SalesFeedBack "
ps -ef | grep SalesFeedBack |grep jlsales.cfg |grep -v grep | awk '{print $2}'|xargs kill
echo "SalesFeedBack end"

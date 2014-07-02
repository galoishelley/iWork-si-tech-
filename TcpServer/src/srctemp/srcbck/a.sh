echo "Í£Ö¹ TcpServer ·þÎñ"
ps -ef|grep TcpServer|awk {'print $2'}|xargs kill -9
echo "over"

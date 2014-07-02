echo "停止 TcpServer 服务"
ps -ef|grep TcpServer|awk {'print $2'}|xargs kill -9
echo "起动 TcpServer 服务"
TcpServer
echo "over"

echo "ֹͣ TcpServer ����"
ps -ef|grep TcpServer|awk {'print $2'}|xargs kill -9
echo "�� TcpServer ����"
TcpServer
echo "over"

##sudo named -c 172.16.9.60
sudo route -n add -net 172.0.0.0 -netmask 255.0.0.0 10.161.251.16
sudo ifconfig en1 ether 00:24:7E:11:E2:44 

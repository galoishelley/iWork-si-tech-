ADD_64BIT_OPT=+DA2.0W +M1 -w3 -DSS_64BIT_SERVER

all:ftp_decode
ftp_decode:ftp_decode.c ftp_conn.h 
	@lint -c ftp_decode.c
	@cc $(ADD_64BIT_OPT) -o ftp_decode -g ftp_decode.c
clean:
	@rm *.o *.ln

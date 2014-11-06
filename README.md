tftp_implementation
===================

tftp implementation in cplusplus

To run the program
type
g++ -o common.o -c common.cpp
g++ tftpserver.cpp -o t common.o
g++ tftpclient.cpp -o r common.o

./t
./r -r virat.txt -- for reading
./r -w virat.txt --for writing

./t portno ---> for running on specific port
./r -r virat.txt portno

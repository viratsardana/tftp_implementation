#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<iostream>
#include<string.h>
#include<stdio.h>
#include<unistd.h>
#include<signal.h>
#include<stdlib.h>
using namespace std;
/*this is group specific port*/

#define SERV_UDP_PORT 9800


/*keeps track of the number of timeouts*/
extern int count;

extern void test();
extern void test2();

//now declaring various packets in TFTP protocol

extern struct sockaddr_in serveraddr;
extern struct sockaddr_in clientaddr;
extern struct sockaddr_in g_clientaddr;

extern char last_data_sent[512];

extern int sd;
extern int nd;
extern int recvlen;

//function for handling error cases
extern void send_errorpack(int,struct sockaddr_in);

//functions for sending and receiving initial packet requests
extern void send_req(int,char *);
extern void send_packet(struct sockaddr_in);
extern void send_ack(struct sockaddr_in*);
extern int recv_initial_ack();

//functions to read and write files
extern void read_file(const char*,struct sockaddr_in);
extern void write_file(const char*,struct sockaddr_in);

//sending and receiving data packet functions
extern void send_datapacket(char *,int,int,struct sockaddr_in);
extern int recv_datapacket(const char*);

//sending and receiving data ack packet functions
extern void send_data_ack(int,struct sockaddr_in);
extern void recv_data_ack(int);

//timeout functions
extern void handler(int);

struct rwq_packet{
  short opcode;
  char filename[100];
  char middle;
  char mode[100];
  char last;
};

struct data_packet{
  short opcode;
  short blockno;
  char data[512];
};

struct ack_packet{
   short opcode;
   short blockno;
};

struct error_packet{
  short opcode;
  short error_code;
  char errormsg[100];
  char last;
};

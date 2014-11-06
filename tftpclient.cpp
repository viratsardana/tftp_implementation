#include "common.h"


int main(int argc,char ** argv){

 int flag=-1;
 int port;
 
 cout<<"The value of argc is "<<argc<<endl;
 
 if(argc<=3){
  if(argv[1][1]=='r'){
   //cout<<"The clinet wishes to read a file from server"<<endl;
   flag=0;
   }
   
   else if(argv[1][1]=='w'){
     //cout<<"The client wishes to write a file to server"<<endl;
     flag=1;
     }
     port=SERV_UDP_PORT;
   }  
   
   //cout<<"File name is "<<p<<endl;
   else if(argc>3){
      if(argv[1][1]=='r'){
   //cout<<"The clinet wishes to read a file from server"<<endl;
   flag=0;
   }
   
   else if(argv[1][1]=='w'){
     //cout<<"The client wishes to write a file to server"<<endl;
     flag=1;
     }
   char *q=argv[3];
   port=atoi(q);
   cout<<port<<endl;
   } 
  
  char *p;
  p=argv[2];
  
  serveraddr.sin_port=htons(port);
  serveraddr.sin_family=AF_INET;
  serveraddr.sin_addr.s_addr=htonl(INADDR_ANY);


  if(sd<0){
    cout<<"Socket could not be created"<<endl;
    return 0;
  }

  send_req(flag,argv[2]);   //makes the packet to be sent ready
  send_packet(serveraddr); //sends the initial request packet
  if(!recv_initial_ack()){
   //could not receive ack
    return 0;
  }
  
  else{
   cout<<"ack received successfully"<<endl;
   //start receiving the datapackets
   if(flag==0){
   while(1){
   if(!recv_datapacket(p))
     break;
   }
  }  
  
  else if(flag==1){
     //read file on client side and send data packets to the server
     read_file(argv[2],serveraddr);
  }
  }
  close(sd);
return 0;
}

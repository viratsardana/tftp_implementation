#include "common.h"
#include<stdio.h>
  
int main(int argc,char ** argv){

 //creating socket
 
 //sd=socket(AF_INET,SOCK_DGRAM,0);
     
  int port;
 
  /*
   initially the clients will contact server on group specific port
 */
 
  if(argc==2){
    char *p=argv[1];
    port=atoi(p);
    cout<<port<<endl;
    }
    
  else{
    port=SERV_UDP_PORT;
    cout<<port<<endl;
    }
  
  serveraddr.sin_port=htons(port);
  serveraddr.sin_family=AF_INET;
  serveraddr.sin_addr.s_addr=htonl(INADDR_ANY);
 
 if(sd<0){
   cout<<"Socket cannot be created"<<endl;
   return 0;
   }

 if(bind(sd,(struct sockaddr*) &serveraddr,sizeof(serveraddr))<0){
   cout<<"Binding of socket failed"<<endl;
   return 0;
 }
  
 //struct sockaddr_in clientaddr;
 
 socklen_t addrlen=sizeof(clientaddr);
 struct rwq_packet abc;
 recvfrom(sd,(struct rwq_packet*)&abc,sizeof(abc),0,(struct sockaddr*)&clientaddr,&addrlen);
 
     cout<<"packet received successfully"<<endl;    
     cout<<abc.filename<<"\n";
     cout<<std::flush;
     send_ack(&clientaddr);
     
     if(abc.opcode==1){
     read_file(abc.filename,clientaddr);
     }
     else if(abc.opcode==2){
     write_file(abc.filename,serveraddr);
     }
     
     
 close(sd);
 
return 0;
}

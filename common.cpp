#include "common.h"


int sd=socket(AF_INET,SOCK_DGRAM,0);
int nd=socket(AF_INET,SOCK_DGRAM,0);
int recvlen=0;

struct rwq_packet rw;

struct sockaddr_in serveraddr;
struct sockaddr_in clientaddr;
struct sockaddr_in g_clientaddr;

int count=0;

char last_data_sent[512];
int g_block;
int g_dpacket_size;

void send_errorpack(int ecode,struct sockaddr_in myaddr){
   struct error_packet epacket;
   epacket.opcode=5;
   
   if(ecode==1){
      /* file cannot be opened or file does not exist in read operation*/
      
      cout<<"File cannot be opened"<<endl;
      
      epacket.error_code=1;
      char str[]="File does not exist";
      strcpy(epacket.errormsg,str);
      epacket.last='0';
   }
   
   else if(ecode==2){
      epacket.error_code=2;
      char str[]="File already exists..Overwrite warning";
      strcpy(epacket.errormsg,str);
      epacket.last='0';   
   }
   
   sendto(sd,(struct error_packet*)&epacket,sizeof(epacket),0,(struct sockaddr*)&myaddr,sizeof(myaddr));
   
}

void handler(int sig){

/*this handler handles the initial request packet timeout*/

   cout<<"receiving initial ack packet failed"<<endl;
   test();
   alarm(3);
  }

void test(){
   cout<<count<<endl;
   if(count==10){
   /*exit as server is not there*/
   exit(0);
   }
   send_packet(serveraddr);
   count++;
}

void handler2(int sig){
   cout<<"receiving ack for data packet failed"<<endl;
   test2();
   alarm(3);
}

void test2(){
  /* retransmit the last datapacket again*/
  send_datapacket(last_data_sent,g_dpacket_size,g_block,g_clientaddr); 
}

void recv_data_ack(int blockno){
   
   /*waiting for the acknowledgement of this blockno--->blockno*/

   struct ack_packet dack;
   struct sockaddr_in n_addr;
   socklen_t addrlen=sizeof(n_addr);
   
   /*Receive error packet for the first time to check for the error file already exists*/
   
   if(blockno==1){
       struct error_packet epacket; 
       recvfrom(sd,(struct error_packet*)&epacket,sizeof(error_packet),0,(struct sockaddr*)&n_addr,&addrlen);
       cout<<epacket.errormsg<<endl;
   }
   
   /*If you do not receive the data ack within timeout period retransmit the packet again*/
   
   signal(SIGALRM,handler2);
   alarm(3); /*timeout of 3 seconds*/      
   if(recvfrom(sd,(struct ack_packet*)&dack,sizeof(ack_packet),0,(struct sockaddr*)&n_addr,&addrlen)<0){
      cout<<"Receiving ack failed"<<endl;
      return;
    }
    alarm(0);
    cout<<"Ack #"<<dack.blockno<<endl;
}

void send_data_ack(int block,struct sockaddr_in myaddr){
   struct ack_packet dack;
   dack.opcode=4;
   dack.blockno=block;
   if(sendto(sd,(struct ack_packet*)&dack,sizeof(ack_packet),0,(struct sockaddr*)&myaddr,sizeof(myaddr))<0){
     cout<<"Sending ack failed"<<endl;
     return;
    }
}

int recv_datapacket(const char* filename){
   
   /* filename name of the file to which you want to write to*/

   struct data_packet dp1;
   struct sockaddr_in n_addr;
   socklen_t addrlen=sizeof(n_addr);
   if(recvfrom(sd,(struct data_packet*)&dp1,sizeof(data_packet),0,(struct sockaddr*)&n_addr,&addrlen)<0){
      cout<<"Receiving Failed"<<endl;
      return 0;
      }
   //cout<<dp1.data<<" ";
   else{
   if(dp1.opcode==5){
     /*an error is received*/
     /*print the error message*/
     cout<<dp1.data<<endl;
     return 0;
   }
   else{
   cout<<"Receiving #"<<dp1.blockno<<endl;
   
   /*once you receive the data write to file
     if file already exists send an error message of overwrite warning
      */
     
   if( access( filename, F_OK ) != -1 && dp1.blockno==1){
    send_errorpack(2,n_addr);
    cout<<"File already exists"<<endl;    
    } 
   send_data_ack(dp1.blockno,n_addr);
   
   if((strlen(dp1.data))<511)
     return 0;
     }
    } 
   return 1;
}

void send_datapacket(char *message,int dpacket_size,int block,struct sockaddr_in myaddr){
    //cout<<message<<endl;
    struct data_packet dp;
    dp.opcode=3;
    dp.blockno=block;
    
    //cout<<dp.blockno<<" ";
    int i;
    
    for(i=0;message[i]!='\0';i++){
      dp.data[i]=message[i];
    }
    dp.data[i]='\0';
    //cout<<dp.data<<endl<<"--------------";
    
    /*copying data so that it can be retransmitted in case ack is not received after timeout*/
    g_clientaddr=myaddr;
    strcpy(last_data_sent,dp.data);
    g_block=block;
    g_dpacket_size=dpacket_size;
    
    if(sendto(sd,(struct data_packet*)&dp,sizeof(struct data_packet),0,(struct sockaddr*)&myaddr,sizeof(myaddr))<0){
      /* sending failed */
      /*
        code remaining for handling errors and timeout
      */
      cout<<"Sending failed"<<endl;
    }
    cout<<"Sending #"<<dp.blockno<<endl;
    recv_data_ack(dp.blockno);
}

void read_file(const char *filename,struct sockaddr_in myaddr){
  //cout<<filename<<" reading";
  FILE *fp;
  fp=fopen(filename,"r");
  
  /*
  
      add code for checking permission also
  
  */
  
  if(fp==NULL){
   
   /*
     code remaining handle this case for error packet   
   */
   send_errorpack(1,myaddr);   
   return;
  }
  //cout<<"File opened successfully"<<endl;
  
  /*sending packets in length of 512 bytes*/
  char ch;
  char message[512];
  char *p;

  int i=0;
  int block=1;
  

  while(1){
    ch=fgetc(fp);
   p=message;
   if(ch==EOF){
   message[i]='\0';
   send_datapacket(p,i,block,myaddr);
   break;
   }
   message[i]=ch;
   //cout<<message[i];
   i++;
   if(i==511){
      message[i]='\0';
      send_datapacket(p,i,block,myaddr);
      i=0;
      block++;
     }
   }
  
  //while(fgets(message,511,fp)!=NULL){
    
 
  fclose(fp);
}
void write_file(const char *filename,struct sockaddr_in myaddr){
  //cout<<filename<<" writing"<<endl;
  /* This function will receive packets from
      client side
  */
     while(1){
     if(!recv_datapacket(filename))
        break;
     }
}

void send_packet(struct sockaddr_in serveraddr){
   if(sendto(sd,(struct rwq_packet *)&rw,sizeof(struct rwq_packet),0,(struct sockaddr*)&serveraddr,sizeof(serveraddr))<0)
     cout<<"sending failed"<<endl;
     
     cout<<"packet sent"<<endl;
}

void send_ack(struct sockaddr_in* clientaddr){
  //cout<<"inside send_ack"<<endl;
  struct ack_packet a;
  a.opcode=4;
  a.blockno=0;
  if(sendto(sd,(struct ack_packet*)&a,sizeof(struct ack_packet),0,(struct sockaddr*) clientaddr,sizeof(*clientaddr))<0){
      cout<<"sending failed"<<endl;
          }
    
}

int recv_initial_ack(){

 struct ack_packet a;
 struct sockaddr_in serveraddr;
 socklen_t addrlen=sizeof(serveraddr);
 int len;
 
 
 /*After sending initial request for read or write session wait for the server's first acknowledgement
               If ack not received 10 times close the connection 
 */
 
 cout<<"hello";
 signal(SIGALRM,handler);
 alarm(3);
 int n=recvfrom(sd,(struct ack_packet*)&a,sizeof(struct ack_packet),0,(struct sockaddr*) &serveraddr,&addrlen);
    alarm(0);    
  
 //cout<<a.opcode;
 if(a.opcode==4 && a.blockno==0){
  //cout<<"connection successfully established and ack received"<<endl;
  return 1;
  }
  return 0;
}

void send_req(int flag,char *filename){
  
  if(flag==0){
    //cout<<"reading mode"<<endl;
    rw.opcode=1;
    }
    
    else if(flag==1){
     //cout<<"writing mode"<<endl;
     rw.opcode=2;
     }
     
     else
      return;
      
      int i;

      for(i=0;filename[i]!='\0';i++)
        rw.filename[i]=filename[i];

        rw.filename[i]='\0';
        rw.middle='0';
        rw.last='0';
        char mode[]="netascii";
        strcpy(rw.mode,mode);
      //cout<<filename;

      //cout<<rw.filename<<" "<<rw.opcode<<" "<<rw.mode<<" "<<rw.middle<<endl;
           }

#include<stdio.h>

int main(){
  
   struct first{
      int a;
      int b;
      int c;
   };
   
   struct second{
      int d;   
   };
   
   struct first f;
   f.a=10;
   f.b=20;
   f.c=30;
   struct second s;
   s=(struct s)f;
   
   printf("%d\n",s.d);

return 0;
}

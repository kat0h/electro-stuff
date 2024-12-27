#include <stdint.h>
#include <stdio.h>
// IR signal generator for HK9487MM
//   http://elm-chan.org/docs/ir_format.html
//   https://days-of-programming.blogspot.com/2017/05/blog-post.html
// AEHA format
// 0 = 1t 1t
// 1 = 1t 3t
// CH1
#define CC 0x344A // customer code
#define SIG_ON 0x034
#define SIG_OFF 0x0f4
void t0(uint8_t t){
  // printf("%d,",t);
  for(uint8_t i=0;i<t;i++)printf("_");
}
void t1(uint8_t t){
  // printf("%d,",t);
  for(uint8_t i=0;i<t;i++)printf("-");
}
void p0(){t1(1);t0(1);}
void p1(){t1(1);t0(3);}
void senddat(uint16_t dat,uint8_t len){for(uint8_t i=len;i--;i!=0)dat&1<<i?p1():p0();}
void mkdata(uint16_t cc,uint16_t dat){
  // leader
  t1(8);t0(4);
  senddat(cc,16); // customer code
  uint8_t c[4];for(uint8_t i=0;i<=12;i+=4)c[i/4]=(cc&0xf<<i)>>i; // customer code parity
  uint8_t p=c[0]^c[1]^c[2]^c[3];senddat(p,4);
  senddat(dat,12); // main data
  uint8_t p2=(p<<4|dat>>8)^dat&0xff; // parity
  senddat(p2,8);
  t1(1);// trailer
}

int main() {
  puts("ON");
  mkdata(CC,SIG_ON);puts("");
  puts("OFF");
  mkdata(CC,SIG_OFF);puts("");
}

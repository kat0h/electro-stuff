#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
// PA2 IR-LED
// PA7 ON
// PA1 OFF
#define PWMOFF() {TCA0.SINGLE.CTRLB&=~TCA_SINGLE_CMP2EN_bm;}
#define PWMON() {TCA0.SINGLE.CTRLB|=TCA_SINGLE_CMP2EN_bm;}
#define CC 0x344A // customer code
#define SIG_ON 0x034
#define SIG_OFF 0x0f4
void t0(uint8_t t){PWMOFF();_delay_us(425*t);}
void t1(uint8_t t){PWMON();_delay_us(425*t);}
void p0(){t1(1);t0(1);}
void p1(){t1(1);t0(3);}
void senddat(uint16_t dat,uint8_t len){for(uint8_t i=len;i--;i!=0)dat&1<<i?p1():p0();}
void mkdata(uint16_t cc,uint16_t dat){
  t1(8);t0(4);senddat(cc,16);
  uint8_t c[4];for(uint8_t i=0;i<=12;i+=4)c[i/4]=(cc&0xf<<i)>>i;
  uint8_t p=c[0]^c[1]^c[2]^c[3];senddat(p,4);
  senddat(dat,12);uint8_t p2=(p<<4|dat>>8)^dat&0xff;
  senddat(p2,8);t1(1);
}
void main() {
  _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, 0x00);
  PORTA.DIRSET=1<<2;// PA2 as OUTPUT
  TCA0.SINGLE.CTRLA=TCA_SINGLE_ENABLE_bm|TCA_SINGLE_CLKSEL_DIV2_gc; // 20MHz/2/PER=38KHz PER=263
  TCA0.SINGLE.CTRLB=TCA_SINGLE_CMP2EN_bm|TCA_SINGLE_WGMODE_SINGLESLOPE_gc;
  TCA0.SINGLE.PER=F_CPU/2/38000; // 38KHZ
  TCA0.SINGLE.CMP2=TCA0.SINGLE.PER/3; // 1/3duty
  PORTA.PIN1CTRL=PORT_PULLUPEN_bm;
  PORTA.PIN7CTRL=PORT_PULLUPEN_bm;
  while(1){
    if(!(PORTA.IN>>7&1))mkdata(CC,SIG_OFF);
    else if(!(PORTA.IN>>1&1))mkdata(CC,SIG_ON);
    PWMOFF();_delay_ms(50);
  }
}

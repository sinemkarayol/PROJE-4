#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/sysctl.c"
#include "driverlib/gpio.h"
#include "inc/hw_gpio.h"
#include "inc/tm4c123gh6pm.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"
#include "driverlib/pin_map.h"
#include "driverlib/adc.h"


#define LCDPORT             GPIO_PORTB_BASE
#define LCDPORTENABLE       SYSCTL_PERIPh_GPIOB
#define RS                  GPIO_PIN_0
#define E                   GPIO_PIN_1
#define D4                  GPIO_PIN_4
#define D5                  GPIO_PIN_5
#define D6                  GPIO_PIN_6
#define D7                  GPIO_PIN_7

uint32_t temp;

void otuzhexgonder();
void LCDkomut(unsigned char);
void LCDilkayarlar();
void LCDkarakteryaz(unsigned char);
void LCDfonksiyonayarla(unsigned char DL, unsigned char N, unsigned char F);
void LCDtemizle();
void LCDgit(unsigned char,unsigned char);
void LCDsaatYazdir();

void setinitsettings();
void saatkontrol();
void pcyesaatgonder();
void adcseridengonder(int deger);
void LCDsayiYaz(int sayi);
void adckesme();

int saatdizi[8] = {0,0,10,0,0,10,0,0};
//{1,3,10,4,5,10,3,5};  //10 a 48 eklersem 58 olur 58 ise ':'n n  asci kodu
int timerkesmesi;

uint32_t adcdata[4];

int gelenveri; //seriporttan
bool saatmi = false;
int gelensaatno=0;

int main(void)
{
    setinitsettings();
    LCDilkayarlar();
    LCDfonksiyonayarla(0, 1, 0); // 4-bit mod, 2 sat r, 5x8 karakter
    LCDtemizle();

    LCDgit(0, 0); // 1. Sat r, 1. S tun
    LCDkarakteryaz('s'); // Sabit metin ba lang c
    LCDkarakteryaz('i');
    LCDkarakteryaz('n');
    LCDkarakteryaz('e');
    LCDkarakteryaz('m');

    while(1)
    {
         timerkesmesi=TimerIntStatus(TIMER0_BASE, true);
         if(timerkesmesi!=0) //burada 1 sn ge ti
         {
             TimerIntClear(TIMER0_BASE, timerkesmesi);
             //1 sn ge mi tir
             if (GPIOPinRead(GPIO_PORTF_BASE , GPIO_PIN_1))
             {
                 GPIOPinWrite(GPIO_PORTF_BASE , GPIO_PIN_1, 0);
             }
             else
             {
                 GPIOPinWrite(GPIO_PORTF_BASE , GPIO_PIN_1, 2);
             }
             saatkontrol(); //saat g ncelllndi
             pcyesaatgonder(); //saat gitti
             LCDsaatYazdir();
             ADCProcessorTrigger(ADC0_BASE, 3);
             LCDgit(1, 0);
             LCDsayiYaz(temp);
         }

         if(ADCIntStatus(ADC0_BASE, 1,false))
         {
             ADCIntClear(ADC0_BASE, 3);
             ADCSequenceDataGet(ADC0_BASE, 3, adcdata);
             uint32_t ortalamadeger=(adcdata[0]+adcdata[1]+adcdata[2]+adcdata[3])/4;
             adcseridengonder(ortalamadeger);
         }

         if(!GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4))
         {
             UARTCharPut(UART0_BASE, '{');
             UARTCharPut(UART0_BASE, 's');
             UARTCharPut(UART0_BASE, 'i');
             UARTCharPut(UART0_BASE, 'n');
             UARTCharPut(UART0_BASE, 'e');
             UARTCharPut(UART0_BASE, 'm');
             UARTCharPut(UART0_BASE, '}');
             UARTCharPut(UART0_BASE, '\n');
         }

         if(UARTCharsAvail(UART0_BASE))
         {
             gelenveri=UARTCharGet(UART0_BASE);
             if(gelenveri =='%')
             {
                 //saat gelecek 8 tane
                 saatmi=true;
                 gelensaatno=0;
                 TimerDisable(TIMER0_BASE, TIMER_A);
             }
             else if(saatmi==true)
             {
                 saatdizi[gelensaatno]=gelenveri-48;
                 gelensaatno++;
                 if(gelensaatno==8)
                 {
                     saatmi=false;
                     gelensaatno=0;
                     TimerEnable(TIMER0_BASE, TIMER_A);
                     LCDsaatYazdir();
                     //LCDadcYazdir();
                 }
             }
         }
    }
}

void LCDsaatYazdir()
{
    LCDgit(1, 8); // LCD'nin ikinci sat r na git
    int a = 0 ;
    for (a = 0; a < 8; a++)
    {
        if (saatdizi[a] == 10)
        {
            LCDkarakteryaz(':'); // 10, ':' karakterine d n  t r l yor
        }
        else
        {
            LCDkarakteryaz(saatdizi[a] + 48); // Say lar  karaktere  evir ve yazd r
        }
    }
}

void adcseridengonder(deger)
{
    // 5,25,345,2345
    char hane3;
    char hane2;
    char hane1;
    char hane0;

    if(deger<10)
    {
        hane3=0;
        hane2=0;
        hane1=0;
        hane0=deger;
    }
    else if(deger<100)
    {
        hane3=0;
        hane2=0;
        hane0=(deger %10);
        hane1=(deger-hane0)/10;
    }
    else if(deger<1000)
    {
        hane3=0;
        hane0=(deger %10);
        hane1=((deger-hane0)/10)%10;
        hane2= (deger-hane1*10-hane0*1)/100;
    }
    else
    {
        hane0=(deger %10);
        hane1=((deger-hane0)/10)%10;
        hane2=((deger-hane1*10-hane0*1)/100)%100;
        hane3=(deger-100*hane2-10*hane1-1*hane0)/1000;
    }

    UARTCharPut(UART0_BASE, '(');  //adc gidiyor
    UARTCharPut(UART0_BASE, hane3+48);
    UARTCharPut(UART0_BASE, hane2+48);
    UARTCharPut(UART0_BASE, hane1+48);
    UARTCharPut(UART0_BASE, hane0+48);
    UARTCharPut(UART0_BASE, ')');
    UARTCharPut(UART0_BASE, '\n');
}

void pcyesaatgonder()
{
    int i;
    UARTCharPut(UART0_BASE, '[');
    for(i=0;i<8;i++)
    {
       UARTCharPut(UART0_BASE, saatdizi[i]+48);
    }
    UARTCharPut(UART0_BASE, '\n');
}
void saatkontrol()
{
    int sa = saatdizi[0]*10 + saatdizi[1];
    int dk = saatdizi[3]*10 + saatdizi[4];
    int sn = saatdizi[6]*10 + saatdizi[7];

    sn++;
    if(sn==60)
    {
        sn=0;
        dk++;
        if(dk==60)
        {
            dk=0;
            sa++;
            if(sa==24)
            {
                sa=0;
            }
        }
    }
    //15
    saatdizi[1]= sa % 10 ; //mod 10
    saatdizi[0]=(sa-saatdizi[1])/10;

    saatdizi[4]= dk % 10 ; //mod 10
    saatdizi[3]=(dk-saatdizi[4])/10;

    saatdizi[7]= sn % 10 ; //mod 10
    saatdizi[6]=(sn-saatdizi[7])/10;

}

void setinitsettings()
{
    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE , GPIO_PIN_1| GPIO_PIN_2 | GPIO_PIN_3);
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01;

    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE , GPIO_PIN_0| GPIO_PIN_4);
    GPIOPadConfigSet(GPIO_PORTF_BASE , GPIO_PIN_0| GPIO_PIN_4 , GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD_WPU);
    GPIOPinWrite(GPIO_PORTF_BASE , GPIO_PIN_1| GPIO_PIN_2| GPIO_PIN_3 , 0);

    //timer ayarlar
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    TimerConfigure(TIMER0_BASE, TIMER_CFG_A_PERIODIC); //down sayac
    TimerLoadSet(TIMER0_BASE, TIMER_A , SysCtlClockGet()); // 40 000 000 1 sn i in

    //3 yerel interrup ayarlar  //kesmesiz yapaca  m z i in di er kodlar  sildik
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT); //bayrak temizlendi

    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //sadece hangi pinleri uart olarak kullanaca  z
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0|GPIO_PIN_1);

    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200, UART_CONFIG_WLEN_8 |UART_CONFIG_STOP_ONE |UART_CONFIG_PAR_NONE);
    UARTEnable(UART0_BASE);

    // ADC0 yap land rmas
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH0 | ADC_CTL_IE | ADC_CTL_END);
    ADCSequenceEnable(ADC0_BASE, 3);

    IntMasterEnable();
    IntEnable(INT_ADC0SS3);
    ADCIntRegister(ADC0_BASE, 3, adckesme);
    ADCIntClear(ADC0_BASE, 3);
    ADCIntEnable(ADC0_BASE, 3);

    TimerEnable(TIMER0_BASE, TIMER_A);//setb TR0

}

//***********LCD.C ******
void otuzhexgonder()
{
    SysCtlDelay(10000000);
    //rs=0
    GPIOPinWrite(LCDPORT, RS, 0);
    //ayar yaz ld
    GPIOPinWrite(LCDPORT, D4|D5|D6|D7, 0x30);   //30h g ndermi  olduk yani
    //en ac kapa
    GPIOPinWrite(LCDPORT, E, 2);  //P N1  2  LE ACILIYORDU
    SysCtlDelay(1000);
    GPIOPinWrite(LCDPORT, E, 0);
    //3 kez 30h gonder
}

void LCDkomut(unsigned char c)   //0x07 g ndermek istersek
{
    GPIOPinWrite(LCDPORT, RS, 0);      //O DI DE   T RD M
    GPIOPinWrite(LCDPORT, D4|D5|D6|D7, (c & 0xf0));
    GPIOPinWrite(LCDPORT, E, 2);  //P N1  2  LE ACILIYORDU
    SysCtlDelay(1000);
    GPIOPinWrite(LCDPORT, E, 0);

    SysCtlDelay(1000);

    GPIOPinWrite(LCDPORT, RS,0 );
    GPIOPinWrite(LCDPORT, D4|D5|D6|D7, (c & 0x0f)<<4);
    GPIOPinWrite(LCDPORT, E, 2);  //P N1  2  LE ACILIYORDU
    SysCtlDelay(1000);
    GPIOPinWrite(LCDPORT, E, 0);

    SysCtlDelay(1000);
}

void LCDilkayarlar()
{
    //portB_base_enable
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, 0xFF);
    //portb de kitli pin yok

    otuzhexgonder();
    otuzhexgonder();
    otuzhexgonder();

    LCDkomut(0x02);
}

void LCDfonksiyonayarla(unsigned char DL ,unsigned char N,unsigned char F)
{
    unsigned char cmd = 0x20; // Fonksiyon set komutu ba lang c
    if (DL) cmd |= 0x10; // DL: Data Length (0 = 4-bit, 1 = 8-bit)
    if (N) cmd |= 0x08;  // N: Display lines (0 = 1 sat r, 1 = 2 sat r)
    if (F) cmd |= 0x04;  // F: Font (0 = 5x8, 1 = 5x10)
    LCDkomut(cmd); // Fonksiyon set komutunu g nder
}

void LCDkarakteryaz(unsigned char c)  //
{
    GPIOPinWrite(LCDPORT, RS, RS); // Veri modu

    //  st 4 bit g nder
    GPIOPinWrite(LCDPORT, D4|D5|D6|D7, (c & 0xf0));
    GPIOPinWrite(LCDPORT, E, 2); // Enable pinini y ksek yap
    SysCtlDelay(1000); // Gecikme
    GPIOPinWrite(LCDPORT, E, 0); // Enable pinini s f rla

    SysCtlDelay(1000); // Gecikme

    // Alt 4 bit g nder
    GPIOPinWrite(LCDPORT, D4 | D5 | D6 | D7, (c & 0x0f) << 4);
    GPIOPinWrite(LCDPORT, E, 2); // Enable pinini y ksek yap
    SysCtlDelay(1000); // Gecikme
    GPIOPinWrite(LCDPORT, E, 0); // Enable pinini s f rla

    SysCtlDelay(1000); // Gecikme
}

void LCDtemizle()
{
    LCDkomut(0x01);
    SysCtlDelay(1000);
}

void LCDgit(unsigned char k,unsigned char m)
{
    unsigned char address;
    if (k == 0) address = 0x80 + m; // 1. Sat r
    else if (k == 1) address = 0xC0 + m; // 2. Sat r

    LCDkomut(address);
}
////////////////////////////////////////////////
void adckesme()
{
    uint32_t adcdata[1];
    ADCSequenceDataGet(ADC0_BASE, 3, adcdata);
    uint32_t adcDegeri = adcdata[0];
    temp = (adcDegeri * 3300) / 4096; // mV
    temp = temp / 10;                //  C'ye  evirme
    ADCIntClear(ADC0_BASE, 3);
}

void LCDsayiYaz(int sayi)
{
    char buffer[10];
    sprintf(buffer, "%d", sayi);
    int i;
    for (i = 0; buffer[i] != '\0'; i++)
    {
       LCDkarakteryaz(buffer[i]);
    }
}

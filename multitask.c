#include <stdio.h> /* prototype declarations for I/O functions */
#include <string.h>
#include <math.h>
#include <reg51.h>
#define TRUE 1
#define FALSE 0
unsigned int task0_interval, task1_interval, task2_interval;
unsigned int task0_max_interval, task1_max_interval, task2_max_interval;
bit task0_enabled, task1_enabled, task2_enabled;
float task0_variable, task1_variable, variable;
//---------------------------------------------------------
//---------------------------------------------------------------------------
void Timer0_Initial(void)
{
TMOD=TMOD&0xF2;
TR0=1;
ET0=1;
}
//---------------------------------------------------------------------------
void Serial_Initial(void)
{
PCON=PCON|0x80;//Smod=1
SCON = 0x52; // SCON: mode 1, 8-bit UART, enable rcvr
TMOD = 0x20; // TMOD: timer 1, mode 2, 8-bit reload
TH1 = TL1 = 0xfd; // TH1: reload value for 19200 baud
TR1 = 1; // TR1: timer 1 run
ES=1;//serial interrupt enabled
RI=0;
}
//---------------------------------------------------------
Putch(unsigned char c)
{
TI=0;
SBUF=c;
while( !TI );
}
//---------------------------------------------------------
unsigned char Getch()
{
unsigned char c;
while( !RI );
c = SBUF;
RI = 0;
return (c);
}
//---------------------------------------------------------
void Serial_Printf ( char *c )
{
unsigned char m;
for(m=0; c[m] != '\n'; m++)
{
if(m>80) return;
Putch(c[m]);
}
}
//---------------------------------------------------------
unsigned char Char_Msb(unsigned char c)
{
c=c&240;
c=c>>4;
c=c+48;
return(c);
} 
//---------------------------------------------------------
unsigned char Char_Lsb(unsigned char c)
{
c=c&15;
c=c+48;
return(c);
}
//---------------------------------------------------------
unsigned char BinToDec(unsigned char c)
{
if (c>89) c=c+6;
if (c>79) c=c+6;
if (c>69) c=c+6;
if (c>59) c=c+6;
if (c>49) c=c+6;
if (c>39) c=c+6;
if (c>29) c=c+6;
if (c>19) c=c+6;
if (c>9) c=c+6;
return(c);
}
//---------------------------------------------------------
void Serial_Write_Asci ( unsigned char c )
{
unsigned char tmp;
tmp = BinToDec(c);
Putch( Char_Msb(tmp) );
Putch( Char_Lsb(tmp) );
} 
//---------------------------------------------------------
char *Serial_Scanf()
{
unsigned char i, c;
char cm[16];
i=0;
do
{
c = Getch();
cm[i]= c;
i=i+1;
} while ( c != 0x0D );
i=i-1;
cm[i]='\n';
return (&cm);
} 
//---------------------------------------------------------
void Serial_Interrupt(void) interrupt 4
{
char cmd[16];
EA=0;//global interrupt disable
TI=0;
if( RI==1 )
	{
	strcpy( &cmd, Serial_Scanf() );
	if( !strcmp(cmd,"t task0\n") ) task0_enabled=FALSE; // command = terminate task0 ...
	if( !strcmp(cmd,"t task1\n") ) task1_enabled=FALSE;
	if( !strcmp(cmd,"t task2\n") ) task2_enabled=FALSE;
	if( !strcmp( cmd,"c task0\n") ) task0_enabled=TRUE;
	if( !strcmp( cmd,"c task1\n") ) task1_enabled=TRUE;
	if( !strcmp( cmd,"c task2\n") ) task2_enabled=TRUE;
	if( !strcmp( cmd,"i task0\n") ) task0_max_interval = task0_max_interval / 2;
	if( !strcmp( cmd,"i task1\n") ) task1_max_interval = task1_max_interval / 2;
	if( !strcmp( cmd,"i task2\n") ) task2_max_interval = task2_max_interval / 2;
	if( !strcmp( cmd,"d task0\n") ) task0_max_interval = task0_max_interval * 2;
	if( !strcmp( cmd,"d task1\n") ) task1_max_interval = task1_max_interval * 2;
	if( !strcmp( cmd,"d task2\n") ) task2_max_interval = task2_max_interval * 2;
	}
EA=1;//global interrupt enable
}
//---------------------------------------------------------
void timer0 (void) interrupt 1
{
char tmp[32];
EA=0;//global interrupt disable
ET0=0;//timer intr dis
task0_interval++;
task1_interval++;
task2_interval++;
variable += 0.0001;
if( (task0_enabled)&&(task0_interval>=task0_max_interval) ) //1 sn for 100
	{
	task0_interval=0;
		{
		task0_variable += 1; // write task0's program here..
		//...........
		}
	}
if( (task1_enabled)&&(task1_interval>=task1_max_interval) ) //2sn for 200
	{
	task1_interval=0;
		{
		task1_variable += 1; // write task1's program here..
		//...........
		}
	}
if( (task2_enabled)&&(task2_interval>=task2_max_interval) ) //0.1sn for 10
	{
	task2_interval=0;
		{ //write task2's program here..
		if(task0_enabled==TRUE)
			{
			sprintf( tmp," Task0 is executing.. %0.0f\n",task0_variable);
			Serial_Printf(&tmp);
			}
		if(task1_enabled==TRUE)
			{
			sprintf( tmp," Task1 is executing.. %0.0f\n",task1_variable);
			Serial_Printf(&tmp);
			}
		Putch(0x0D);//enter, next line //task2's program ends here.
		}
	}
TF0=0;//timer0 flag
ET0=1;//timer intr enb
EA=1;//global interrupt enable
}
//---------------------------------------------------------
/****************/
/* main program */
/****************/
void main (void) { /* execution starts here after stack init */
IE=0x00;//all individual interrupts are disabled
EA=1;//global interrupt enable
Timer0_Initial();
Serial_Initial();
task0_enabled=TRUE;
task1_enabled=TRUE;
task2_enabled=TRUE;
task0_max_interval=100;//1sn these max interval values defines the priorites...
task1_max_interval=200;//2sn max priority is given to min interval value...
task2_max_interval=50;//0.5sn
	while (1) { /* An embedded program does not stop and */
	; /* ... */ /* never returns. We've used an endless */
	} /* loop. You may wish to put in your own */
}



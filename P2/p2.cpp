#include <dos.h>
#include <conio.h>

#define POSITION 1994
#define F12 88
#define USECONDS_CLOCKINTERRUPTION 54945

#define bool int
#define true 1
#define false 0

/*
void enable(void);        // habilitar interrupciones hardware, equivalente a STI
void disable(void);       // inhibir interrupciones hardware, equivalente a CLI
*/

void interrupt (* OldHandler08H)(...);
void interrupt (* OldHandler09H)(...);
void calculateTime(void);
void buildTimeText(void);
void buildTimeTextUSec(void);

struct dostime_t tm;
unsigned int hour, minute, second;
unsigned long int usecond;
unsigned int eger;
bool state;

void interrupt NewHandler08H(...){//clock
	asm sti
	//calculates the time from an initial time, every time that clock interruption is called
	calculateTime();
	buildTimeText();
	if(state){//clock enhaced
		buildTimeTextUSec();
	}
	OldHandler08H();
}

void interrupt NewHandler09H(...){//keyboard
	asm sti
	if(eger == F12){//switch mode
		if(state){
			state = false;
		}
		else{
			state = true;
		}
	}
	OldHandler09H();
}

void calculateTime(void){//18,2times/second
	usecond = usecond + USECONDS_CLOCKINTERRUPTION;//54.945,05494505495
	if(usecond >= 1000000){//microseconds
		second = second + 1;
		usecond = 0;
		if(second == 60){//seconds
			minute = minute + 1;
			second = 0;
			if(minute == 60){//minutes
				hour = hour + 1;
				minute = 0;
				if(hour == 24){//hours
					hour = 0;
				}//end hours
			}//end minutes
		}//end seconds
	}//end microseconds
}

void buildTimeText(void){
	char a_hour[2];
	char a_minute[2];
	char a_second[2];
	//HOURS
	//Load hour
	a_hour[0] = hour/10 + 48;
	a_hour[1] = hour%10 + 48;
	//Print hour
	pokeb(0xB800, POSITION, a_hour[0]);
	pokeb(0xB800, POSITION+2, a_hour[1]);
	pokeb(0xB800, POSITION+4, ':');
	//MINUTES
	//load minute
	a_minute[0] = minute/10 + 48;
	a_minute[1] = minute%10 + 48;
	//Print minute
	pokeb(0xB800, POSITION+6, a_minute[0]);
	pokeb(0xB800, POSITION+8, a_minute[1]);
	pokeb(0xB800, POSITION+10, ':');
	//SECONDS
	//Load second
	a_second[0] = second/10 + 48;
	a_second[1] = second%10 + 48;
	//Print second
	pokeb(0xB800, POSITION+12, a_second[0]);
	pokeb(0xB800, POSITION+14, a_second[1]);
	for(int i=8; i < 15; i++){
		pokeb(0xB800, POSITION+(i*2), ' ');
	}
}

void buildTimeTextUSec(void){
	char a_usecond[6];
	unsigned long int aux = usecond;
	//USECONDS
	//Load usecond
	int i = 0;
	for(;i < 6;i++){
		a_usecond[5-i] = aux%10 + 48;
		aux = aux/10;
	}
	//Print usecond
	pokeb(0xB800, POSITION+16, ':');
	for(i=0; i < 6; i++){
		pokeb(0xB800, POSITION+18+(i*2), a_usecond[i]);
	}
}

void main(){
	state = false;
	clrscr();
	_setcursortype(_NOCURSOR);
	_dos_gettime(&tm);//initial time
	hour = tm.hour; minute = tm.minute; second = tm.second; usecond = 0;
	//clock interruption setted
	OldHandler08H = getvect(0x08);
	setvect(0x08, NewHandler08H);
	//keyboard interruption setted
	OldHandler09H = getvect(0x09);
	setvect(0x09, NewHandler09H);
	do{
		eger = inportb(0x60);
	}while (eger != 1);
	//restore interruptions
	setvect(0x08, OldHandler08H);
	setvect(0x09, OldHandler09H);
	_setcursortype(_NORMALCURSOR);
	clrscr();
}

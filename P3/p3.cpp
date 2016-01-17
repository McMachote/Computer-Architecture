#include <dos.h>
#include <conio.h>
#include <stdio.h>

#define POSITION 1994
#define F12_DOWN 88
#define F12_UP 216
#define ESC 1

#define USECONDS_CLOCKINTERRUPTION 54945
#define CLOCKDISPLAY_SIZE 8

#define bool int
#define true 1
#define false 0


void interrupt (* OldHandler08H)(...);
void interrupt (* OldHandler09H)(...);
void calculateTime(void);
void buildTimeText(void);
void savePreviousScreen(void);
void restorePreviousScreen(void);


unsigned int hour, minute, second;
unsigned int eger;
unsigned long int usecond;
unsigned char restore_char_arr[CLOCKDISPLAY_SIZE];
unsigned char restore_attr_arr[CLOCKDISPLAY_SIZE];
bool state;


int convertBCDToInt(int bcd){
	return bcd & 0x0F + 10 * (bcd >> 4);
}

void savePreviousScreen(void){
	int i;
	for(i=0; i < CLOCKDISPLAY_SIZE; i++){
		restore_char_arr[i] = peekb(0xB800, POSITION+i*2);
	}
	for(i=0; i < CLOCKDISPLAY_SIZE; i++){
		restore_attr_arr[i] = peekb(0xB800, POSITION+i*2+1);
	}
}

void restorePreviousScreen(void){
	int i;
	for(i=0; i < CLOCKDISPLAY_SIZE; i++){
		pokeb(0xB800, POSITION+(i*2), restore_char_arr[i]);
	}
	for(i=0; i < CLOCKDISPLAY_SIZE; i++){
		pokeb(0xB800, POSITION+(i*2+1), restore_attr_arr[i]);
	}
}

void interrupt NewHandler08H(...){//clock
	asm sti
	calculateTime();
	if(state){
		buildTimeText();
	}
	OldHandler08H();
}

void interrupt NewHandler09H(...){//keyboard
	asm sti
	eger = inportb(0x60);
	if(eger == F12_DOWN){//} || eger == F12_UP){//switch mode
		if(state){
			state = false;
			restorePreviousScreen();
		}
		else{
			savePreviousScreen();
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
}

void main(){
	state = false;
	clrscr();
	_setcursortype(_NOCURSOR);
	
	unsigned int hourAux, minuteAux, secondAux;
	outportb(0x70, 4); hourAux   = inportb(0x71);
	outportb(0x70, 2); minuteAux = inportb(0x71);
	outportb(0x70, 0); secondAux = inportb(0x71);
	hour = convertBCDToInt(hourAux);
	minute = convertBCDToInt(minuteAux);
	second = convertBCDToInt(secondAux);
	
	OldHandler08H = getvect(0x08);
	setvect(0x08, NewHandler08H);
	OldHandler09H = getvect(0x09);
	setvect(0x09, NewHandler09H);
	
	_setcursortype(_NORMALCURSOR);
	keep(0, (_SS + ((_SP)/16) - _psp));
}

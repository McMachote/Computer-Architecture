#include <dos.h>
#include <conio.h>
#include <bios.h>

#define SCREEN_SIZE 80*25

#define K_UP 72
#define K_DOWN 80
#define K_LEFT 75
#define K_RIGHT 77
#define K_PLUS 27
#define K_MINUS 53

void writeText(unsigned char *mander, unsigned int &pos, unsigned int color, unsigned int stringSize);

void main(){
	clrscr();
	_setcursortype(_NOCURSOR);
	unsigned char mander[] = "Hello world";
	unsigned int stringSize = sizeof(mander) / sizeof(mander[0]);
	unsigned int eger = 0;
	unsigned int sizeLine = 80 * 2;
	unsigned int current_pos = 0x0000 + 12 * sizeLine + (40 * 2) - (stringSize/2);//with initial value
	unsigned int color = 0x42;
	
	do{
		writeText(mander, current_pos, color, stringSize);
		while(!kbhit()){}
		eger = inportb(0x60);
		switch(eger){
			case K_RIGHT:
				if(current_pos % sizeLine < sizeLine - stringSize * 2){
					current_pos = current_pos + 2;
				}
				break;
			case K_LEFT:
				if(current_pos % sizeLine > 0){
					current_pos = current_pos - 2;
				}
				break;
			case K_UP:
				if(current_pos >= (80 * 2)){
					current_pos = current_pos - (80 * 2);
				}
				break;
			case K_DOWN:
				if(current_pos < (80 * 2 * 24)){
					current_pos = current_pos + (80 * 2);
				}
				break;
			case K_PLUS:
				//color = color + 4;
				color = 0x42;
				break;
			case K_MINUS:
				color = 0x24;
				break;
			default:
				break;
		}
		//the keyboard buffer is full
		bioskey(0);
	}while (eger != 1);
	_setcursortype(_NORMALCURSOR);
	clrscr();
}

void writeText(unsigned char *mander, unsigned int &pos, unsigned int color, unsigned int stringSize){
	unsigned int i = 0;
	clrscr();
	for(; i < stringSize; i++){
		pokeb(0xB800, pos + i*2,  mander[i]);
	}
	for(i = 0; i < SCREEN_SIZE; i++){
		pokeb(0xB800, i*2 + 1,  color);
	}
}

#include <dos.h>
#include <conio.h>
#include <stdio.h>
#include <iostream.h>

#define K_UP 72
#define K_DOWN 80
#define SPACE_BAR 57
#define ESC 1

void interrupt (* OldHandler09H)(...);

//--------------------------------------------------------------------------//

typedef unsigned char BYTE;
typedef unsigned int  WORD;
typedef unsigned int  UINT;
typedef unsigned long DWORD;
typedef unsigned long LONG;

//--------------------------------------------------------------------------//

struct BITMAPFILEHEADER{
	UINT  bfType;
	DWORD bfSize;
	UINT  bfReserved1;
	UINT  bfReserved2;
	DWORD bfOffBits;
};

struct BITMAPINFOHEADER{
	DWORD biSize;
	LONG  biWidth;
	LONG  biHeight;
	WORD  biPlanes;
	WORD  biBitCount;
	DWORD biCompression;
	DWORD biSizeImage;
	LONG  biXPelsPerMeter;
	LONG  biYPelsPerMeter;
	DWORD biClrUsed;
	DWORD biClrImportant;
};

struct RGBQUAD{
	BYTE rgbBlue;
	BYTE rgbGreen;
	BYTE rgbRed;
	BYTE rgbReserved;
};

//--------------------------------------------------------------------------//

BITMAPFILEHEADER bmfh;
BITMAPINFOHEADER bmih;
RGBQUAD palette[256];
BYTE *video_memory = (BYTE *)0xA0000000L;
FILE *bitmap_file;

unsigned int brightness_factor = 128;
unsigned int eger;

//--------------------------------------------------------------------------//

void xorSwap (int *x, int *y) {
     if (x != y) {
         *x ^= *y;
         *y ^= *x;
         *x ^= *y;
     }
 }
 
 //--------------------------------------------------------------------------//

void set_video_mode(int mode){
	REGPACK regs;
	regs.r_ax = mode;
	intr(0x10, &regs);
}

//--------------------------------------------------------------------------//

void set_video_palette(){ //color rescale
	outportb(0x038C, 0);
	for(int i=0; i<bmih.biClrUsed; i++){
		outp(0x03C9, palette[i].rgbRed * 63/255);
		outp(0x03C9, palette[i].rgbGreen * 63/255);
		outp(0x03C9, palette[i].rgbBlue * 63/255);
	}
}

void flip_image(){
	int W = bmih.biWidth;
	int H = bmih.biHeight;
	int N = bmih.biSizeImage-1;
	for(int i=0; i<H ; i++){
		fread(&video_memory[N-i*W], W, 1, bitmap_file);
	}
}

void load_image_data(char *file_name){
	bitmap_file = fopen(file_name, "rb");
	fread(&bmfh, sizeof(bmfh), 1, bitmap_file);
	fread(&bmih, sizeof(bmih), 1, bitmap_file);
	fread(&palette[0], bmih.biClrUsed * sizeof(RGBQUAD),  1, bitmap_file);
}

void display_image_data(){
	flip_image();
	fclose(bitmap_file);
}

void negative(){
	for(int i=0; i<bmih.biClrUsed; i++){
		palette[i].rgbRed = 255 - palette[i].rgbRed;
		palette[i].rgbGreen = 255 - palette[i].rgbGreen;
		palette[i].rgbBlue = 255 - palette[i].rgbBlue;
	}
}

void brightness(int x){
	if(x > 0){
		for(int i=0; i<bmih.biClrUsed; i++){
			if(palette[i].rgbRed < 255){
				palette[i].rgbRed +=x;
			}
			if(palette[i].rgbGreen < 255){
				palette[i].rgbGreen += x;
			}
			if(palette[i].rgbBlue < 255){
				palette[i].rgbBlue += x;
			}
		}
	}
	else{
		for(int i=0; i<bmih.biClrUsed; i++){
			if(palette[i].rgbRed > 0){
				palette[i].rgbRed -=x;
			}
			if(palette[i].rgbGreen > 0){
				palette[i].rgbGreen -= x;
			}
			if(palette[i].rgbBlue > 0){
				palette[i].rgbBlue -= x;
			}
		}
	}
}

//--------------------------------------------------------------------------//

void interrupt NewHandler09H(...){
	asm sti
	switch(eger){
		case K_UP:
			brightness(1);
			break;
		case K_DOWN:
			brightness(-1);
			break;
		case SPACE_BAR:
			negative();
			break;
		default:
			break;
	}
	OldHandler09H();
}

//--------------------------------------------------------------------------//

void main(int argc, char *argv[]){
	if (argc != 2) {
		cout << "Incorrect number of arguments." << endl;
		return; 
	}
	set_video_mode(0x13);
	load_image_data(argv[1]);
	set_video_palette();
	OldHandler09H = getvect(0x09);
	setvect(0x09, NewHandler09H);
	display_image_data();
	do{
		set_video_palette();
		eger = inportb(0x60);
	}while (eger != ESC);
	setvect(0x09, OldHandler09H);
	getch();
	set_video_mode(0x03);
}

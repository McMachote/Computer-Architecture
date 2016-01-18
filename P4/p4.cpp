#include <dos.h>
#include <stdio.h>

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

unsigned int brightness_factor = 1;
unsigned int eger;
 
 //--------------------------------------------------------------------------//

void set_video_mode(int mode){
	REGPACK regs;
	regs.r_ax = mode;
	intr(0x10, &regs);
}

//--------------------------------------------------------------------------//

void set_video_palette(){ //color rescale
	outportb(0x038C, 0);
	for(int i=0; i<bmih.biClrUsed; i++){ //bit shifting improves!
		outp(0x03C9, palette[i].rgbRed >> 2);
		outp(0x03C9, palette[i].rgbGreen >> 2);
		outp(0x03C9, palette[i].rgbBlue >> 2);
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
	set_video_palette();
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
	int redAux, greenAux, blueAux;
	for(int i=0; i<bmih.biClrUsed; i++){
		redAux = palette[i].rgbRed + x;
		greenAux = palette[i].rgbGreen + x;
		blueAux = palette[i].rgbBlue + x;
		//truncate
		redAux = (redAux <= 255) ? redAux : 255;
		redAux = (redAux >= 0) ? redAux : 0;
		greenAux = (greenAux <= 255) ? greenAux : 255;
		greenAux = (greenAux >= 0) ? greenAux : 0;
		blueAux = (blueAux <= 255) ? blueAux : 255;
		blueAux = (blueAux >= 0) ? blueAux : 0;
		palette[i].rgbRed = redAux;
		palette[i].rgbGreen = greenAux;
		palette[i].rgbBlue = blueAux;
	}
}

//--------------------------------------------------------------------------//

void interrupt NewHandler09H(...){
	asm sti
	switch(eger){
		case K_UP:
			brightness(brightness_factor);
			break;
		case K_DOWN:
			brightness(-brightness_factor);
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
		printf("Incorrect number of arguments.");
		return; 
	}
	set_video_mode(0x13);
	load_image_data(argv[1]);
	set_video_palette();
	OldHandler09H = getvect(0x09);
	setvect(0x09, NewHandler09H);
	do{
		set_video_palette();
		eger = inportb(0x60);
	}while (eger != ESC);
	setvect(0x09, OldHandler09H);
	set_video_mode(0x03);
}

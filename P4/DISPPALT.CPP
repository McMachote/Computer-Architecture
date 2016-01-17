#include <stdio.h>
#include <conio.h>
#include <dos.h>
#include <iostream.h>

//--------------------------------------------------------------------------//

typedef unsigned char BYTE;
typedef unsigned int  WORD;
typedef unsigned int  UINT;
typedef unsigned long DWORD;
typedef unsigned long LONG;

//--------------------------------------------------------------------------//

struct BITMAPFILEHEADER
{
 UINT  bfType;
 DWORD bfSize;
 UINT  bfReserved1;
 UINT  bfReserved2;
 DWORD bfOffBits;
};

struct BITMAPINFOHEADER
{
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

struct RGBQUAD
{
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

//--------------------------------------------------------------------------//

void set_video_mode(int mode)
{
 REGPACK regs;
 regs.r_ax = mode;
 intr(0x10, &regs);
}

//--------------------------------------------------------------------------//

void set_video_palette()
{
 outportb(0x03C8, 0);
 for (int i = 0; i < bmih.biClrUsed; i++)
 {
  outp(0x03C9, palette[i].rgbRed   * 63 / 255);
  outp(0x03C9, palette[i].rgbGreen * 63 / 255);
  outp(0x03C9, palette[i].rgbBlue  * 63 / 255);
 }
}

//--------------------------------------------------------------------------//

void display_palette(char *file_name)
{
 bitmap_file = fopen(file_name, "rb");
 fread(&bmfh, sizeof(bmfh), 1, bitmap_file);
 fread(&bmih, sizeof(bmih), 1, bitmap_file);
 fread(&palette[0], bmih.biClrUsed * sizeof(RGBQUAD), 1, bitmap_file);
 fclose(bitmap_file);
 set_video_palette();
 for (int i = 0; i < bmih.biClrUsed && i < 200; i++)
  for (int j = 0; j < 320; j++)
   video_memory[i * 320 + j] = i;
}

//--------------------------------------------------------------------------//

void main(int argc, char *argv[])
{
 if (argc != 2) 
 { 
  cout << "Incorrect number of arguments." << endl;
  return; 
 }
 set_video_mode(0x13);
 display_palette(argv[1]);
 getch();
 set_video_mode(0x03);
}

//--------------------------------------------------------------------------//

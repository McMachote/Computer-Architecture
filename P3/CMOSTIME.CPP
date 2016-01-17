#include <conio.h>
#include <dos.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream.h>

//----------------------------------------------------------------------------

void main()
{
 unsigned char hour, minute, second;
 clrscr(); _setcursortype(_NOCURSOR); 
 while (kbhit()) getch();
 do
 {
  outportb(0x70, 4); hour   = inportb(0x71);
  outportb(0x70, 2); minute = inportb(0x71);
  outportb(0x70, 0); second = inportb(0x71);
  gotoxy(36, 12);
  cout << (int)hour << ":" << (int)minute << ":" << (int)second << "     " << endl;
 }
 while (!kbhit());
 clrscr(); _setcursortype(_NORMALCURSOR);
 while (kbhit()) getch();
}

#include <conio.h>
#include <dos.h>
#include <string.h>
#include <stdlib.h>

//----------------------------------------------------------------------------

unsigned char value = 0;
void interrupt (* OldHandler08H)(...);

//----------------------------------------------------------------------------

void interrupt NewHandler08H(...)
{
 char text[16]; int i;
 for (i = 0; i < 15; i++) text[i] = ' ';
 text[15] = 0;
 itoa(value, text, 10);
 text[strlen(text)] = ' ';
 for (i = 0; i < strlen(text); i++)
  pokeb(0xB800, 12 * 160 + (40 + i) * 2, text[i]);
 value++;
 outp(0x20,0x20);
}

//----------------------------------------------------------------------------

void main()
{
 textcolor(LIGHTGRAY);
 textbackground(BLACK);
 _setcursortype(_NOCURSOR);
 clrscr();
 OldHandler08H = getvect(0x08);//guarda la función de 0x08 en OldHandler08H
 setvect(0x08, NewHandler08H);
 while (kbhit()) getch(); getch();
 setvect(0x08, OldHandler08H);
 _setcursortype(_NORMALCURSOR);
 clrscr();
}

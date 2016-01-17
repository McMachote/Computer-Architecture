#include <stdlib.h>
#include <dos.h>
#include <conio.h>
#include <string.h>
#include <iostream.h>

//----------------------------------------------------------------------------


void interrupt (* OldHandler09H)(...);

//----------------------------------------------------------------------------

void interrupt NewHandler09H(...)
{
 asm sti
 int i;
 char text[] = "Hello from keyboard interrupt..."; 
 for (i = 0; i < strlen(text); i++)
  pokeb(0xB800, 11 * 160 + (25 + i) * 2, text[i]);
 delay(300);
 for (i = 0; i < strlen(text); i++)
  pokeb(0xB800, 11 * 160 + (25 + i) * 2, ' ');
 OldHandler09H();
}

//----------------------------------------------------------------------------

void main()
{
 char dummy_text[256];
 textcolor(LIGHTGRAY);
 textbackground(BLACK);
 clrscr();
 while (kbhit()) getch(); delay(100);
 OldHandler09H = getvect(0x09);
 setvect(0x09, NewHandler09H);
 cout << "Type some dummy text: ";
 cin >> dummy_text;
 setvect(0x09, OldHandler09H);
 clrscr();
}

#include   <stdio.h>
#include   <dos.h>
#include   <conio.h>

int main(void)
{
 struct dostime_t tm;
 clrscr();
 _dos_gettime(&tm);
 printf("The current time is: %02d:%02d:%02d\n", tm.hour, tm.minute, tm.second);
 getch();
 return 0;
}

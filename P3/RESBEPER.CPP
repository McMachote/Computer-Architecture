#include <dos.h>

int  pressed = 0;
void interrupt (*old_handler_keyboard_09h)(...);

void interrupt new_handler_keyboard_09h(...)
{
 old_handler_keyboard_09h();
 if (pressed == 0)
 {
  pressed = 1;
  sound(200); delay(100); nosound();
 }
 else pressed = 0;
}

void main()
{
 old_handler_keyboard_09h  = getvect(0x09);
 setvect(0x09, new_handler_keyboard_09h);
 keep(0, (_SS + ((_SP)/16) - _psp));
}

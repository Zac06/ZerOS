#include<ps2/ps2_keyboard.hpp>
#include<i686/io.h>
#include<stddef.h>

ps2_keyboard::ps2_keyboard(int p_port_no)
    :ps2_driver(p_port_no)
{

}

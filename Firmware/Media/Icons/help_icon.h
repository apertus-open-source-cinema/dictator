#ifndef HELP_ICON_H
#define HELP_ICON_H

#include <stdint.h>

#include "../../UI/Widgets/Icon.h"
 
static struct help_icon : public Icon {
 
    uint8_t data[24 * 24] = {
0x00, 0x7E, 0x00, 0x00, 0xFF, 0x00, 0xC0, 0x81, 0x03, 0xC0, 0x00, 0x03, 0x60, 0x00, 0x06, 0x60, 0x00, 0x06, 0x60, 0x00, 0x06, 0x60, 0x00, 0x06, 0x00, 0x00, 0x06, 0x00, 0x00, 0x06, 0x00, 0x00, 0x06, 0x00, 0x00, 0x03, 0x00, 0xC0, 0x01, 0x00, 0xE0, 0x00, 0x00, 0x30, 0x00, 0x00, 0x18, 0x00, 0x00, 0x18, 0x00, 0x00, 0x18, 0x00, 0x00, 0x18, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x18, 0x00, };
 
    help_icon()
    {
        Icon::Width = 24;
        Icon::Height = 24;
        Icon::Data = &data[0];
    }
}help_icon;

#endif // HELP_ICON_H
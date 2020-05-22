#include <stdint.h>

#include "../../UI/Widgets/Icon.h"
 
static const struct  ApertusRingLogo : public Icon {
 
    uint8_t data[14 * 14] = {
0xF0, 0x03, 0xFC, 0x0F, 0xFE, 0x1F, 0xFE, 0x1F, 0x1F, 0x3C, 0x0F, 0x3C, 0x0F, 0x3C, 0x0F, 0x3C, 0x0F, 0x3C, 0x1F, 0x3E, 0xFE, 0x1F, 0xFE, 0x1F, 0xFC, 0x0F, 0xF0, 0x03, };
 
     ApertusRingLogo()
    {
        Icon::Width = 14;
        Icon::Height = 14;
        Icon::Data = &data[0];
    }
} ApertusRingLogo;

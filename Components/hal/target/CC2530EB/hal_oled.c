#include "hal_types.h"
#include "hal_oled.h"
#include "OSAL.h"
#include "OnBoard.h"
#include "hal_assert.h"
#include "hal_i2c.h"
#include "u8x8.h"
#include "u8g2.h"
#include "u8x8cb.h"

#if defined (ZTOOL_P1) || defined (ZTOOL_P2)
  #include "DebugTrace.h"
#endif
#if (HAL_OLED == TRUE)
  u8g2_t u8g2;
#endif

void HalOledInit(void)
{
  #if (HAL_OLED == TRUE)
    u8g2_Setup_ssd1306_i2c_128x64_noname_1(&u8g2, U8G2_R0, u8x8_byte_UltraIto_sw_i2c, u8x8_gpio_and_delay_Ultriot);
    u8g2_InitDisplay(&u8g2);
    u8g2_SetPowerSave(&u8g2, 0);
    u8g2_SetFont(&u8g2, u8g2_font_6x12_tr);

    u8g2_FirstPage(&u8g2);
    do
    {
      u8g2_DrawStr(&u8g2, 0,15,"UltraIot");
      u8g2_SendBuffer(&u8g2);
    } while( u8g2_NextPage(&u8g2) );
  #endif
}

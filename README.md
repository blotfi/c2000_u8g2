# c2000_u8g2
u8g2 adaptation for c2000 Texas Instruments DSC

this a mix using I2C hardware interface mentionned in 
https://github.com/blotfi/c2000_OLED_SSD1306

with the library u8g2 very popular for Arduino
https://github.com/olikraus/u8g2

Comments for adaptation on:
https://github.com/olikraus/u8g2/issues/549#issuecomment-385145095


Does not compile yet, neither for 27F nor for 69M, it requires a "link garbage collector"
but I didn't found it in CCS 7 as explained on:
https://e2e.ti.com/support/development_tools/code_composer_studio/f/81/p/685102/2525556#2525556

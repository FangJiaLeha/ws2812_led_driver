@echo off
D:\Keil_v5\ARM\ARMCC\bin\fromelf.exe --bin --output "Bin/gd32f330_ws2812_driver_app_1_xx_xx.bin" "Objects/gd32f330_ws2812_driver_app.axf"
CalculateFirmwareCRC 16 3976 "Bin/gd32f330_ws2812_driver_app_1_xx_xx.bin"
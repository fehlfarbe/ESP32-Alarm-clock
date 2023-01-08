EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "ESP32-Alarm-clock"
Date "2020-11-21"
Rev "v0.2"
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 "https://github.com/fehlfarbe/ESP32-Alarm-clock"
$EndDescr
$Comp
L mp3alarm-rescue:RDA-RDA5807M U3
U 1 1 5FB9BAAC
P 7800 3100
F 0 "U3" H 7800 3200 50  0000 L BNN
F 1 "RDA5807" H 7800 3100 50  0000 L BNN
F 2 "pcb:RDA5807" H 7800 3100 50  0000 L BNN
F 3 "" H 7800 3100 50  0001 L BNN
	1    7800 3100
	1    0    0    -1  
$EndComp
$Comp
L ESP32_mini:mini_esp32 U2
U 1 1 5FB9C3D2
P 5500 2600
F 0 "U2" H 5525 2725 50  0000 C CNN
F 1 "mini_esp32" H 5525 2634 50  0000 C CNN
F 2 "pcb:ESP32_mini" H 5650 2700 50  0001 C CNN
F 3 "" H 5650 2700 50  0001 C CNN
	1    5500 2600
	1    0    0    -1  
$EndComp
$Comp
L 105162-0001:105162-0001 J7
U 1 1 5FB9E975
P 7600 4950
F 0 "J7" H 8250 5217 50  0000 C CNN
F 1 "microSD" H 8250 5126 50  0000 C CNN
F 2 "pcb:105162-0001" H 7600 4950 50  0001 L BNN
F 3 "" H 7600 4950 50  0001 L BNN
F 4 "Molex" H 7600 4950 50  0001 L BNN "MANUFACTURER_NAME"
F 5 "https://www.mouser.co.uk/ProductDetail/Molex/105162-0001?qs=1fNsfHe5VsK8daqlgKxZfQ%3D%3D" H 7600 4950 50  0001 L BNN "MOUSER_PRICE-STOCK"
F 6 "Memory Card Connectors 1.45H MICRO SD HEADER WITH D/C PIN" H 7600 4950 50  0001 L BNN "DESCRIPTION"
F 7 "538-105162-0001" H 7600 4950 50  0001 L BNN "MOUSER_PART_NUMBER"
F 8 "mm" H 7600 4950 50  0001 L BNN "HEIGHT"
F 9 "105162-0001" H 7600 4950 50  0001 L BNN "MANUFACTURER_PART_NUMBER"
	1    7600 4950
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x03_Male J1
U 1 1 5FBA03B7
P 1950 6050
F 0 "J1" H 2800 5800 50  0000 C CNN
F 1 "AUDIO" H 3050 5800 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 1950 6050 50  0001 C CNN
F 3 "~" H 1950 6050 50  0001 C CNN
	1    1950 6050
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x04_Male J6
U 1 1 5FBA110B
P 6200 1450
F 0 "J6" H 6308 1731 50  0000 C CNN
F 1 "TM1637" H 6308 1640 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x04_P2.54mm_Vertical" H 6200 1450 50  0001 C CNN
F 3 "~" H 6200 1450 50  0001 C CNN
	1    6200 1450
	0    1    1    0   
$EndComp
$Comp
L Connector:Conn_01x02_Male J2
U 1 1 5FBA2322
P 2800 1700
F 0 "J2" H 2908 1881 50  0000 C CNN
F 1 "SW0" H 2908 1790 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 2800 1700 50  0001 C CNN
F 3 "~" H 2800 1700 50  0001 C CNN
	1    2800 1700
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Male J3
U 1 1 5FBA3008
P 2800 2100
F 0 "J3" H 2908 2281 50  0000 C CNN
F 1 "SW1" H 2908 2190 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 2800 2100 50  0001 C CNN
F 3 "~" H 2800 2100 50  0001 C CNN
	1    2800 2100
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Male J4
U 1 1 5FBA344E
P 2800 2550
F 0 "J4" H 2908 2731 50  0000 C CNN
F 1 "SW2" H 2908 2640 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 2800 2550 50  0001 C CNN
F 3 "~" H 2800 2550 50  0001 C CNN
	1    2800 2550
	1    0    0    -1  
$EndComp
$Comp
L UDA1334A:UDA1334A_Breakout U1
U 1 1 5FB9B024
P 3000 5750
F 0 "U1" H 3000 6415 50  0000 C CNN
F 1 "UDA1334A_Breakout" H 3000 6324 50  0000 C CNN
F 2 "UDA1334A:UDA1334A-breakout" H 2700 6250 50  0001 C CNN
F 3 "" H 2700 6250 50  0001 C CNN
	1    3000 5750
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x04_Male J5
U 1 1 5FB9B96D
P 5300 1450
F 0 "J5" H 5408 1731 50  0000 C CNN
F 1 "I2C" H 5408 1640 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x04_P2.54mm_Vertical" H 5300 1450 50  0001 C CNN
F 3 "~" H 5300 1450 50  0001 C CNN
	1    5300 1450
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR017
U 1 1 5FBD45AA
P 9150 5600
F 0 "#PWR017" H 9150 5350 50  0001 C CNN
F 1 "GND" H 9155 5427 50  0000 C CNN
F 2 "" H 9150 5600 50  0001 C CNN
F 3 "" H 9150 5600 50  0001 C CNN
	1    9150 5600
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR01
U 1 1 5FBD68B8
P 2150 5550
F 0 "#PWR01" H 2150 5300 50  0001 C CNN
F 1 "GND" V 2155 5422 50  0000 R CNN
F 2 "" H 2150 5550 50  0001 C CNN
F 3 "" H 2150 5550 50  0001 C CNN
	1    2150 5550
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR010
U 1 1 5FBD6D94
P 6450 2900
F 0 "#PWR010" H 6450 2650 50  0001 C CNN
F 1 "GND" V 6455 2772 50  0000 R CNN
F 2 "" H 6450 2900 50  0001 C CNN
F 3 "" H 6450 2900 50  0001 C CNN
	1    6450 2900
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR011
U 1 1 5FBE0CF7
P 6450 3000
F 0 "#PWR011" H 6450 2750 50  0001 C CNN
F 1 "GND" V 6455 2872 50  0000 R CNN
F 2 "" H 6450 3000 50  0001 C CNN
F 3 "" H 6450 3000 50  0001 C CNN
	1    6450 3000
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR012
U 1 1 5FBE113A
P 6450 3100
F 0 "#PWR012" H 6450 2850 50  0001 C CNN
F 1 "GND" V 6455 2972 50  0000 R CNN
F 2 "" H 6450 3100 50  0001 C CNN
F 3 "" H 6450 3100 50  0001 C CNN
	1    6450 3100
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR016
U 1 1 5FBE1516
P 8850 3650
F 0 "#PWR016" H 8850 3400 50  0001 C CNN
F 1 "GND" V 8855 3522 50  0000 R CNN
F 2 "" H 8850 3650 50  0001 C CNN
F 3 "" H 8850 3650 50  0001 C CNN
	1    8850 3650
	0    -1   -1   0   
$EndComp
$Comp
L power:VCC #PWR09
U 1 1 5FBE1D30
P 6450 2800
F 0 "#PWR09" H 6450 2650 50  0001 C CNN
F 1 "VCC" V 6465 2928 50  0000 L CNN
F 2 "" H 6450 2800 50  0001 C CNN
F 3 "" H 6450 2800 50  0001 C CNN
	1    6450 2800
	0    1    1    0   
$EndComp
$Comp
L power:VCC #PWR013
U 1 1 5FBE4D57
P 7400 5250
F 0 "#PWR013" H 7400 5100 50  0001 C CNN
F 1 "VCC" V 7415 5377 50  0000 L CNN
F 2 "" H 7400 5250 50  0001 C CNN
F 3 "" H 7400 5250 50  0001 C CNN
	1    7400 5250
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR014
U 1 1 5FBE530E
P 7400 5450
F 0 "#PWR014" H 7400 5200 50  0001 C CNN
F 1 "GND" V 7405 5322 50  0000 R CNN
F 2 "" H 7400 5450 50  0001 C CNN
F 3 "" H 7400 5450 50  0001 C CNN
	1    7400 5450
	0    1    1    0   
$EndComp
$Comp
L power:VCC #PWR06
U 1 1 5FBE7E7B
P 5100 1800
F 0 "#PWR06" H 5100 1650 50  0001 C CNN
F 1 "VCC" H 5115 1973 50  0000 C CNN
F 2 "" H 5100 1800 50  0001 C CNN
F 3 "" H 5100 1800 50  0001 C CNN
	1    5100 1800
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR07
U 1 1 5FBE888F
P 5200 1800
F 0 "#PWR07" H 5200 1550 50  0001 C CNN
F 1 "GND" H 5205 1627 50  0000 C CNN
F 2 "" H 5200 1800 50  0001 C CNN
F 3 "" H 5200 1800 50  0001 C CNN
	1    5200 1800
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR03
U 1 1 5FBE90DC
P 3200 1700
F 0 "#PWR03" H 3200 1450 50  0001 C CNN
F 1 "GND" V 3205 1572 50  0000 R CNN
F 2 "" H 3200 1700 50  0001 C CNN
F 3 "" H 3200 1700 50  0001 C CNN
	1    3200 1700
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR04
U 1 1 5FBE97AD
P 3200 2100
F 0 "#PWR04" H 3200 1850 50  0001 C CNN
F 1 "GND" V 3205 1972 50  0000 R CNN
F 2 "" H 3200 2100 50  0001 C CNN
F 3 "" H 3200 2100 50  0001 C CNN
	1    3200 2100
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR05
U 1 1 5FBE9C52
P 3200 2550
F 0 "#PWR05" H 3200 2300 50  0001 C CNN
F 1 "GND" V 3205 2422 50  0000 R CNN
F 2 "" H 3200 2550 50  0001 C CNN
F 3 "" H 3200 2550 50  0001 C CNN
	1    3200 2550
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR08
U 1 1 5FBEA0FB
P 6100 1800
F 0 "#PWR08" H 6100 1550 50  0001 C CNN
F 1 "GND" H 6105 1627 50  0000 C CNN
F 2 "" H 6100 1800 50  0001 C CNN
F 3 "" H 6100 1800 50  0001 C CNN
	1    6100 1800
	1    0    0    -1  
$EndComp
Wire Wire Line
	6450 3100 6050 3100
Wire Wire Line
	6450 3000 6050 3000
Wire Wire Line
	6050 2900 6450 2900
Wire Wire Line
	6450 2800 6050 2800
Wire Wire Line
	5100 1800 5100 1650
Wire Wire Line
	5200 1800 5200 1650
Wire Wire Line
	6100 1800 6100 1650
Wire Wire Line
	3000 1700 3200 1700
Wire Wire Line
	3000 2100 3200 2100
Wire Wire Line
	3000 2550 3200 2550
Wire Wire Line
	2150 5550 2550 5550
Wire Wire Line
	2150 5950 2550 5950
Wire Wire Line
	2150 6050 2350 6050
Wire Wire Line
	9150 5600 9150 5450
Wire Wire Line
	9150 5450 8900 5450
Wire Wire Line
	8900 5350 8900 5450
Connection ~ 8900 5450
Wire Wire Line
	8900 5350 8900 5250
Connection ~ 8900 5350
Wire Wire Line
	8900 5250 8900 5150
Connection ~ 8900 5250
Wire Wire Line
	7400 5250 7600 5250
Wire Wire Line
	7400 5450 7600 5450
Text GLabel 7500 3200 0    50   Input ~ 0
SDA
Wire Wire Line
	7500 3200 7650 3200
Text GLabel 7500 3300 0    50   Input ~ 0
SCL
Text GLabel 8850 3250 2    50   Input ~ 0
ANT
Text GLabel 8850 3450 2    50   Input ~ 0
RDA_L
Text GLabel 8850 3550 2    50   Input ~ 0
RDA_R
Wire Wire Line
	7500 3300 7650 3300
Wire Wire Line
	8600 3250 8850 3250
Wire Wire Line
	8600 3450 8850 3450
Wire Wire Line
	8600 3550 8850 3550
Text GLabel 4750 4600 0    50   Input ~ 0
SDA
Text GLabel 4750 4700 0    50   Input ~ 0
SCL
Wire Wire Line
	4750 4700 5000 4700
Wire Wire Line
	5000 4600 4750 4600
Text GLabel 6500 2700 2    50   Input ~ 0
5V
Wire Wire Line
	6500 2700 6050 2700
Text GLabel 2150 5350 0    50   Input ~ 0
5V
Wire Wire Line
	2150 5350 2550 5350
Text GLabel 6000 1850 3    50   Input ~ 0
5V
Wire Wire Line
	6000 1850 6000 1650
Text GLabel 5300 1800 3    50   Input ~ 0
SDA
Text GLabel 5400 1800 3    50   Input ~ 0
SCL
Wire Wire Line
	5300 1800 5300 1650
Wire Wire Line
	5400 1650 5400 1800
Text GLabel 7350 5350 0    50   Input ~ 0
SD_CLK
Text GLabel 7350 5150 0    50   Input ~ 0
SD_CMD
Text GLabel 7350 5050 0    50   Input ~ 0
SD_CD
Text GLabel 7350 5550 0    50   Input ~ 0
SD_DATA0
Wire Wire Line
	7350 5050 7600 5050
Wire Wire Line
	7350 5150 7600 5150
Wire Wire Line
	7350 5350 7600 5350
Wire Wire Line
	7350 5550 7600 5550
Text GLabel 4750 3800 0    50   Input ~ 0
SD_CLK
Wire Wire Line
	4750 3800 5000 3800
Text GLabel 4750 3000 0    50   Input ~ 0
SD_CD
Wire Wire Line
	4750 3000 5000 3000
Text GLabel 4750 3900 0    50   Input ~ 0
SD_DATA0
Wire Wire Line
	4750 3900 5000 3900
Text GLabel 4750 4800 0    50   Input ~ 0
SD_CMD
Wire Wire Line
	4750 4800 5000 4800
$Comp
L Connector:Conn_01x01_Male J9
U 1 1 5FC132E3
P 9050 1950
F 0 "J9" H 9158 2131 50  0000 C CNN
F 1 "ANT" H 9158 2040 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x01_P2.54mm_Vertical" H 9050 1950 50  0001 C CNN
F 3 "~" H 9050 1950 50  0001 C CNN
	1    9050 1950
	1    0    0    -1  
$EndComp
Text GLabel 9400 1950 2    50   Input ~ 0
ANT
Wire Wire Line
	9400 1950 9250 1950
Text GLabel 6200 1850 3    50   Input ~ 0
TM_DIO
Text GLabel 6300 1850 3    50   Input ~ 0
TM_CLK
Wire Wire Line
	6300 1850 6300 1650
Wire Wire Line
	6200 1650 6200 1850
Text GLabel 4750 3700 0    50   Input ~ 0
TM_DIO
Text GLabel 4750 3600 0    50   Input ~ 0
TM_CLK
Wire Wire Line
	4750 3600 5000 3600
Wire Wire Line
	5000 3700 4750 3700
Text GLabel 2150 5650 0    50   Input ~ 0
I2S_LRC
Wire Wire Line
	2150 5650 2550 5650
Text GLabel 2150 5750 0    50   Input ~ 0
I2S_DIN
Text GLabel 2150 5850 0    50   Input ~ 0
I2S_BCLK
Wire Wire Line
	2150 5750 2550 5750
Wire Wire Line
	2150 5850 2550 5850
Text GLabel 3650 5550 2    50   Input ~ 0
I2S_MUTE
Wire Wire Line
	3650 5550 3450 5550
Text GLabel 4750 4400 0    50   Input ~ 0
RDA_L
Text GLabel 4750 4100 0    50   Input ~ 0
RDA_R
Wire Wire Line
	4750 4100 5000 4100
Text GLabel 3250 1800 2    50   Input ~ 0
SW0
Text GLabel 3250 2200 2    50   Input ~ 0
SW1
Text GLabel 3250 2650 2    50   Input ~ 0
SW2
Wire Wire Line
	3250 2650 3000 2650
Wire Wire Line
	3000 2200 3250 2200
Wire Wire Line
	3000 1800 3250 1800
$Comp
L Device:R R1
U 1 1 5FC28A60
P 7900 1500
F 0 "R1" V 8000 1650 50  0000 C CNN
F 1 "500" V 7900 1500 50  0000 C CNN
F 2 "Resistor_SMD:R_1206_3216Metric" V 7830 1500 50  0001 C CNN
F 3 "~" H 7900 1500 50  0001 C CNN
	1    7900 1500
	0    -1   -1   0   
$EndComp
$Comp
L power:VCC #PWR019
U 1 1 5FC2AD70
P 8550 1000
F 0 "#PWR019" H 8550 850 50  0001 C CNN
F 1 "VCC" V 8565 1128 50  0000 L CNN
F 2 "" H 8550 1000 50  0001 C CNN
F 3 "" H 8550 1000 50  0001 C CNN
	1    8550 1000
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR018
U 1 1 5FC2B300
P 8550 1950
F 0 "#PWR018" H 8550 1700 50  0001 C CNN
F 1 "GND" V 8555 1822 50  0000 R CNN
F 2 "" H 8550 1950 50  0001 C CNN
F 3 "" H 8550 1950 50  0001 C CNN
	1    8550 1950
	1    0    0    -1  
$EndComp
Text GLabel 7450 1500 0    50   Input ~ 0
STATUS_DATA
$Comp
L power:VCC #PWR015
U 1 1 5FBE298E
P 7500 3600
F 0 "#PWR015" H 7500 3450 50  0001 C CNN
F 1 "VCC" V 7515 3727 50  0000 L CNN
F 2 "" H 7500 3600 50  0001 C CNN
F 3 "" H 7500 3600 50  0001 C CNN
	1    7500 3600
	0    -1   -1   0   
$EndComp
Wire Wire Line
	7750 1500 7450 1500
Text GLabel 4750 4900 0    50   Input ~ 0
I2S_DIN
Text GLabel 4750 2900 0    50   Input ~ 0
I2S_BCLK
Wire Wire Line
	4750 2900 5000 2900
Text GLabel 4750 5100 0    50   Input ~ 0
I2S_LRC
Wire Wire Line
	4750 5100 5000 5100
Wire Wire Line
	5000 4900 4750 4900
Text GLabel 4750 4200 0    50   Input ~ 0
SW0
Text GLabel 4750 4300 0    50   Input ~ 0
SW1
Text GLabel 4750 4000 0    50   Input ~ 0
I2S_MUTE
Wire Wire Line
	5000 4300 4750 4300
Text GLabel 4750 4500 0    50   Input ~ 0
SW2
Text GLabel 4750 5000 0    50   Input ~ 0
STATUS_DATA
Wire Wire Line
	5000 4500 4750 4500
Wire Wire Line
	5000 4200 4750 4200
$Comp
L power:GND #PWR02
U 1 1 5FBD6393
P 2350 6300
F 0 "#PWR02" H 2350 6050 50  0001 C CNN
F 1 "GND" H 2355 6127 50  0000 C CNN
F 2 "" H 2350 6300 50  0001 C CNN
F 3 "" H 2350 6300 50  0001 C CNN
	1    2350 6300
	1    0    0    -1  
$EndComp
Wire Wire Line
	2150 6150 2550 6150
Wire Wire Line
	2350 6300 2350 6050
Connection ~ 2350 6050
Wire Wire Line
	2350 6050 2550 6050
$Comp
L LED:WS2812B D1
U 1 1 5FBB8694
P 8550 1500
F 0 "D1" H 8894 1546 50  0000 L CNN
F 1 "WS2812B" H 8894 1455 50  0000 L CNN
F 2 "LED_SMD:LED_WS2812B_PLCC4_5.0x5.0mm_P3.2mm" H 8600 1200 50  0001 L TNN
F 3 "https://cdn-shop.adafruit.com/datasheets/WS2812B.pdf" H 8650 1125 50  0001 L TNN
	1    8550 1500
	1    0    0    -1  
$EndComp
Wire Wire Line
	8550 1000 8550 1200
Wire Wire Line
	8550 1950 8550 1800
Wire Wire Line
	8050 1500 8250 1500
$Comp
L pspice:C C1
U 1 1 5FBCC7B2
P 8150 3950
F 0 "C1" V 7835 3950 50  0000 C CNN
F 1 "C" V 7926 3950 50  0000 C CNN
F 2 "Capacitor_SMD:C_1206_3216Metric" H 8150 3950 50  0001 C CNN
F 3 "~" H 8150 3950 50  0001 C CNN
	1    8150 3950
	0    1    1    0   
$EndComp
Wire Wire Line
	7500 3600 7600 3600
Wire Wire Line
	7900 3950 7600 3950
Wire Wire Line
	7600 3950 7600 3600
Connection ~ 7600 3600
Wire Wire Line
	7600 3600 7650 3600
Wire Wire Line
	8400 3950 8700 3950
Wire Wire Line
	8700 3950 8700 3650
Wire Wire Line
	8700 3650 8850 3650
Wire Wire Line
	8600 3650 8700 3650
Connection ~ 8700 3650
Wire Wire Line
	4750 5000 5000 5000
Wire Wire Line
	4750 4400 5000 4400
Wire Wire Line
	4750 4000 5000 4000
$Comp
L Connector:Conn_01x02_Male J8
U 1 1 5FFA7191
P 2800 2900
F 0 "J8" H 2908 3081 50  0000 C CNN
F 1 "5V" H 2908 2990 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 2800 2900 50  0001 C CNN
F 3 "~" H 2800 2900 50  0001 C CNN
	1    2800 2900
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0101
U 1 1 5FFA77AD
P 3200 2900
F 0 "#PWR0101" H 3200 2650 50  0001 C CNN
F 1 "GND" V 3205 2772 50  0000 R CNN
F 2 "" H 3200 2900 50  0001 C CNN
F 3 "" H 3200 2900 50  0001 C CNN
	1    3200 2900
	0    -1   -1   0   
$EndComp
Text GLabel 3200 3000 2    50   Input ~ 0
5V
Wire Wire Line
	3200 3000 3000 3000
Wire Wire Line
	3200 2900 3000 2900
Text GLabel 2500 3850 2    50   Input ~ 0
TMS
Text GLabel 2500 4050 2    50   Input ~ 0
TDO
Text GLabel 2500 4150 2    50   Input ~ 0
TDI
Text GLabel 2500 3950 2    50   Input ~ 0
TCK
$Comp
L Connector_Generic:Conn_02x05_Odd_Even J0
U 1 1 5FFCFAB1
P 2050 4050
F 0 "J0" H 2100 4467 50  0000 C CNN
F 1 "debug" H 2100 4376 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_2x05_P2.54mm_Vertical_SMD" H 2050 4050 50  0001 C CNN
F 3 "~" H 2050 4050 50  0001 C CNN
	1    2050 4050
	1    0    0    -1  
$EndComp
$Comp
L power:VCC #PWR0102
U 1 1 5FFD0F20
P 1650 3850
F 0 "#PWR0102" H 1650 3700 50  0001 C CNN
F 1 "VCC" V 1665 3978 50  0000 L CNN
F 2 "" H 1650 3850 50  0001 C CNN
F 3 "" H 1650 3850 50  0001 C CNN
	1    1650 3850
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR0103
U 1 1 5FFD5D90
P 1650 3950
F 0 "#PWR0103" H 1650 3700 50  0001 C CNN
F 1 "GND" V 1655 3822 50  0000 R CNN
F 2 "" H 1650 3950 50  0001 C CNN
F 3 "" H 1650 3950 50  0001 C CNN
	1    1650 3950
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR0104
U 1 1 5FFD6265
P 1650 4050
F 0 "#PWR0104" H 1650 3800 50  0001 C CNN
F 1 "GND" V 1655 3922 50  0000 R CNN
F 2 "" H 1650 4050 50  0001 C CNN
F 3 "" H 1650 4050 50  0001 C CNN
	1    1650 4050
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR0105
U 1 1 5FFD6381
P 1650 4150
F 0 "#PWR0105" H 1650 3900 50  0001 C CNN
F 1 "GND" V 1655 4022 50  0000 R CNN
F 2 "" H 1650 4150 50  0001 C CNN
F 3 "" H 1650 4150 50  0001 C CNN
	1    1650 4150
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR0106
U 1 1 5FFD658B
P 1650 4250
F 0 "#PWR0106" H 1650 4000 50  0001 C CNN
F 1 "GND" V 1655 4122 50  0000 R CNN
F 2 "" H 1650 4250 50  0001 C CNN
F 3 "" H 1650 4250 50  0001 C CNN
	1    1650 4250
	0    1    1    0   
$EndComp
Wire Wire Line
	2500 3850 2350 3850
Wire Wire Line
	2500 3950 2350 3950
Wire Wire Line
	2350 4050 2500 4050
Wire Wire Line
	2500 4150 2350 4150
Wire Wire Line
	1850 4250 1650 4250
Wire Wire Line
	1650 4150 1850 4150
Wire Wire Line
	1850 4050 1650 4050
Wire Wire Line
	1650 3950 1850 3950
Wire Wire Line
	1850 3850 1650 3850
Text GLabel 4750 3500 0    50   Input ~ 0
TMS
Text GLabel 4750 3400 0    50   Input ~ 0
TCK
Text GLabel 6200 4300 2    50   Input ~ 0
TDO
Text GLabel 4750 3300 0    50   Input ~ 0
TDI
Wire Wire Line
	4750 3300 5000 3300
Wire Wire Line
	4750 3400 5000 3400
Wire Wire Line
	4750 3500 5000 3500
Wire Wire Line
	6200 4300 6050 4300
$EndSCHEMATC
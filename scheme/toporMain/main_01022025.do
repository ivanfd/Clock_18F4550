# DipTrace Auto-Generated DO File
bestsave off
grid via 0.0039
grid wire 0.0039
define (class Dip_Net_Class_0 DQ DS_CLK DS_CSN DS_DAT DS_INT i2c_scl_3_3v i2c_sda_3_3v ISP_CLK_BUT_OK ISP_DAT_BUT_UP mclr NRF_CE NRF_CSN NRF_MISO NRF_MOSI NRF_SCK Power_On_8266 RX_ESP8266 RX_ESP8266_LO SCL SDA TX_ESP8266 TX_ESP8266_LO USB_D+ USB_D- Vusb Мережа@37 Сеть@4 Сеть@8 Сеть@9 Сеть@10 Сеть@23 Сеть@24 Сеть@34 Сеть@35 Сеть@36 Сеть@37)
circuit class Dip_Net_Class_0 (use_via DipViaStyle_0)
rule class Dip_Net_Class_0 (width 15.748)
rule class Dip_Net_Class_0 (clearance 19.685)
define (class Dip_Net_Class_1 +5V power_8266)
circuit class Dip_Net_Class_1 (use_via DipViaStyle_0)
rule class Dip_Net_Class_1 (width 19.685)
rule class Dip_Net_Class_1 (clearance 19.685)
define (class Dip_Net_Class_2 GND)
circuit class Dip_Net_Class_2 (use_via DipViaStyle_0)
rule class Dip_Net_Class_2 (width 19.685)
rule class Dip_Net_Class_2 (clearance 19.685)
define (class Dip_Net_Class_3 3.3V 3.3V_NODEMCU)
circuit class Dip_Net_Class_3 (use_via DipViaStyle_0)
rule class Dip_Net_Class_3 (width 19.685)
rule class Dip_Net_Class_3 (clearance 19.685)
rule pcb (pin_width_taper down)
bus diagonal
route 20
clean 2
route 25 16
clean 2
filter 5
recorner diagonal

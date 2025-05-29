# DipTrace Auto-Generated DO File
bestsave off
grid via 0.0039
grid wire 0.0039
define (class Dip_Net_Class_0 AM_SDA DQ DS_CLK DS_CSN DS_DAT DS_INT HC-05_AT ISP_CLK ISP_DAT mclr NRF_CE NRF_CSN NRF_MISO NRF_MOSI NRF_SCK power_in RX_HC-05 SCL SDA TX_HC-05 USB_D+ USB_D- Vusb Сеть@4 Сеть@8 Сеть@9 Сеть@10 Сеть@12 Сеть@13 Сеть@20 Сеть@21 Сеть@22 Сеть@23 Сеть@24 Сеть@34 Сеть@35 Сеть@36 Сеть@37 Сеть@38 Сеть@39)
circuit class Dip_Net_Class_0 (use_via DipViaStyle_0)
rule class Dip_Net_Class_0 (width 15.748)
rule class Dip_Net_Class_0 (clearance 19.685)
define (class Dip_Net_Class_1 5V)
circuit class Dip_Net_Class_1 (use_via DipViaStyle_0)
rule class Dip_Net_Class_1 (width 19.685)
rule class Dip_Net_Class_1 (clearance 19.685)
define (class Dip_Net_Class_2 GND)
circuit class Dip_Net_Class_2 (use_via DipViaStyle_0)
rule class Dip_Net_Class_2 (width 19.685)
rule class Dip_Net_Class_2 (clearance 19.685)
define (class Dip_Net_Class_3 +3.3V)
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

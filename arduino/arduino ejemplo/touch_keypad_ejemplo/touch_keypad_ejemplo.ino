
/******************************************************************
** Código creado en Electrónica 5Hz                              **
** www.5hz-electronica.com                                       **
** Por: Rafael Almaguer                                          **
** Basado en el código de:                                       **
** Descripción del código:                                       **
**                                                               **
** Ejemplo para el touch Keypad y el touch shield                **
** indica por medio del puerto serial                            **
** que teclas estan siendo presionadas                           **
*******************************************************************

Conexiones para la comunicacion i2c usando el breakout de 5Hz con un Arduino Uno / Duemilanove:
Arduino <-> Breakout board
Gnd      -  GND
3.3v     -  VCC
3.3v     -  CS
Analog 4 -  SDA
Analog 5 -  SCL
12       -  IRQ

Conexiones para la comunicacion i2c usando el breakout de 5Hz con un Arduino Mega / Mega ADK:
Arduino <-> Breakout board
Gnd      -  GND
3.3v     -  VCC
3.3v     -  CS
20       -  SDA
21       -  SCL
12       -  IRQ
*/



#include <Wire.h>


//definiciones///////////////////////////////////////////////////
#define MPR_121 0x5A  //direccion del sensor (su nombre)

#define MHD_R	0x2B
#define NHD_R	0x2C
#define	NCL_R 	0x2D
#define	FDL_R	0x2E
#define	MHD_F	0x2F
#define	NHD_F	0x30
#define	NCL_F	0x31
#define	FDL_F	0x32
#define	ELE0_T	0x41
#define	ELE0_R	0x42
#define	ELE1_T	0x43
#define	ELE1_R	0x44
#define	ELE2_T	0x45
#define	ELE2_R	0x46
#define	ELE3_T	0x47
#define	ELE3_R	0x48
#define	ELE4_T	0x49
#define	ELE4_R	0x4A
#define	ELE5_T	0x4B
#define	ELE5_R	0x4C
#define	ELE6_T	0x4D
#define	ELE6_R	0x4E
#define	ELE7_T	0x4F
#define	ELE7_R	0x50
#define	ELE8_T	0x51
#define	ELE8_R	0x52
#define	ELE9_T	0x53
#define	ELE9_R	0x54
#define	ELE10_T	0x55
#define	ELE10_R	0x56
#define	ELE11_T	0x57
#define	ELE11_R	0x58
#define	FIL_CFG	0x5D
#define	ELE_CFG	0x5E
#define GPIO_CTRL0	0x73
#define	GPIO_CTRL1	0x74
#define GPIO_DATA	0x75
#define	GPIO_DIR	0x76
#define	GPIO_EN		0x77
#define	GPIO_SET	0x78
#define	GPIO_CLEAR	0x79
#define	GPIO_TOGGLE	0x7A
#define	ATO_CFG0	0x7B
#define	ATO_CFGU	0x7D
#define	ATO_CFGL	0x7E
#define	ATO_CFGT	0x7F

#define TOU_THRESH	0x06
#define	REL_THRESH	0x0A
#define DIGITS  11

//prototipos de funciones

//variables globales
int irqpin = 12;  // Digital 12
boolean touchStates[12]; //para hacer un seguimiento de los estados táctiles anteriores
char teclas[13] =
{
  '3','0','8','5','2','*','7','4','1','#','9','6'
};

//setup
void setup(){
  pinMode(irqpin, INPUT);
  digitalWrite(irqpin, HIGH); //habilita resistencia pullup 
  Serial.begin(9600);
  Wire.begin();
  mpr121_setup();
  Serial.println("***Electronica 5Hz***");
  Serial.println("programa de ejemplo para teclado touch");
  Serial.println("Presionado = o, no presionado = .");
  Serial.println("0  1  2  3  4  5  6  7  8  9  *  #  ");
}

//programa principal
void loop(){
  readTouchInputs();
    if(touchStates[1] == 1) Serial.print("o  ");  // tecla 0
    else Serial.print(".  ");
    if(touchStates[8] == 1) Serial.print("o  ");  // tecla 1
    else Serial.print(".  ");
    if(touchStates[4] == 1) Serial.print("o  ");  // tecla 2
    else Serial.print(".  ");
    if(touchStates[0] == 1) Serial.print("o  ");  // tecla 3
    else Serial.print(".  ");
    if(touchStates[7] == 1) Serial.print("o  ");  // tecla 4
    else Serial.print(".  ");
    if(touchStates[3] == 1) Serial.print("o  ");  // tecla 5
    else Serial.print(".  ");
    if(touchStates[11] == 1) Serial.print("o  ");  // tecla 6
    else Serial.print(".  ");
    if(touchStates[6] == 1) Serial.print("o  ");  // tecla 7
    else Serial.print(".  ");
    if(touchStates[2] == 1) Serial.print("o  ");  // tecla 8
    else Serial.print(".  ");
    if(touchStates[10] == 1) Serial.print("o  ");  // tecla 9
    else Serial.print(".  ");
    if(touchStates[5] == 1) Serial.print("o  ");  // tecla *
    else Serial.print(".  ");
    if(touchStates[9] == 1) Serial.print("o  ");  // tecla #
    else Serial.print(".  ");
  Serial.println("");
  delay(1000);
}

void readTouchInputs(){
  char digito[DIGITS];
  if(!checkInterrupt()){
    //lee el estado del MPR121
    Wire.requestFrom(MPR_121, 2); 
    int touched = concatenate(Wire.read(), Wire.read()); //forma un entero (16bits) de 2 bytes (MSB y LSB) 
    for (int i=0; i < 12; i++)
    {  // Checa que electrodos fueron presionados 
      if(bitRead(touched, i)) touchStates[i] = 1;
      else touchStates[i] = 0;
    }
  }
}

void mpr121_setup(void)
{
  set_register(MPR_121, ELE_CFG, 0x00); 
  // Seccion A - Control de filtrado cuando los datos son mayores que la línea de base.
  set_register(MPR_121, MHD_R, 0x01);
  set_register(MPR_121, NHD_R, 0x01);
  set_register(MPR_121, NCL_R, 0x00);
  set_register(MPR_121, FDL_R, 0x00);
  // Seccion B - Control de filtrado cuando los datos son menores que la linea de base. 
  set_register(MPR_121, MHD_F, 0x01);
  set_register(MPR_121, NHD_F, 0x01);
  set_register(MPR_121, NCL_F, 0xFF);
  set_register(MPR_121, FDL_F, 0x02);
  // Seccion C - Ajusta contacto y  umbrales de liberación para cada electrodo 
  for(byte reg = ELE0_T; reg <= ELE11_R; reg++)
  {
    if(reg%2 != 0) set_register(MPR_121, reg, TOU_THRESH);
    else set_register(MPR_121, reg, REL_THRESH);
  }
  // Seccion D
  // Ajusta la Configuración del Filtro 
  // Ajusta ESI2
  set_register(MPR_121, FIL_CFG, 0x04);
  // Seccion E
  // Configuración de los electrodos
  // Ajusta ELE_CFG a 0x00 para regresar al modo de espera 
  set_register(MPR_121, ELE_CFG, 0x0C);  // Habilita los 12 Electrodos
  // Seccion F
  // Habilita Auto Config y auto Reconfig
  set_register(MPR_121, ELE_CFG, 0x0C);
}

boolean checkInterrupt(void)
{
  return digitalRead(irqpin);
}

void set_register(int address, unsigned char r, unsigned char v)
{
    Wire.beginTransmission(address);
    Wire.write(r);
    Wire.write(v);
    Wire.endTransmission();
}

int concatenate(byte _lowByte_, byte _highByte_)
{
  int _integer_ = (_highByte_ <<8) | _lowByte_; //crea un entero a partir de sus 2 bytes (MSB y LSB)
  return _integer_;
}

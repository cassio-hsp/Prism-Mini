//Prism Mini Basic - Use shake detection to alternates between three color effects (all_red, all_green, all_blue) 

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <SPI.h>

#define PIN       25
#define NUMPIXELS 24 
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
#define RANDOMDELAY 50

//-----------------------------SPI LIS3DH -----------------------------------------
#define VSPI_MISO   MISO
#define VSPI_MOSI   MOSI
#define VSPI_SCLK   SCK
#define VSPI_SS     SS

static const int spiClk = 1000000; // 1 MHz

const uint8_t CTRL_REG1 = 0x20;     
const uint8_t CTRL_REG2 = 0x21;     
const uint8_t CTRL_REG3 = 0x22; 
const uint8_t CTRL_REG4 = 0x23;    
const uint8_t CTRL_REG5 = 0x24;
const uint8_t CTRL_REG6 = 0x25;
const uint8_t FIFO_CTRL = 0x2E;

const uint8_t AcX_L = 0x28;  
const uint8_t AcX_H = 0x29;
const uint8_t AcY_L = 0x2A;  
const uint8_t AcY_H = 0x2B;


uint8_t x_buffer[2];
uint8_t y_buffer[2];
int16_t AcX;                 //IMPORTANTE: int16_t pois o tamanho do registro excede em valor o tamanho de um INT.
int16_t AcY;                                         
int effect = 0;    

uint8_t spi_receive(byte address) 
{  
  uint8_t retnd_result;
  SPI.beginTransaction(SPISettings(spiClk, MSBFIRST, SPI_MODE0));
  digitalWrite(VSPI_SS, LOW);
  SPI.transfer(address | 0x80);
  retnd_result = SPI.transfer(0x00);
  digitalWrite(VSPI_SS, HIGH);
  SPI.endTransaction();
  return retnd_result;
}

void spi_send(byte address, uint8_t data_to_transfer) 
{  
  SPI.beginTransaction(SPISettings(spiClk, MSBFIRST, SPI_MODE0));
  digitalWrite(VSPI_SS, LOW);
  SPI.transfer(address);
  SPI.transfer(data_to_transfer);
  digitalWrite(VSPI_SS, HIGH);
  SPI.endTransaction();
}
//--------------------------------------------------------------------------
void inicia_imu()
{ 
  Serial.println("IMU init"); 
  spi_send(CTRL_REG1, 0b10010111);
  spi_send(CTRL_REG2, 0b00000000);
  spi_send(CTRL_REG3, 0b00000000);
  spi_send(CTRL_REG4, 0b00000000);         //g select 16g         
  spi_send(CTRL_REG5, 0b00000000);
}

void all_red()
{
  for(int p=0;p<=23;p++)
  {
    pixels.setPixelColor(p, pixels.Color(200, 0, 0));   //faz tudo verm
    pixels.show();   
  }       
}

void all_green()
{
  for(int p=0;p<=23;p++)
  {
    pixels.setPixelColor(p, pixels.Color(0, 200, 0));   //faz tudo verde
    pixels.show();   
  }       
}

void all_blue()
{
  for(int p=0;p<=23;p++)
  {
    pixels.setPixelColor(p, pixels.Color(0, 0, 200));   //faz tudo azul
    pixels.show();   
  }       
}

void acel()
{
  x_buffer[0] = spi_receive(AcX_L);
  x_buffer[1] = spi_receive(AcX_H);
  AcX = (int16_t)x_buffer[0] | int16_t(x_buffer[1] << 8);
}

void setup()
{
  Serial.begin(9600);
  pinMode(VSPI_SS, OUTPUT);                               //VSPI SS - Chip Select/Enable
  SPI.begin(VSPI_SCLK, VSPI_MISO, VSPI_MOSI, VSPI_SS);
  delay(100);  
  //inicia_imu();
  delay(500);
  pixels.clear();
  pixels.show();    
}

void loop()
{
  acel();

  if(AcX <= -25000 || AcX >= 25000)
  {
    effect += 1;
    Serial.println(effect); 
    delay(300);
  }
  
  if(effect >= 4)
  {
    effect = 0;
    pixels.clear();
    pixels.show();
  }
 
  if(effect == 1){
    all_red();
  } 
    
  if(effect == 2){
    all_green(); 
  }
    
  if(effect == 3){
    all_blue();
  } 

  delay(100); 
}        



/*
SLT5006(UART) -->ARDUINO
Red&white -->5V
Black--> GND
Blue -->D2
Yellow-->D3
*/

/*
LCD 16X2 -->ARDUINO
VSS --> GND
VDD -->5V
V0 -->GND
RS--> D4
RW -->GND
E -->D6
DB4-->D10
DB5-->D11
DB6-->D12
DB7-->D13
*/

#include <SoftwareSerial.h>
SoftwareSerial mySerial(2,3);
#include <LiquidCrystal.h>
LiquidCrystal lcd( 4, 6, 10, 11, 12, 13 );

char flag;
char size;//valnw

void writecom(int size, int value[]){
for (int i=0;i<size;i++){
mySerial.write(value[i]);
 Serial.print(value[i],HEX);
}
 Serial.println("write");
}

void recieve(int valn){
int val[valn]={0};
if(mySerial.available()>0){
for (int i=0; i<valn;i++){
 val[i]=mySerial.read();
 Serial.print(val[i],HEX);
}
 Serial.println("read");
}
while (mySerial.available()>0){
 char t = mySerial.read();
}

if((val[1]==8)&&(val[3]==1)){
 flag=1;
 }
 int tempint;
 float temp;
 float bulk;
 float VWC;
 float pore;

if(val[1]==0X13){
 temp=(val[3]+val[4]*256)*0.0625;
 bulk=(val[5]+val[6]*256)*0.001;
 VWC=(val[9]+val[10]*256)*0.1;
 pore=(val[15]+val[16]*256)*0.001;
 Serial.print(",");Serial.print(temp);
 Serial.print(",");Serial.print(bulk);
 Serial.print(",");Serial.print(VWC);
 Serial.print(",");Serial.print(pore);Serial.println(",");
 lcd.clear();
 lcd.setCursor(0, 0);
 lcd.print(temp);lcd.print("deg");
 lcd.setCursor(8, 0);
 lcd.print(bulk);lcd.print("dS/m");
 lcd.setCursor(0, 1);
 lcd.print(VWC);lcd.print("%");
 lcd.setCursor(8, 1);
 lcd.print(pore);lcd.print("dS/m");
 }

return flag;

}

void setup() {

  // put your setup code here, to run once:

Serial.begin(9600);

mySerial.begin(9600);

Serial.println("Soil Sensor");

Serial.println("--------------------------------------");

Serial.print("Temp [degC],");

Serial.print("EC BULK [dS/m],");

Serial.print("VWC [%],");

Serial.println("EC PORE [dS/m");

 lcd.begin( 16, 2 );

 lcd.clear();

 lcd.setCursor(0, 0);

 lcd.print("Soil Sensor");



}

void loop() {

  // put your main code here, to run repeatedly

size=6;

int value[]={0X02,0X07,0X01,0X01,0X0D,0X70};

writecom(size,value);

recieve(6);

flag=0;

while(flag==0){

 size=5;

 int value1[]={0X01,0X08,0X01,0X00,0XE6,0X00};

 writecom(size,value1);

 delay(100);

 recieve(6);

}

size=5;
int value2[]={0X01,0X13,0X10,0XFC,0X2C,0X00};
writecom(size,value2);
recieve(21);
delay(5000);
}

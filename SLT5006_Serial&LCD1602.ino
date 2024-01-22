/*
SLT5006(UART) -->ARDUINO
Red&white -->5V
Black--> GND
Blue -->D2
Yellow-->D3

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
A-->5V
K-->GND
*/

#include <SoftwareSerial.h> //EspSoftwareSerial必要
SoftwareSerial mySerial(2,3);
#include <LiquidCrystal.h>
LiquidCrystal lcd( 4, 6, 10, 11, 12, 13 );

int flag;
int size;

void writecom(int size, int value[]){//センサ送信
  for (int i=0;i<size;i++){
    mySerial.write(value[i]);
  }

  while (mySerial.available()>0){
    char t = mySerial.read();
  }
/*デバッグ用
  Serial.print("From Arduino:");Serial.print(" ");
  for (int i=0;i<size;i++){
    Serial.print(value[i],HEX);Serial.print(" "); 
  }
  Serial.print(flag);  Serial.println("send");
*/
}


void receive(int valn){//センサ受信
  int val[valn]={0};
  if(mySerial.available()>0){
    for (int i=0; i<valn;i++){
      val[i]=mySerial.read();
    }
  }
  while (mySerial.available()>0){
    char t = mySerial.read();
  }
/*デバッグ用
  Serial.print(flag);Serial.print("fromsensor");
  for (int i=0;i<valn;i++){
    Serial.print(val[i],HEX);;Serial.print(" "); 
  }
  Serial.print(flag);Serial.println("receive");
  */
  if((val[1]==8)&&(val[3]==1)){
    flag=1;
    /*デバッグ用
    Serial.println("read/measured");
    for (int i=0; i<valn;i++){
      Serial.print(val[i],HEX);Serial.print(" ");
     }
  }else{
  Serial.println("readonly");
  */
  }
}

void receive2(int valn){
  valn=21;
  int val[valn]={0};
  if(mySerial.available()>0){
    for (int i=0; i<valn;i++){
      val[i]=mySerial.read();
    }
  }
  while (mySerial.available()>0){
    char t = mySerial.read();
  }
/*デバッグ用
  Serial.print(flag);Serial.print("fromsensor");
  for (int i=0;i<valn;i++){
    Serial.print(val[i],HEX);;Serial.print(" "); 
  }
  Serial.print(flag);Serial.print("receive");
  */
  Serial.println();

  if((val[1]==8)&&(val[3]==1)){
    flag=1;
    /*デバッグ用
    Serial.println("read/measured");
    for (int i=0; i<valn;i++){
      Serial.print(val[i],HEX);Serial.print(" ");
     }
  }else{
      Serial.println("readonly");
      */
  }

float temp=0;
float bulk=0;
float VWC=0;
float VWCR=0;
float VWCC=0;
float pore=0;
temp=(val[3]+val[4]*256)*0.0625;
bulk=(val[5]+val[6]*256)*0.001;
VWC=(val[9]+val[10]*256)*0.1;
VWCR=(val[7]+val[8]*256)*0.1;
VWCC=(val[11]+val[12]*256)*0.1;
pore=(val[15]+val[16]*256)*0.001;
display(flag,temp,bulk,VWC,pore,VWCR,VWCC);

}

void display(int flag2,float temp2,float bulk2,float VWC2,float pore2,float VWCR2,float VWCC2){
  if(flag2==1) {
 //Serial.print(flag);Serial.println("display");//デバッグ用
  Serial.println("=============================");
  Serial.print("Temprature [degC],");
  Serial.print("EC BULK [dS/m],");
  Serial.print("VWC [%],");
  Serial.print("EC PORE [dS/m]"); 
  Serial.print("VWC-ROCK [%],");
  Serial.println("VWC-COCO [%],");
  Serial.print(temp2); Serial.print(",");
  Serial.print(bulk2); Serial.print(","); 
  Serial.print(VWC2); Serial.print(","); 
  Serial.print(pore2); Serial.print(""); 
  Serial.print(VWCR2); Serial.print(","); 
  Serial.print(VWCC2); Serial.println(","); 

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(temp2);lcd.print("deg");
  lcd.setCursor(8, 0);
  lcd.print(bulk2);lcd.print("dS/m");
  lcd.setCursor(0, 1);
  lcd.print(VWC2);lcd.print("%");
  lcd.setCursor(8, 1);
  lcd.print(pore2);lcd.print("dS/m");
 }
}

void program(){
  //測定開始
 // Serial.print("Start sensor");デバッグ用
  int  value[]={0X02,0X07,0X01,0X01,0X0D,0X70};
  writecom(6,value);
  delay(100);
  receive(6);//受信バイト数6

 //測定完了ステータス読み出し
  flag=0;
 while(flag==0){
//  Serial.print("Read status");デバッグ用
  int value1[]={0X01,0X08,0X01,0X00,0XE6,0X00};
  writecom(5,value1);
  delay(100);
  receive(6);//受信バイト数6
  delay(500);//測定完了ステータス確認サイクル
   }

//測定データ読み出し
 int value2[]={0X01,0X13,0X10,0XFC,0X2C,0X00};
  writecom(5,value2);
  delay(100);
  receive2(21);//受信バイト数21
}
void setup() {
  Serial.begin(9600);         // 標準のシリアル通信初期化
  mySerial.begin(9600);        // シリアル通信2初期化 
    //LCD表示
  lcd.begin( 16, 2 );
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Soil Sensor");
  Serial.println("Soil sensor");
  Serial.println("=============================");
  Serial.print("Temprature [degC],");
  Serial.print("EC BULK [dS/m],");
  Serial.print("VWC [%],");
  Serial.print("EC PORE [dS/m]"); 
  Serial.print("VWC-ROCK [%],");
  Serial.println("VWC-COCO [%],");
}


void loop() {
  program();
  int interval=3000;
  delay(interval);//測定間隔(ms)
}

チェックポイント
品番：SLT5006(UART)
接続を確認する
////////////////////////////////////////////////////////////
//(1)センサ接続
//Sensor ->Arduino Pin connection
//RED&White  -->5V
//Black      -->GND
//Blue      TxD -->D2(->RX)
//Yellow    RxD -->D3(<-TX)
//Baud rate =9600
//
#include <SoftwareSerial.h>
SoftwareSerial mySerial(2,3);

int flag=0;
int size=0;

//センサへの送信
void writecom(int size,int value[]){

  for (int i=0; i<size;i++){
  mySerial.write(value[i]);
  }
    while (mySerial.available() > 0) {//受信バッファクリア
     char t = mySerial.read();
  }

  Serial.print("From Arduino:");Serial.print(" ");
  for (int i=0; i<size;i++){
    Serial.print(value[i],HEX);Serial.print(" "); 
  }
  Serial.println("send");


}

// センサからの読み出し
void receive(int valn){

 int val[valn] ={0};

  if(mySerial.available()>0){
  for (int i=0; i<valn;i++){
  val[i]=mySerial.read();
  }
  }

  while (mySerial.available() > 0) {//受信バッファクリア
     char t = mySerial.read();
  }

  for (int i=0; i<valn;i++){
    Serial.print(val[i],HEX);Serial.print(" "); 
  }



//測定判定
if((val[1]==8)&&(val[3]==1)){
 Serial.println("Read/Measured");
 flag=1;//測定は完了した

  for (int i=0; i<valn;i++){
    Serial.print(val[i],HEX);Serial.print(" "); 
  }


} else {
Serial.println("Read");
}
//換算
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
 sendvalue(temp,bulk,VWC,pore,VWCR,VWCC);
 return flag;
}

void sendvalue(float temp2,float bulk2,float VWC2,float pore2,float VWCR2,float VWCC2){
if(flag==1){//測定値取り出すときの処理
///*シリアルモニタ表示
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
//*/

}
}

void program(){
  //測定開始
  Serial.print("Start sensor");
  size=6;
int  value[]={0X02,0X07,0X01,0X01,0X0D,0X70};
  writecom(size,value);
  receive(6);//受信バイト数6

 //測定完了ステータス読み出し
  flag=0;
 while(flag==0){
  Serial.print("Read status");
  size=5;
  int value1[]={0X01,0X08,0X01,0X00,0XE6,0X00};
  writecom(size,value1);
  delay(100);//測定完了ステータス確認サイクル
  receive(6);//受信バイト数6
 }

//測定データ読み出し

 size=5;
 int value2[]={0X01,0X13,0X10,0XFC,0X2C,0X00};
  writecom(size,value2);
  receive(21);//受信バイト数21

}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  mySerial.begin(9600);
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
  delay(3000);//測定間隔
}

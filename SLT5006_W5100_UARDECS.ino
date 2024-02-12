//Sample Cord Ver1.5
/*
チェックポイント
(0)当サンプルコードの開発環境
目的
土壌センサの測定値をLANを通じてUECS形式で出力する

機種
SLT5006
Arduino UNO
Ethernet Shield R3(W5100)

ソフトウェア
Arduino IDE 2.2.1
UARDECS(W5100用)

(1)センサ
品番：SLT5006(UART)
(2)接続を確認する
////////////////////////////////////////////////////////////
//Sensor ->Arduino Pin connection
//RED&White  -->5V
//Black      -->GND
//Blue      TxD -->D2(->RX)
//Yellow    RxD -->D3(<-TX)
//Baud rate =9600
////////////////////////////////////////////////////////////
(3)UARDECS導入する
参考：https://uecs.org/arduino/uardecs.html
当サンプルコードではArduino UNO　＋　Ethernet Shield R3(W5100)を使用しています。
◎Ethernet Shield R3またはW5100を搭載した互換シールドを使う場合
　"W5100(旧機種用)"フォルダにファイルが入っています
　"libraries"の中に入っている"UARDECS"と"UARDECS_MEGA"フォルダをコピーします。
　①最初からインストールされているEthernetライブラリを探します。インストーラ付きのIDEを使っている場合は、”C:\Program Files (x86)\Arduino\libraries”の中にあります。ここにEthernetというフォルダがあるのでフォルダごと、ここからコピーします。
　②Ethernetライブラリをマイドキュメント/Arduino/librariesのUARDECSと同じフォルダの中にペーストします。
　③ライブラリマネージャという画面が出るので、タイプに「全て」、検索欄にethernetと入力するとEthernet Built-IN by Various・・・というライブラリが出てくるのでバージョンが2.0.0であることを確認します。
　④IDE1.8.7ではバージョン2.0.0が最初からインストールされています。もし、他のバージョンが検出された場合、バージョンを選択→バージョン2.0.0を選んでインストールして下さい。
　⑤次に、再びマイドキュメント/Arduino/librariesフォルダに入ります。先程コピーしたEthernetフォルダを開き、その下のsrcフォルダに入ると"socket.cpp"があります。UARDECSの"W5100用Ethernetパッチ(不具合対応)"フォルダにある、Ver2.0.0用の"socket.cpp"で上書きします。
◎IPアドレスの設定
受け手側IPアドレスは192.168.1.1に設定していますか？
サブネットマスクは255.255.255.0に設定してますか？
ブラウザの確認画面は192.168.1.7を参照していますか？
*/
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

////////////////////////////////////////////////////////////
#include <SPI.h>
//#include <Ethernet.h> //Arduino IDE Ver1.7.2以降でW5500搭載機種
#include <Ethernet.h> //Ver1.7.2以降でW5100搭載機種
#include <avr/pgmspace.h>
#include <EEPROM.h>
#include <Uardecs.h>

const byte U_InitPin = 5;//UECSのEnable用　変更可能
const byte U_InitPin_Sense=HIGH;
const char U_name[] PROGMEM= "UARDECS Node v.1.0";
const char U_vender[] PROGMEM= "XXXXXX Co.";
const char U_uecsid[] PROGMEM= "000000000000";
const char U_footnote[] PROGMEM= "Test node";
char U_nodename[20] = "Sample";
UECSOriginalAttribute U_orgAttribute;
const int U_HtmlLine = 0;//入力欄なし
struct UECSUserHtml U_html[U_HtmlLine]={};

enum{
 CCMID_SoilTemp,
 CCMID_VWC,
 CCMID_ECbulk,
 CCMID_ECpore,
 CCMID_VWCR,
 CCMID_VWCC,
 CCMID_dummy,
};

const int U_MAX_CCM = CCMID_dummy;//CCMの総数
UECSCCM U_ccmList[U_MAX_CCM];
//CCM定義用の素材、被らないように適当な変数名で3つずつ宣言(必ずPROGMEMを付ける)

const char ccmInfo1[] PROGMEM= "Test";//CCMの説明(Webでのみ表示)
const char ccmType1[] PROGMEM= "SoilTemp.mIC";//CCMのType文字列
const char ccmUnit1[] PROGMEM= "degC";//CCMの単位
const char ccmInfo2[] PROGMEM= "Test";
const char ccmType2[] PROGMEM= "VWC";
const char ccmUnit2[] PROGMEM= "%";
const char ccmInfo3[] PROGMEM= "Test";
const char ccmType3[] PROGMEM= "ECbulk";
const char ccmUnit3[] PROGMEM= "dS/m";
const char ccmInfo4[] PROGMEM= "Test";
const char ccmType4[] PROGMEM= "ECpore";
const char ccmUnit4[] PROGMEM= "dS/m";
const char ccmInfo5[] PROGMEM= "Test";
const char ccmType5[] PROGMEM= "VWCROCK";
const char ccmUnit5[] PROGMEM= "%";
const char ccmInfo6[] PROGMEM= "Test";
const char ccmType6[] PROGMEM= "VWCCOCO";
const char ccmUnit6[] PROGMEM= "%";

void UserInit(){
U_orgAttribute.mac[0] = 0x00;//MACアドレスの設定
U_orgAttribute.mac[1] = 0x00;
U_orgAttribute.mac[2] = 0x00;
U_orgAttribute.mac[3] = 0x00;
U_orgAttribute.mac[4] = 0x00;
U_orgAttribute.mac[5] = 0x00;

//UECSsetCCM(送受信の区分[trueで送信],通し番号[0から始まる],CCM説明,Type,単位,priority[通常は29],少数桁数[0で整数],送信頻度設定[A_1S_0で1秒間隔])
UECSsetCCM(true,0 ,ccmInfo1,ccmType1,ccmUnit1,29,4,A_10S_0);//Temp 4桁
UECSsetCCM(true,1 ,ccmInfo2,ccmType2,ccmUnit2,29,1,A_10S_0);//VWC　1桁
UECSsetCCM(true,2 ,ccmInfo3,ccmType3,ccmUnit3,29,3,A_10S_0);//bulk　3桁
UECSsetCCM(true,3 ,ccmInfo4,ccmType4,ccmUnit4,29,3,A_10S_0);//pore　3桁
UECSsetCCM(true,4 ,ccmInfo4,ccmType5,ccmUnit5,29,1,A_10S_0);//VWCROCK　1桁
UECSsetCCM(true,5 ,ccmInfo5,ccmType6,ccmUnit6,29,1,A_10S_0);//VWCCOCO　1桁

//棟・機械番号の設定
U_ccmList[CCMID_SoilTemp].baseAttribute[0] =1;//room
U_ccmList[CCMID_SoilTemp].baseAttribute[1] =1;//
U_ccmList[CCMID_SoilTemp].baseAttribute[2] =1;//
}
void UserEverySecond(){//毎秒の動作
}

void UserEveryMinute(){//毎分の動作
float num=0;
Serial.println("minute");
for (int i = 1; i < 2; i++){
program();
// Serial.print("count");Serial.println(i);//debug用
}
}

void OnWebFormRecieved(){}//sendボタンの利用
void UserEveryLoop(){}

////////////////////////////////////////////////////////////

void sendUECS(float temp2,float bulk2,float VWC2,float pore2,float VWCR2,float VWCC2){

float temp3=temp2*10000;//有効桁数分調整
float VWC3=VWC2*10;
float VWCR3=VWCR2*10;
float VWCC3=VWCC2*10;
float bulk3=bulk2*1000;
float pore3=pore2*1000;
U_ccmList[CCMID_SoilTemp].value=temp3;
U_ccmList[CCMID_VWC].value=VWC3;
U_ccmList[CCMID_ECbulk].value=bulk3;
U_ccmList[CCMID_ECpore].value=pore3;
U_ccmList[CCMID_VWCR].value=VWCR3;
U_ccmList[CCMID_VWCC].value=VWCC3;
/*デバック用
 Serial.print("CCMvalue,"); 
 Serial.print(temp3); Serial.print(",");
 Serial.print(bulk3); Serial.print(","); 
 Serial.print(VWC3); Serial.print(","); 
 Serial.print(pore3); Serial.print(","); 
 Serial.print(VWCR3); Serial.print(","); 
 Serial.print(VWCC3); Serial.println(","); 
*/
}

////////////////////////////////////////////////////////////
/*　UARDECSより引用
CCMへのアクセス方法
各CCMの状態はU_ccmList[CCMID]という構造体に格納されています
受信に設定したCCMの値は受信すると自動更新されます
送信に設定したCCMの値は自動送信されます

boolean U_ccmList[CCMID].sender;	//送受信の別
const char * U_ccmList[CCMID].name;	//CCM名(表示用)
const char * U_ccmList[CCMID].type;	//type(送受信用)
const char * U_ccmList[CCMID].unit;	//単位
unsigned char U_ccmList[CCMID].decimal;	//小数桁数
signed long U_ccmList[CCMID].value;	//CCMの値
signed char U_ccmList[CCMID].ccmLevel;	//送受信レベル(A_1S_0など、UECS仕様書参照)
signed short U_ccmList[CCMID].attribute[4];//受信時、受信したCCMのroom,region,order,priorityの順に格納
signed short U_ccmList[CCMID].baseAttribute[3];//ノードに設定したroom,region,order
boolean U_ccmList[CCMID].validity;	//受信時、CCMの値が有効かどうか(受信不能、タイムアウトするとfalse)
IPAddress U_ccmList[CCMID].address;	//受信時、相手のIPアドレス
boolean U_ccmList[CCMID].flagStimeRfirst;//送信:trueにするとCCMが送信される 受信:初めて受信するとtrueにセットされる

valueの小数桁数の設定について
送信側:
例えば小数桁数1の場合、valueに123を代入すると12.3として送信されます。
floatなどから変換する場合、10倍して整数にする処理が必要です。

受信側:
受信したCCMの小数桁数にかかわらず、valueはプログラム内で設定した小数桁数に丸められます。
例えば、小数桁数1の場合以下のように値が変換されてvalueに代入されます
123→1230
123.4→1234
123.45→1234
ただし、条件によっては桁数の変換時に誤差が発生することがあります。
*/
///////////////////////////////////////////////////////////

int flag;
int size;

//センサへの送信
void writecom(int size,int value[]){
  for (int i=0; i<size;i++){
  mySerial.write(value[i]);
  }
  while (mySerial.available() > 0) {//受信バッファクリア
    char t = mySerial.read();
  }
/*デバッグ用
  Serial.print("From Arduino:");Serial.print(" ");
  for (int i=0; i<size;i++){
    Serial.print(value[i],HEX);Serial.print(" "); 
  }
  Serial.println("send");
*/
}

// センサからの読み出し
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
sendvalue(flag,temp,bulk,VWC,pore,VWCR,VWCC);
sendUECS(temp,bulk,VWC,pore,VWCR,VWCC);
}

void sendvalue(int flag2,float temp2,float bulk2,float VWC2,float pore2,float VWCR2,float VWCC2){
if(flag2==1){//測定値取り出すときの処理
// Serial.print(flag);Serial.println("display");//デバッグ用
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
}
}

void program(){
  //測定開始
//Serial.print("Start sensor");//デバッグ用
  int  value[]={0X02,0X07,0X01,0X01,0X0D,0X70};
  writecom(6,value);
  delay(100);
  receive(6);//受信バイト数6

 //測定完了ステータス読み出し
  flag=0;
 while(flag==0){
//  Serial.print("Read status");//デバッグ用
  int value1[]={0X01,0X08,0X01,0X00,0XE6,0X00};
  writecom(5,value1);
  delay(100);
  receive(6);//受信バイト数6
  delay(500);//測定完了ステータス確認サイクル
  }

//測定データ読み出し
  size=5;
  int value2[]={0X01,0X13,0X10,0XFC,0X2C,0X00};
  writecom(size,value2);
  delay(100);
  receive2(21);//受信バイト数21

}

void setup() {
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

  UECSsetup();
  }

void loop() {
  program();
  UECSloop();
  int interval=3000;
  delay(interval);//測定間隔(ms)
}

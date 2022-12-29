
#define LED 10
 
hw_timer_t * timer = NULL; //timer 初期化

bool flag = true;

//タイマー割込みサービスルーチン
void IRAM_ATTR LED_Blink() {
   
  if(flag == true){   
    Serial.println("Timer  ON");
    digitalWrite(LED,HIGH);
    flag = false;
  } else {   
    Serial.println("Timer OFF");
    digitalWrite(LED,LOW);
    flag = true;
  }
}
 
void setup() {
  Serial.begin(115200);

  pinMode(LED,OUTPUT);
  digitalWrite(LED,LOW);
  
  timer = timerBegin(0, 80, true); //timer=1us
  timerAttachInterrupt(timer, &LED_Blink, true); //タイマー割込みに使う関数を登録
  timerAlarmWrite(timer, 500000, true); // タイマー割込み時間の設定us単位 500ms
  timerAlarmEnable(timer);
}
 
void loop() {
  delay(10);
}

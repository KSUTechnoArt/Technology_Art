#define SERVO_PIN 10 // サーボモータ(前輪モーター)のピン番号
#define LED_PIN 4 // LED(確認用)のピン番号

hw_timer_t * timer = NULL; // timer 初期化
volatile int length_pwm_time_servo = 500; // サーボモータのPWM用変数。volatileは、すぐに値が変わるような変数の型として用いる。

/*
<サーボモーターについて>
PWMを利用して回転量を調整する。
HIGHとなる時間の範囲は0.5msから2.4msの間。

【参考用】
0.5ms  -> -90度
1.45ms -> 0度
2.4ms  -> 90度

なぜタイマー割込みなのか？ -> Servoライブラリが使えないため。
*/

//タイマー割込みサービスルーチン
void IRAM_ATTR call_Servo() {
  digitalWrite(LED_PIN, HIGH);
  digitalWrite(SERVO_PIN, HIGH);
  delayMicroseconds(length_pwm_time_servo);
  digitalWrite(LED_PIN, LOW);
  digitalWrite(SERVO_PIN, LOW);
}

void setup() {
  pinMode(SERVO_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(SERVO_PIN, LOW);
  digitalWrite(LED_PIN, LOW);
  
  timer = timerBegin(0, 80, true); // timer=1us
  timerAttachInterrupt(timer, &call_Servo, true); // タイマー割込みに使う関数を登録
  timerAlarmWrite(timer, 20000, true); // タイマー割込み時間の設定us単位 20ms
  timerAlarmEnable(timer);
  Serial.begin(115200);
}

void loop() {
  length_pwm_time_servo = 500; // -90度
  delay(2000);
  length_pwm_time_servo = 1450; // 0度
  delay(2000);
  length_pwm_time_servo = 2400; // 90度
  delay(2000);
  length_pwm_time_servo = 1450; // 0度
  delay(2000);
}

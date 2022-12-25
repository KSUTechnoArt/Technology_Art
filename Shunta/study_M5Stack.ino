// M5Stamp-C3のプログラム
// このプログラムの構造を保ったままモーターや距離センサー用にアレンジできる。
#define LED 0    // LED
#define BUTTON 1 // ボタン
#define ANA 4    // 可変抵抗

void setup() {
  pinMode(LED, OUTPUT);   // LED番ピンを出力に設定
  pinMode(BUTTON, INPUT); // BUTTON番ピンを入力に設定
  pinMode(ANA, INPUT);    // ANA番ピンを入力に設定
  Serial.begin(115200);   // シリアル通信の設定
}

void loop() {
  // ここは、if文でも可能だがwhile文の方が望ましい
  while(digitalRead(BUTTON) == LOW){
    // ボタンが押されたら･･･
    Serial.println("Button ON");
    // LEDの点滅処理
    digitalWrite(LED, LOW);
    delay(500);
    digitalWrite(LED, HIGH);
    delay(500);
  }

  Serial.printf("Volt = %.2f\n", (analogRead(ANA) / 4096.0) * 3.3);
  delay(500); // 500ミリ秒待機
}

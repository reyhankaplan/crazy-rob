#include <Servo.h>
const int motor1[] = {8, 9, 10, 11}; // Step Motor

const int delayTime = 3;
const int controlPins[] = {49, 47, 45};
const int actionPin = 51;

const int servoPin = 13; // Servo Motor Pini
const Servo servoMotor;
int servoValue = 90;

void setup() {
  /*Motor pinleri OUTPUT moduna getirilir*/
  for(int i=0; i<4; i++) {
    pinMode(motor1[i], OUTPUT);
    }

    /* Control pinleri ise INPUT moduna getirilir. */
    for(int i=0; i<3; i++) {
    pinMode(controlPins[i], INPUT);
    }
    /* Hareket pini INPUT yapilir.*/
    pinMode(actionPin, INPUT);

    /* Servo kütüphanesinin Servo sınıfından örneklenen servoMotor
      değişkeni ile servo motorun bağlı olduğu pin iliştirilir. */

    servoMotor.attach(servoPin);

    /* Ve servo motora başlangıç olarak servoValue'nin değeri verilir. */
    
    servoMotor.write(servoValue);
    
    Serial.begin(9600);
}

void loop() {

  int sw = 0; // Hangi motorun hangi yonde donecegini belirleyecek olan deger
  /* 
     Bu değer controlPins'den okunan okunan digital değerlerin sırayla 2'nin 0., 1. ve 2. kuvvetleri ile çarpılıp toplanmasıyla bulunur.
     Bu pinler 3 step motorun ve servo motorun iki yönde dönüşlerinden hangisinin gerçekleşeceğini belirler. Yani 8 durum 3 pin ile kontrol edilir.
  */
  for(int i=0; i<3; i++) {
    sw += ( ( digitalRead(controlPins[i])==HIGH ) ? 1 : 0 ) * (i==0 ? 1 : (i==1 ? 2: 4));
  }

  /*
     actionPin'den HIGH olduğunda robot sw değerine göre hareket ettirilir, LOW olduğunda ise hareket ettirilmez.
  */
  if(digitalRead(actionPin) == HIGH) {
    Serial.print("\nSwitch value: ");
    Serial.println(sw);
    Serial.println("Action pin is HIGH");
    switch(sw) {
      
      case 0:
        servoValue += 3;
        gripper();
        break;
      case 1:
        moveNext(motor1);
        break;
      case 2:
        moveBack(motor1);
        break;
      case 3:
        Serial.println("Move motor2 to next");
        break;
      case 4:
        Serial.println("Move motor2 to back");
        break;
      case 5:
        Serial.println("Move motor3 to next");
        break;
      case 6:
        Serial.println("Move motor3 to back");
        break;
      case 7:
        servoValue -= 3;
        gripper();
        break;
       
      }
      delay(2);
  }
}

void gripper() {
  /* Bu fonksiyon gripper'ın hareketini sağlayan servo motoru hareket ettirir. */
  servoValue = servoValue > 180 ? 180: (servoValue < 0 ? 0: servoValue);
  Serial.println("\nServo value is ");
  Serial.print(servoValue);
  Serial.println("");
  servoMotor.write(servoValue);
  delay(10);
}

void moveNext(int motor[]) {
  
  int _step = 0;

  /* Burada tanımlanan _step değişkeni, motorun
  hangi indexteki pinine HIGH verileceğini tutmaktadır.
  Aynı zamanda bir tam adımın kaçıncı çeyrek adımında olduğunu temsil etmektedir.
  Bu fonksiyonda ileri tam adım atıldığı için 0. indexteki pinden adım başlamıştır. */

  while(_step < 4) {

  /* 4 pinin her birine sırasıyla HIGH ve kalanlara LOW verilmesi için
    için 4 kez işleme giren while döngüsü yazılmıştır. */
    
   for(int i=0; i<4; i++) {

    /* Ve for döngüsü içerisinde motorun _step indexindeki pinine
    HIGH diğer pinlerine LOW verilmiştir. */
    
    if(i != _step){
        digitalWrite(motor[i], LOW);
      }else {
          digitalWrite(motor[i], HIGH);
        }
    }
    
    delay(delayTime);

    /* Her pine değeri verildikten sonra delayTime kadar motorun 
    hareket edebilmesi için beklenmiştir.
    Ardından _step değişkeni artırılarak sonraki çeyrek adıma geçilmiştir. */
    
    _step ++;
   }
   for (int i=0; i<4; i++) {
    digitalWrite(motor[i], LOW);
   }
   delay(delayTime);
}

void moveBack(int motor[]) {

  /* Bu fonksiyonun moveNext fonksiyonundan tek farkı, tam adıma, 
  motorun 3. indexteki pinine HIGH vererek başlaması ve
  _step değişkeninin azaltarak ters yönde adım atılmasını sağlamasıdır. */
  
  int _step = 3;

  // Bir adim
  while(_step > -1) {

   for(int i=0; i<4; i++) {
    
    if(i != _step){
        digitalWrite(motor[i], LOW);
      }else {
          digitalWrite(motor[i], HIGH);
        }
    }
    delay(delayTime);
    _step --;
   }
   for (int i=0; i<4; i++) {
    digitalWrite(motor[i], LOW);
   }
   delay(delayTime);
}

  

#include <Servo.h>
const int motor1[] = {4, 5, 6, 7}; // Step Motor 1 - X ekseni 
const int motor2[] = {8, 9, 10, 11}; // Step Motor 2 - Z ekseni 
const int motor3[] = {14, 15, 16, 17}; // Step Motor 3 - Y ekseni

const int delayTime = 2;
const int xPin = A0; // Joy 1 X
const int yPin = A1; // Joy 1 Y
const int zPin = A2; // Joy 2 X
const int uPin = A3; // Joy 2 Y

const int servoPin = 13; // Servo Motor Pini
const Servo servoMotor;
int servoValue = 0;

void setup() {

   /* For döngüsü içerisinde, başlangıçta tanımlanan motor pinleri
    OUTPUT moduna getirilir. */


  for(int i=0; i<4; i++) {
    pinMode(motor1[i], OUTPUT);
    pinMode(motor2[i], OUTPUT);
    pinMode(motor3[i], OUTPUT);
    }

    /* Joystick pinleri ise INPUT moduna getirilir. */
    
    pinMode(xPin, INPUT);
    pinMode(yPin, INPUT);
    pinMode(zPin, INPUT);
    pinMode(uPin, INPUT);

    /* Servo kütüphanesinin Servo sınıfından örneklenen servoMotor
      değişkeni ile servo motorun bağlı olduğu pin iliştirilir. */

    servoMotor.attach(servoPin);

    /* Ve servo motora başlangıç olarak servoValue'nin değeri verilir. */
    
    servoMotor.write(servoValue);
    
    Serial.begin(9600);
}

void loop() {

  /* Döngüye girildiğinde ilk olarak analogRead ile 
    joysticklerin pinlerinden değerler okunur. */  
    
  int xValue = analogRead(xPin);
  int yValue = analogRead(yPin);
  int zValue = analogRead(zPin);
  int uValue = analogRead(uPin);

  /* Burada xValue ve yValue birinci joyistiğin pinlerinden okunan, 
    zValue ve uValue ise ikinci joyistiğin pinlerinden okunan sırasıyla x ve y değerleridir. 
    Joyistik yönlendirilmediğinde bu değerler 512 olmalıdır. */ 

    /* Birinci joyistiğin değerleri ve ikinci joyistiğin x yönündeki değeri 
    470'den küçük olduğunda her pin için belirlenen step motor geriye adım atmayı sağlayan 
    moveBack fonksiyonuna gönderilir. 600'den büyük olduğunda ise ilgili step motor ileri 
    adım atmayı sağlayan moveNext fonksiyonuna gönderilir. */ 
    
    /* İkinci joyistiğin y yönündeki değeri ise servo motoru yani gripperı kontrol eder.
    470'den küçük olduğunda veya 600'den büyük olduğunda gripper fonksiyonu çağırılır ve gripperın
    açılıp kapanması sağlanır. */

  if(yValue < 470) {
    /// y degeri 470'den küçükse
    moveBack(motor1);
    
    }else if(yValue > 600) {
      /// y degeri 600'den büyükse
      moveNext(motor1);
      }
  if(xValue < 470) {
      /// x degeri 470'den küçükse
      moveBack(motor2);
    
    }else if(xValue > 600) {
      /// x degeri 600'den büyükse
      moveNext(motor2);
      }
  if(zValue < 470) {
      /// z degeri 470'den küçükse
      moveBack(motor3);
    
    }else if(zValue > 600) {
      /// z degeri 600'den büyükse
      moveNext(motor3);
      }
  if(uValue < 470) {
      /// u degeri 470'den küçükse
      gripper();
    
    }else if(uValue > 600) {
      /// u degeri 600'den büyükse
      gripper();
      }  
}

void gripper() {
  
   outputJoystick();
   
  /* Bu fonksiyon servo motor aracılığı ile 
    gripperı hareket ettirmek için kullanılır. Program içerisinde ikinci joyistiğin
    y yönündeki değerine yani uPin'inden okunan değere göre çağırılır. */
    
    /* analogRead fonksiyonu ile, ikinci joystiğin Y pini, uPin'den
    0 ile 1024 arasında bir değer okunur. */
  

   
   servoValue = analogRead(uPin);

   /* Ve bu değer map fonksiyonu ile 100 ile 900 aralığından
    90 ile 185 arasında bir değere dönüştürülerek, servo motora verdiğimiz değeri tutan
    servoValue değişkenine atanır. */
   
   servoValue = map(servoValue, 100, 900, 90, 185);

   /* Daha sonra servo motor için hazırlanan bu değişkenin değeri 
    write metodu ile servo motora verilir */ 

   servoMotor.write(servoValue);
   delay(450);

   /* 450 milisaniye duraklatıldıktan sonra program akışına dönülür */
  }

void outputJoystick() {

  Serial.print(analogRead(zPin));
  Serial.print("----");
  
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
}

  

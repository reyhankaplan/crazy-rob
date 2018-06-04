#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <Hash.h>

/* Wifi baglantı bilgileri sabit olarak tanımlanır. */
const char* ssid = "AndroidAP";
const char* password = "YoYoYoYo";
/* Kullanıcılara gönderilecek olan html sayfaları sabit stringler olarak tanımlanır. */
const String homePage = "<!DOCTYPE html><html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"><title>CrazyRob</title><style>html,body{height:100%;}body{color:#333;font-family:'Ubuntu',cursive;display:flex;justify-content:center;flex-direction:column;align-items:center;}div{max-width:70%;display:flex;justify-content:space-between;flex-wrap:wrap;}a{display:inline-block;padding:2em;margin:0.5em;text-decoration:none;color:#333;box-shadow: 0px 1px 5px 0 rgba(0,0,0,0.3);}a:hover,a:active{box-shadow: 0px 1px 5px 0 rgba(0,0,0,0.5);}</style></head><body><h1>CrazyRob</h1><div><a href=\"XNext\">X+</a><a href=\"XBack\">X-</a><a href=\"YNext\">Y+</a><a href=\"YBack\">Y-</a><a href=\"ZNext\">Z+</a><a href=\"ZBack\">Z-</a><a href=\"GOpen\">G+</a><a href=\"GClose\">G-</a></div></body></html>";
const String stopPage = "<!DOCTYPE html><html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"><title>CrazyRob</title><style>html,body{height:100%;}body{color:#333;font-family:'Ubuntu',cursive;display:flex;justify-content:center;flex-direction:column;align-items:center;}div{max-width:70%;display:flex;justify-content:space-between;flex-wrap:wrap;}a{display:inline-block;padding:2em;margin:0.5em;text-decoration:none;color:#333;box-shadow: 0px 1px 5px 0 rgba(200,0,0,0.3);}a:hover,a:active{box-shadow: 0px 1px 5px 0 rgba(200,0,0,0.5);}</style></head><body><h1>CrazyRob</h1><div><a href=\"/\">Dur</a></div></body></html>";

/* WebSocket sunucusu ve web sunucusu örneklenir. */
WebSocketsServer webSocket = WebSocketsServer(81);
ESP8266WebServer server(80); 

int LEDPin = 13; // LED'in bağlandığı pin -> D7
int controlPins[] = {14, 12, 15}; // Kontrol pinleri -> D5, D6, D8
int actPin = 4; // Hareket kontrol pini -> D2  

void setup(void){

  /* LED pini OUTPUT moduna getirilir ve başlangıçta LOW verilir. */
  pinMode(LEDPin, OUTPUT);
  digitalWrite(LEDPin, LOW);
  
  /* Kontrol pinleri OUTPUT moduna getirilir ve LOW verilir.*/
  for(int i=0; i<3; i++) {
    pinMode(controlPins[i], OUTPUT);
    digitalWrite(controlPins[i], LOW);
  }
  
  /* Aynı işlem hareket pini için yapılır. Bu pin hareketi durdurmak için kullanılır.*/
  pinMode(actPin, OUTPUT);
  digitalWrite(actPin, LOW);
  
  delay(1000);

  /* Serial 115200 baudrate ile başlatılır. */
  Serial.begin(115200);

  /* Wifi bağlantısı başlatılır. */
  WiFi.begin(ssid, password);
  Serial.println("");

  /* Bağlanılana kadar Serial Monitor'e . yazılır. Bir kez bağlandıktan sonra bağlantı kopsa bile kütüphane tekrar bağlanma işlemini kendisi yapar. */
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  /* Wifi bağlantısı kurulduktan sonra LED yakılır ve yerel IP adresi Serial Monitöre yazılır. */
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  digitalWrite(LEDPin, HIGH);

  /* İstek gönderilen URL'lere göre sayfalar gönderilir. */
  server.on("/", [](){
    digitalWrite(actPin, LOW);
    server.send(200, "text/html", homePage);
  });
  
  server.on("/XNext", [](){
    server.send(200, "text/html", stopPage);
    writeControlPins(false, false, true);
    digitalWrite(actPin, HIGH);
    delay(1000);
  });
  
  server.on("/XBack", [](){
    server.send(200, "text/html", stopPage);
    writeControlPins(false, true, false);
    digitalWrite(actPin, HIGH);
    delay(1000);
  });
  
  server.on("/YNext", [](){
    server.send(200, "text/html", stopPage);
    writeControlPins(false, true, true);
    digitalWrite(actPin, HIGH);
    delay(1000);
  });
  
  server.on("/YBack", [](){
    server.send(200, "text/html", stopPage);
    writeControlPins(true, false, false);
    digitalWrite(actPin, HIGH);
    delay(1000);
  });
  
  server.on("/ZNext", [](){
    server.send(200, "text/html", stopPage);
    writeControlPins(true, false, true);
    digitalWrite(actPin, HIGH);
    delay(1000);
  });
  
  server.on("/ZBack", [](){
    server.send(200, "text/html", stopPage);
    writeControlPins(true, true, false);
    digitalWrite(actPin, HIGH);
    delay(1000);
  });
  
  server.on("/GOpen", [](){
    server.send(200, "text/html", stopPage);
    writeControlPins(false, false, false);
    digitalWrite(actPin, HIGH);
    delay(1000);
  });
  
  server.on("/GClose", [](){
    server.send(200, "text/html", stopPage);
    writeControlPins(true, true, true);
    digitalWrite(actPin, HIGH);
    delay(1000);
  });

  /* Sunucu başlatılır. */
  server.begin();
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  Serial.println("Web server started!");
}

void loop(void){
  webSocket.loop();
  server.handleClient();
  if (Serial.available() > 0){
    char c[] = {(char)Serial.read()};
    webSocket.broadcastTXT(c, sizeof(c));
  }
}

void writeControlPins(bool i0, bool i1, bool i2) {
  /* Bu fonksiyon ile kontrol pinlerine değerleri yazılır. */
  digitalWrite(controlPins[0], i0?HIGH:LOW);
  digitalWrite(controlPins[1], i1?HIGH:LOW);
  digitalWrite(controlPins[2], i2?HIGH:LOW);
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length){
  if (type == WStype_TEXT){
   for(int i = 0; i < length; i++) Serial.print((char) payload[i]);
   Serial.println();
  }
}

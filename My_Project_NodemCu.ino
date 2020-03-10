//==================================================================================
//          KÜTÜPHANE TANIMLAMALARI
//==================================================================================
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
//==================================================================================
//          WİFİ SSID VE PASSWORD
//==================================================================================
const char *ssid = "ESP32";
const char *password = "ESP32Test";
//==================================================================================
//          WİFİ İSTEMCİ HOST AYARLARI 
//==================================================================================
WiFiClient client,client2; // WiFi istemci oluşturma
const char *host = "192.168.4.1";
const int httpPort = 90;
const int httpGuvenlikPort = 100;
//==================================================================================
//          SENSÖR PİN TANIMLAMALARI
//==================================================================================
const int pirSensor = 15; // D8-GPIO15 Hareket sensörü sinyal pini
const int buzzerPin = 12; // D6-GPIO12 Buzzer sinyal pini
const int led = 13;       // D7-GPIO13 Led sinyal pini
const int ledBeyaz = 2;   // D4-GPIO02 Led sinyal pini
const int buton = 14;     // D5-GPIO14 Button sinyal pini
//==================================================================================
//          SENSÖR DEĞİŞKEN TANIMLAMALARI
//==================================================================================
int pirDurum = LOW; // Hareket algılama başlangıç durumu
int pirDeger = 0;      // Pinden Gelen Veri
int butonDurum = LOW;
int butonOnce = LOW;
int butonDeger = 0;
long zaman = 0;
long debounce = 200;
//==================================================================================
//          PROGRAM SETUP BÖLÜMÜ
//==================================================================================
void setup(){  
//==================================================================================
//          SENSÖR PİNMODE AYARLARI
//==================================================================================
  pinMode(pirSensor, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(led, OUTPUT);
  pinMode(ledBeyaz, OUTPUT);
  pinMode(buton, INPUT);
//==================================================================================
  Serial.begin(115200);
  
  WiFiBaglantiSetup(); 
}
//==================================================================================
//          PROGRAM LOOP BÖLÜMÜ
//==================================================================================
void loop(){
  if(WiFi.status() != WL_CONNECTED)
  {
    WiFiBaglantiSetup();
  }
  guvenlikBaslat();
}
//==================================================================================
//          WİFİ BAĞLANTI AYARLARI VE STA MODE AYARI 
//==================================================================================
void WiFiBaglantiSetup(){
  Serial.println();
  Serial.println();
  Serial.print("Bağlanıyor ");
  Serial.println(ssid);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid,password);
  
  while(WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    digitalWrite(led, HIGH);
    delay(50);
    digitalWrite(led, LOW);
    delay(50);
  }
  digitalWrite(led, HIGH);
  Serial.println();
  Serial.println("WiFi Bağlantısı Kuruldu.");
  Serial.print("IP Adresi: ");
  Serial.println(WiFi.localIP());  
}
//==================================================================================
//          TCP BAĞLANTI OLUŞTURMA 
//==================================================================================
void TcpBaglanti(){

  Serial.print("Bağlanıyor ");
  Serial.println(host);
  if ( !client.connect (host, httpPort))
  {
     Serial.println("Bağlantı Başarısız.");
     delay(1000);
     return;   
  }
  
  Serial.println("Server Bağlantısı Başarılı!!!");
  for(int i = 0; i<5 ; i++){
    digitalWrite(ledBeyaz, HIGH);
    delay(50);
    digitalWrite(ledBeyaz, LOW);
    delay(50);
  }   
}
//==================================================================================
//          TCP GÜVENLİK İÇİN BAĞLANTI OLUŞTURMA 
//==================================================================================
void TcpGuvenlikBaglanti(){

  Serial.print("Bağlanıyor ");
  Serial.println(host);
  if ( !client2.connect (host, httpGuvenlikPort))
  {
     Serial.println("Bağlantı Başarısız.");
     delay(1000);
     return;   
  }
  
  Serial.println("Server Bağlantısı Başarılı!!!");
  for(int i = 0; i<5 ; i++){
    digitalWrite(ledBeyaz, HIGH);
    delay(50);
    digitalWrite(ledBeyaz, LOW);
    delay(50);
  }   
}
//==================================================================================
//          SUNUCUYA VERİ GÖNDERME İSTEĞİ OLUŞTURMA 
//==================================================================================
void SunucuIstek()
{
  // Sunucuya istek göndermek için url oluşturuluyor.
  String url = "/sensor/";
  url += "?sensor_oku=";
  url += pirDeger;
  
  Serial.print("İstek URL: ");
  Serial.println(url);
  Serial.println("Sunucuya İstek Gönderiliyor...");
  //Sunucuya istek gönderme
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");
  //delay(2000);
  
  Serial.println("Bağlantı Sonlandı...");
  delay(500);
}
//==================================================================================
//          SUNUCUYA GÜVENLİK BAŞLATMA VERİSİ GÖNDERME 
//==================================================================================
void SunucuGuvenlikIstek(int veri)
{
  // Sunucuya istek göndermek için url oluşturuluyor.
  String url = "/guvenlik/";
  url += "?guvenlik_oku=";
  url += veri;
  
  Serial.print("İstek URL: ");
  Serial.println(url);
  Serial.println("Sunucuya İstek Gönderiliyor...");
  //Sunucuya istek gönderme
  client2.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");
  //delay(2000);
  
  
  Serial.println("Bağlantı Sonlandı...");
  delay(500);
}
//==================================================================================
//          SENSÖRLERDEN OKUNAN BİLGİLER
//==================================================================================
void sensorler(){
  pirDeger = digitalRead(pirSensor); // Sensörden gelen verinin okunması
  if ( pirDeger == HIGH ) // Gelen veri high ise hareketi algıla 
  {
    beep(500);
    digitalWrite(led, HIGH);
    digitalWrite(buzzerPin, HIGH);
    delay(500);
    
    if ( pirDurum == LOW ) 
    {
        Serial.println("Hareket Algılandı");  
        TcpBaglanti();
        SunucuIstek();
        pirDurum = HIGH; // Hareket algılandığı için durumu high yap
    }
  }
  else
  {
    digitalWrite(buzzerPin, LOW);   // Buzzeri low hale getir
    digitalWrite(led, LOW);
    //delay(1000);
    if ( pirDurum == HIGH ) // Durum high ise hareket algılanmadı
    {
       Serial.println("Hareket Algılanmadı");
       pirDurum = LOW; // Hareket algılanmadığı için durumu low yap
       delay(10000);
    }
  }
}
//==================================================================================
//          BUZZER İÇİN SES AYARLARI
//==================================================================================
void beep(unsigned char delayms)
{
   analogWrite(10, 255);
   delay(delayms);
   analogWrite(10, 150);
   delay(delayms); 
}
//==================================================================================
//          GÜVENLİK BAŞLATMA AYARLARI
//==================================================================================
void guvenlikBaslat()
{
  while(WiFi.status() != WL_CONNECTED)
  {
    WiFiBaglantiSetup();
  }
  digitalWrite(led, LOW);
  butonDeger = digitalRead(buton);
  
  if ( butonDeger == HIGH && butonOnce == LOW && millis() - zaman > debounce ) 
  {
    if ( butonDurum == HIGH ) 
    {
      Serial.println("GÜVENLİK SONLANDIRILDI.");
      TcpGuvenlikBaglanti();
      SunucuGuvenlikIstek(0);
      butonDurum = LOW;
    } else {
      Serial.println("GÜVENLİK BAŞLATILDI.");
      TcpGuvenlikBaglanti();
      SunucuGuvenlikIstek(1);
      butonDurum = HIGH;
      delay(10000);
    }
    zaman = millis();
  }
  digitalWrite(ledBeyaz, butonDurum);
  if( butonDurum == HIGH ) 
     sensorler();
  
  butonOnce == butonDeger;
}

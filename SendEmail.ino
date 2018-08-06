#include <dht.h>

#include <SPI.h>
#include <dht.h>
#include <LiquidCrystal_I2C.h>
#include <Ethernet.h>
#define DHT11_PIN 8


byte ethernetMACAddress[] ={  0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02 };
EthernetClient client;
LiquidCrystal_I2C lcd(0x3F,20,4);
byte temperature = 0;
int pinDHT11 = 8;
dht DHT;
IPAddress server( 192,168,10,5 );




void setup() {

              lcd.init();
   
              lcd.backlight();
              Serial.begin(9600);
                
              delay(4000);
              while(!Serial);
              
              Serial.println("Cekam DHCP....");
              lcd.setCursor(0,0);
              lcd.print("Cekam DHCP....");
              
              if (Ethernet.begin(ethernetMACAddress) == 0) {
              Serial.println("Ne mogu dobiti adresu");

              lcd.setCursor(0,0);
              lcd.print("Ne mogu dobiti");
              lcd.setCursor(0,1);
              lcd.print("adresu");
              delay(2000);
                setup();
              }
              lcd.setCursor(0,0);
              lcd.print("IP adresa je:   ");
              lcd.setCursor(0,1);
              
              for (byte thisByte = 0; thisByte < 4; thisByte++) { 
              Serial.print(Ethernet.localIP()[thisByte], DEC);
              lcd.print(Ethernet.localIP()[thisByte], DEC);
              lcd.print(".");
              
              Serial.print(".");
                delay(4000);
              }
             Serial.println();
            }

void loop(){       
              for (int i =0;i<10;i++){
                   Serial.println(i); 
                   Serial.println("=================================");
                   Serial.println("Ocitavanje senzora ...");

                   lcd.clear();
                   lcd.setCursor(0,0);
                   lcd.print("Ocitavanje"); 
                   lcd.setCursor(0,1);
                   lcd.print("Senzora ");
                   delay(4000);

                  int chk = DHT.read11(DHT11_PIN);
                  Serial.println("Sample OK: ");
                  Serial.print(DHT.temperature-2); Serial.println(" *C, ");
                  
                  lcd.setCursor(0,0);
                  lcd.clear();
                  lcd.print("Temperatura je:");
                  lcd.setCursor(0,1); 
                  lcd.print(DHT.temperature-2);
                  lcd.print("  *C...");     
                  delay(8000);

                        if(isnan(DHT.temperature) || DHT.temperature<0 || DHT.temperature-2>28){
                            if(i==9){
                              saljiMail();
                              
                            }
                                    
                        }
              }


  
}







                 

void saljiMail() {

  byte thisByte = 0;
  byte respCode;
 
  if(client.connect(server,25) == 1) {
    Serial.println(F("connected"));
  } else {
    Serial.println(F("connection failed"));
    return 0;
  }
 
  if(!eRcv()) return 0;
  Serial.println(F("Sending helo"));
 
// change to your public ip
  client.println(F("helo 1.2.3.4"));
 
  if(!eRcv()) return 0;
  Serial.println(F("wedret.h@??????.ba"));
 
// change to your email address (sender)
  client.println(F("MAIL From: <wedret.h@??????.ba>"));
 
  if(!eRcv()) return 0;
 
// change to recipient address
  Serial.println(F("Sending To"));
  client.println(F("RCPT To: <wedret.h@??????.ba>"));
  client.println(F("RCPT To: <nevres.hrvanovic@??????.ba>"));
  client.println(F("RCPT To: <test1@????.ba>"));

 
  if(!eRcv()) return 0;
 
  Serial.println(F("Sending DATA"));
  client.println(F("DATA"));
 
  if(!eRcv()) return 0;
 
  Serial.println(F("Sending email"));
 
// change to recipient address
  client.println(F("To: You <wedret.h@??????.ba>"));
  client.println(F("CC: You <nevres.hrvanovic@??????.ba>"));
 
// change to your address
  client.println(F("From: Me <wedret.h@??????.ba>"));
 
  client.println(F("Subject: Upozorenje \r\n"));
  String temp ;
  if(DHT.temperature-2>28 && DHT.temperature-2 <40 ){
  temp= "Visoka Temperatura = " + String(DHT.temperature-2);
  }else  temp="Provjeri senzor" ;
  
  client.println(temp);
 
  client.println(F("."));
 
  if(!eRcv()) return 0;
 
  Serial.println(F("Sending QUIT"));
  client.println(F("QUIT"));
 
  if(!eRcv()) return 0;
 
  client.stop();
 
  Serial.println(F("disconnected"));
    lcd.setCursor(0,0);
    lcd.print("Poslo mail,cekam");
    lcd.setCursor(0,1);
    lcd.print("pola sata");
    
  delay(1800000); 
  return 1;

  
}
 
byte eRcv()
{
  byte respCode;
  byte thisByte;
  int loopCount = 0;
 
  while(!client.available()) {
    delay(1);
    loopCount++;
 
    // if nothing received for 10 seconds, timeout
    if(loopCount > 10000) {
      client.stop();
      Serial.println(F("\r\nTimeout"));
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Greska ");
      lcd.setCursor(0,1);
      lcd.print("Resetuj uredjaj ");
      return 0;
    }
  }
 
  respCode = client.peek();
 
  while(client.available())
  {  
    thisByte = client.read();    
    Serial.write(thisByte);
  }
 
  if(respCode >= '4')
  {
    efail();
    return 0;  
  }
 
  return 1;
}
 
 
void efail()
{
  byte thisByte = 0;
  int loopCount = 0;
 
  client.println(F("QUIT"));
 
  while(!client.available()) {
    delay(1);
    loopCount++;
 
    // if nothing received for 10 seconds, timeout
    if(loopCount > 10000) {
      client.stop();
      Serial.println(F("\r\nTimeout"));
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Greska ");
      lcd.setCursor(0,1);
      lcd.print("Resetuj uredjaj ");
      return;
    }
  }
 
  while(client.available())
  {  
    thisByte = client.read();    
    Serial.write(thisByte);
  }
 
  client.stop();
 
  Serial.println(F("disconnected"));
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Greska ");
      lcd.setCursor(0,1);
      lcd.print("Resetuj uredjaj ");
}


  





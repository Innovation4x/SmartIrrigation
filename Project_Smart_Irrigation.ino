// Authour Sain Saji
// Reference for OLED
// https://github.com/sainsaji/SSD1306-Base-Code-for-Raspberry-Pico-Ardunio-Library/edit/main/README.md


//Libraries Included
#include <SPI.h>
#include <Wire.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <Adafruit_GFX.h>     //External Libraries for driving the OLED display
#include <Adafruit_SSD1306.h> //External Libraries for driving the OLED display
#include <DHT.h>

//MQTT Global Variables
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
IPAddress ip(172, 16, 0, 100);
IPAddress myDns(192, 168, 0, 1);
char server [] ="broker.emqx.io";
EthernetClient ethClient;
PubSubClient client(ethClient);

//Definitions
#define SCREEN_WIDTH 128       // OLED display width, in pixels
#define SCREEN_HEIGHT 64       // OLED display height, in pixels
#define OLED_RESET     -1      // Make sure this is set to -1 for Pico
#define SCREEN_ADDRESS 0x3C    // Use 3C with Pico for 128x64 OLED
#define DHTPIN 9               // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11          // Defining DHT Version ,use DHT22 for newer models

//Initialization
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
DHT dht(DHTPIN, DHTTYPE);

//Global Variables 
int current_status=3;       //Initial plant status [0:Healhty,1: Unhealthy, 3: No plant detected]
int rightX=26;              //Display right X value
int leftY=5;                //Display left  Y value
int soilMoistureValue = 0;  //Initial moisture value
int soilmoisturepercent=0;  //Mositure value in percentage

//Global Constants
const int AirValue = 620;    // Airvalue after calibrating
const int WaterValue = 310;  // Water Value after calibrating

//Display Frame bitmap
const unsigned char baseframe [] PROGMEM = {
  0x00, 0x1e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x73, 0x80, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0xe1, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0xe1, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0xe1, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0xe1, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0xe1, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0xe1, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0xed, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0xed, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0xed, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0xed, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0xed, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0xed, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0xed, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0xed, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x01, 0xcc, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x03, 0x9e, 0x70, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x03, 0xbf, 0x70, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x03, 0x3f, 0x30, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x03, 0x3f, 0x30, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x03, 0xbf, 0x70, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x03, 0x9e, 0x70, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x01, 0xc0, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0xf3, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x7f, 0x80, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x1e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x01, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0xdf, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x03, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x01, 0x07, 0x0e, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x03, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x01, 0x07, 0x24, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x07, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x01, 0x0f, 0x0c, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x07, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x01, 0x0f, 0x99, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x0f, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x01, 0x1f, 0xf2, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x1f, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x01, 0x1f, 0xe4, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x1f, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x01, 0x3f, 0xc9, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x1f, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x01, 0x3f, 0x9c, 0x7e, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x3f, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x01, 0x3f, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x3f, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x01, 0x3f, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x3f, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x01, 0x38, 0x61, 0x87, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x3f, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x01, 0x38, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x3f, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x01, 0x38, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x3f, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x01, 0x38, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x1f, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x01, 0x3c, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x1f, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x01, 0x1c, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x0f, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x01, 0x1e, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x07, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x01, 0x0f, 0x00, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x03, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x01, 0x07, 0xe1, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x01, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

//MQTT callback funtion
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  char messageBuffer[30];
  memcpy(messageBuffer, payload, length);
  messageBuffer[length] = '\0';
  Serial.println(messageBuffer);
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  //For convering payload to string
  char message[30];
  for(int i=0;i<length;i++){
    message[i]=payload[i];
    }
  String msg = String(message);

  //Checking if index contain healthy
  int he = msg.indexOf("healthy");
  //Checking if index contain "no" as in "no plants detected"
  int no = msg.indexOf("no");

  //if index contain "healthy" set current_status as 0 else 1
  if(he>0)
  current_status=0;
  else
  current_status=1;
  //if index contain "no" set current_status as 3
  if(no>=0)
  current_status=3;
  Serial.println();
}

//MQTT Reconnect callback
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("arduinoClient")) {
      Serial.println("connected");
      client.subscribe("home/planthealth/2147230");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

//MQTT Ethernet initialization
void ethstart()
{
  //ethernet codes
  Ethernet.init(17);  // WIZnet W5100S-EVB-Pico
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

    Serial.println("Initialize Ethernet with DHCP:");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
      while (true) {
        delay(1); // do nothing, no point running without Ethernet hardware
      }
    }
    if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Ethernet cable is not connected.");
    }
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip, myDns);
  } else {
    Serial.print("  DHCP assigned IP ");
    Serial.println(Ethernet.localIP());
  }
  // give the Ethernet shield a second to initialize:
  delay(1000);
  Serial.print("connecting to ");
  Serial.print(server);
  Serial.println("...");
  client.setClient(ethClient);
  client.setServer(server, 1883);
  client.setCallback(callback);
  // Allow the hardware to sort itself out
  delay(1500);
  
}

//To print temperature to OLED
void printTemp(int temp)
{
  display.setCursor(rightX,leftY);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.println(temp);
  display.setCursor(rightX+24,leftY);
  display.println("C");
}

//To print Humidity to OLED
void printHumidity(int humid)
{
  display.setCursor(rightX,leftY+33);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.println(humid);
  display.setCursor(rightX+24,leftY+33);
  display.println("%");
}

//To print moisture to OLED
void printMoist(int moist)
{
  display.setCursor(rightX+65,leftY+33);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.println(moist);
  display.setCursor(rightX+24+65,leftY+33);
  display.println("%");
}

////To print plant status to OLED
void printStatus(String status)
{
  display.setCursor(rightX+40,leftY);
  display.setTextSize(1.5);
  display.setTextColor(WHITE);
  display.println(status);
}


//setup
void setup()
{
  Serial.begin(115200);
  dht.begin();
  pinMode(14,OUTPUT);
  pinMode(LED_BUILTIN, INPUT);
  display.begin(SSD1306_SWITCHCAPVCC,0x3C);
  display.fillScreen(0); //0 for filling with black dots. 1 for white
  printStatus("Recalibrating");
  display.display();
  ethstart();
  pinMode(LED_BUILTIN, OUTPUT);
}


void loop()
  {
    //Reconnect to MQTT server if disconnected
    if (!client.connected()) {
    reconnect();
    }
    //Subscribe to topic
   client.subscribe("home/planthealth/2147230");    

  //Read temperature and humidity
  int temp=dht.readTemperature();
  int humidity = dht.readHumidity();

  //Read mositure Value
  soilMoistureValue = analogRead(A0);  //put Sensor insert into soil
  soilmoisturepercent = map(soilMoistureValue, AirValue, WaterValue, 0, 100);

   // Check if any reads failed and exit early (to try again).
  if (isnan(humidity) || isnan(temp)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  display.clearDisplay();
  printTemp(temp);
  Serial.println("Temperature");
  Serial.println(temp);
  printHumidity(humidity);
  Serial.println("Humidity");
  Serial.println(humidity);
  printMoist(soilmoisturepercent);
  
  //Startup pump if moisture percentage drops below 30%
  if(soilmoisturepercent<=30)
  {
    digitalWrite(14,HIGH);
    Serial.println("Pump Status: ");
    Serial.print("ON");
    delay(2000);
    digitalWrite(14,LOW);
    Serial.println("Pump Status: ");
    Serial.println("OFF");
    Serial.println("Recalibrating Moisture....... ");
    delay(10000);
  }
  else
  {
    digitalWrite(14,LOW);
  }
  Serial.println("Moisture");
  Serial.println(soilmoisturepercent);

  //Draw base frame on OLED
  display.drawBitmap(0, 0, baseframe, 128, 64, WHITE);

  //Display plant status based on current_status variable
  if(current_status==0)
  {
    printStatus("HEALTHY");
   }
   else if(current_status==1)
   {
    printStatus("DRIED");
    }
  else
  printStatus("NO PLANT");
  display.display();
  delay(100); 

  //To recieve MQTT messages
  client.loop();
}

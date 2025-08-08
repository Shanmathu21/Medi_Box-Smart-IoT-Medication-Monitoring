#include <iostream>      
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <time.h>
#include <DHTesp.h>
#include <ESP32Servo.h>
#include <PubSubClient.h>

#define NTP_SERVER "pool.ntp.org" 
#define UTC_OFFSET_DST 0

#define SCREEN_WIDTH 128  
#define SCREEN_HEIGHT 64   
#define OLED_RESET -1
#define SCREEN_ADDERSS 0x3c 

#define BUZZER 18
#define LED_ALARM 15
#define LED_2 2
#define CANCEL 26
#define UP 35
#define DOWN 32
#define OK 33
#define DHT 12
#define servoPin 25
#define ldr 34

Adafruit_SSD1306 display(SCREEN_WIDTH,SCREEN_HEIGHT, &Wire, OLED_RESET);  
Servo servoMotor;
DHTesp dhtSensor;



int UTC_OFFSET[]={5,30};
int n_notes=8;
int notes[] = {
  262, 294, 330, 349, 392, 440, 494, 523
};

int days=0;
int hours =0;
int minutes =0;
int seconds =0;

float ServoAngle=0.0;
float SamplingInterval=2.0;
float SendingInterval=20.0;
float light_intensity=0.0;
unsigned long sec_sample=0;
unsigned long sec_send=0;
int n=0;

bool alarm_enabled = false;
int n_alarms =2;
int alarm_hours[] = {9,0};
int alarm_minutes[] = {39,10};
bool alarm_triggered[] = {false,false};
bool alarm_active[] = {false,false};

int current_mode =0;
int max_modes = 5; 
String modes[]={
  "1 - Set Time",
  "2 - Set Timezone",
  "3 - Set Alarm", 
  "4 - Active Alarm", 
  "5 - Delete Alarm"
};
 
// Wi-Fi credentials
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// MQTT Broker details
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient mqttClient(espClient);
char tempAr[6];
char humAr[6];


void print_line(String text, int text_size, int row, int column){
  display.setTextSize(text_size);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(column,row);
  display.println(text);

  display.display();
}

void update_time(void){
  struct tm timeinfor;
  getLocalTime(&timeinfor);

  char day_str[8];
  char hour_str[8];
  char min_str[8];
  char sec_str[8];
  strftime(day_str,8, "%d",&timeinfor);
  strftime(sec_str,8, "%S",&timeinfor);
  strftime(hour_str,8, "%H",&timeinfor);
  strftime(min_str,8, "%M",&timeinfor);
  hours = atoi(hour_str);
  minutes = atoi(min_str);
  days = atoi(day_str);  
  seconds = atoi(sec_str);
}

void print_time_now(void){
  display.clearDisplay();

   print_line("Day", 1, 0, 0);
   print_line(":", 2, 0, 20);
   print_line("Hour", 1, 0, 30);
   print_line(":", 2, 0, 50);
   print_line("Min", 1, 0, 60);
   print_line(":", 2, 0, 80);
   print_line("Sec", 1, 0, 90);

   print_line(String(days), 2, 15, 0);
   print_line(String(hours), 2, 15, 30);
   print_line(String(minutes), 2, 15, 60);
   print_line(String(seconds), 2, 15, 90);

  display.display();
}

void ring_alarm(){
  display.clearDisplay();
  print_line("Medicine Time",2,0,0);
  
  digitalWrite(LED_ALARM, HIGH);

  while (digitalRead(CANCEL)==HIGH){
    for (int i=0; i< n_notes; i++){
      if (digitalRead(CANCEL)==LOW){
        display.clearDisplay();
        print_line("Alarm stopped",1,32,0);
        delay(200);
        digitalWrite(LED_ALARM, LOW);
        break;
      }
      else if (digitalRead(OK)==LOW){
        display.clearDisplay();
        print_line("Alarm snoozed for 5 mins",1,32,0);
        delay(5*60*1000);
      }
      tone(BUZZER, notes[i]);
      delay(500);
      noTone(BUZZER);
      delay(200);
    }
  }
  int UTC_Offset = (UTC_OFFSET[0]*60*60) + (UTC_OFFSET[1]*60);
  configTime(UTC_Offset, UTC_OFFSET_DST, NTP_SERVER);
}

const char* wait_for_button_press(){
  while (true){
    if (digitalRead(UP) == 0){
      delay(200);
      delay(200);
      return "UP";
    }
    else if (digitalRead(DOWN) == 0){
      delay(200);
      delay(200);
      return "DOWN";
    }
    else if (digitalRead(CANCEL) == 0){
      delay(200);
      delay(200);
      return "CANCEL";
    }
    else if (digitalRead(OK) == 0){
      delay(200);
      delay(200);
      return "OK";
    }
    update_time();
  }
}

void set_time() {
    int temp_hour = hours;
    while (true) {
        display.clearDisplay();
        print_line("Enter hour: " + String(temp_hour), 0, 0, 2);
        
        String pressed = wait_for_button_press();
        if (pressed == "UP") {
            delay(200);
            temp_hour += 1;
            temp_hour = temp_hour % 24;
        }
        
        else if (pressed == "DOWN") {
            delay(200);
            temp_hour -= 1;
            if (temp_hour < 0) {
                temp_hour = 23;
            }
        }
        
        else if (pressed == "OK") {
            delay(200);
            hours = temp_hour;
            break;
        }
        
        else if (pressed =="CANCEL") {
            delay(200);
            break;
        }
    }
int temp_minute = minutes;
while (true) {
    display.clearDisplay();
    print_line("Enter minute: " + String(temp_minute), 0, 0, 2);
    String pressed = wait_for_button_press();
    if (pressed == "UP") {
        delay(200);
        temp_minute += 1;
        temp_minute = temp_minute % 60;
    }
    
    else if (pressed == "DOWN") {
        delay(200);
        temp_minute -= 1;
        if (temp_minute < 0) {
            temp_minute = 59;
        }
    }
    
    else if (pressed == "OK") {
        delay(200);
        minutes = temp_minute;
        break;
    }
    
    else if (pressed == "CANCEL") {
        delay(200);
        break;
    }
}
display.clearDisplay();
print_line("Time is set", 0, 0, 2);
delay(1000);
}

void set_timezone(){
  while (true){
    display.clearDisplay();
    print_line("Enter hour from UTC:" + String(UTC_OFFSET[0]),0,0,2);

    String pressed = wait_for_button_press();
    if (pressed == "UP"){
      delay(200);
      UTC_OFFSET[0] +=1;
      UTC_OFFSET[0]= UTC_OFFSET[0] % 24;
    }
    else if (pressed == "DOWN"){
        delay(200);
        UTC_OFFSET[0] -= 1;
        if (UTC_OFFSET[0]<0){
          UTC_OFFSET[0]=23;
        }
    }
    else if (pressed == "OK"){
        delay(200);
        break;
    }
    else if (pressed == "CANCEL"){
        delay(200);
        break;
    }
  }
  while (true){
    display.clearDisplay();
    print_line("Enter minutes from UTC:" + String(UTC_OFFSET[1]), 0, 0,2);

    String pressed = wait_for_button_press();
    if (pressed == "UP"){
      delay(200);
      UTC_OFFSET[1] +=1;
      UTC_OFFSET[1] = UTC_OFFSET[1] % 60;
    }
    else if (pressed == "DOWN"){
      delay(200);
      UTC_OFFSET[1] -= 1;
      if (UTC_OFFSET[1]<0){
        UTC_OFFSET[1]=59;
      }
    }
    else if (pressed == "OK"){
      delay(200);
      UTC_OFFSET[1] = (UTC_OFFSET[1]);
      break;
    }

    else if (pressed == "CANCEL"){
      delay(200);
      break;
    }
  }
  int UTC_Offset = (UTC_OFFSET[0]*60*60) + (UTC_OFFSET[1]*60);
  display.clearDisplay();
  print_line("Enter the direction from UTC", 0, 0, 2);
  print_line("+ up", 0, 16, 2);
  print_line("- Down", 0, 24, 2);
  String pressed = wait_for_button_press();
  if (pressed == "DOWN") {
    UTC_Offset *= -1;
  }
  
  configTime(UTC_Offset, UTC_OFFSET_DST, NTP_SERVER);
  delay(2000);
  display.clearDisplay();
  print_line("Time is set", 0, 0, 2);
  delay(1000);
}

void active_alarm(){
  display.clearDisplay();
  for (int i=0; i<n_alarms;i++){
    if (alarm_active[i]==true){
      print_line("Alarm " + String(i+1) , 1, 0, 2);
      print_line( String(alarm_hours[i]) +":"+ String(alarm_minutes[i]), 1, 9, 0);
      delay(2000);
    }
  }
  if (n_alarms == 0) {
    print_line("No alarms were set", 1, 20, 0);
    delay(2000);
  }
}

void set_alarm(int alarm){
  alarm -=1;
  alarm_active[alarm] = true;
  alarm_triggered[alarm] = false;
  alarm_enabled=true;
  int temp_hour = alarm_hours[alarm];
  while (true){
    display.clearDisplay();
    print_line("Enter hour: " + String(temp_hour), 1, 0, 2);

    String pressed = wait_for_button_press();
    if (pressed == "UP") {
      delay(200);
      temp_hour +=1;
      temp_hour = temp_hour % 24;
    }
    else if (pressed == "DOWN"){
      delay(200);
      temp_hour -=1;
      if (temp_hour < 0) {
        temp_hour =23;
      }
    }
    else if (pressed == "OK"){
      delay(200);
      alarm_hours[alarm] = temp_hour;
      break;
    }
    else if (pressed == "CANCEL"){
      delay(200);
      break;
    }
  }
  int temp_minute = alarm_minutes[alarm];
  while (true){
    display.clearDisplay();
    print_line("Enter minute: " + String(temp_minute),1,0,2);
    String pressed = wait_for_button_press();
    if (pressed == "UP"){
      delay(200);
      temp_minute +=1;
      temp_minute = temp_minute % 60;
    }
    else if (pressed == "DOWN"){
      delay(200);
      temp_minute -= 1;
      if (temp_minute<0){
           temp_minute=59;
      }
    }
    else if (pressed == "OK"){
      delay(200);
      alarm_minutes[alarm] = temp_minute;
      break;
    }
    else if (pressed == "CANCEL"){
      delay(200);
      break;
    }
  }
  display.clearDisplay();
  print_line("Alarm is set", 1, 0, 2);
  print_line("Alarm " + String(alarm+1) +"  "+ String(alarm_hours[alarm]) +":"+ String(alarm_minutes[alarm]), 1, 8, 0);
  delay(2000);
  int UTC_Offset = (UTC_OFFSET[0]*60*60) + (UTC_OFFSET[1]*60);
  configTime(UTC_Offset, UTC_OFFSET_DST, NTP_SERVER);
}

void delete_alarm(int alarm){
  alarm_active[alarm-1]=false;
  
  display.clearDisplay();
  print_line(String(alarm) +" is removed", 2, 4, 0);
  
}
void run_mode(int mode){
  if (mode ==0){
    display.clearDisplay();
    print_line("Press UP:rmv Alrm-1",1,16,0);
    print_line("Press DOWN:rmv Alrm-2",1,24,0);
    String pressed2= wait_for_button_press();
    if (pressed2=="UP"){
      delete_alarm(1);
    }
    else if (pressed2=="DOWN"){
      delete_alarm(2);
}
  }

  else if (mode == 1){
    set_time();
   
  }
  else if (mode ==2){
     set_timezone();
   
  }
  else if (mode ==3){
    display.clearDisplay();
    print_line("Press UP:set Alarm-1",1,0,0);
    print_line("Press Down:set Alarm-2",1,8,0);
    String pressed1= wait_for_button_press();
    if (pressed1=="UP"){
      set_alarm(1);
      alarm_enabled = true;
    }
    else if (pressed1=="DOWN"){
      set_alarm(2);
      alarm_enabled = true;
    }


  else if (mode == 4){
  if (alarm_active[0] == true){
       active_alarm();
    }
    else if (alarm_active[1] == true){
      active_alarm();
    }
    else{
      display.clearDisplay();
      print_line("No alarms are active",1,0,0);
    }
}
}
}
void go_to_menu(){
  while (digitalRead(CANCEL)==0){
    display.clearDisplay();
    for (int i=0; i<6;i++){
      print_line("1-set Time",1,0,0);
      print_line("2-set Timezone",1,8,0);
      print_line("3-set Alarm",1,16,0);
      print_line("4-active Alarm",1,24,0);
      print_line("5-delete Alarm",1,32,0);
      delay(200);
    }
    if (alarm_enabled == true){
      delay(500);
    }
    String pressed= wait_for_button_press();
    display.clearDisplay();
    while(pressed != "OK" & (pressed == "UP" | pressed == "DOWN")){
      if (pressed=="UP"){
        current_mode +=1;
        current_mode %= max_modes;
      }
      else if (pressed == "DOWN"){
        current_mode -= 1;
        if (current_mode < 0){
          current_mode = max_modes -1;
        }
        
      }
      if ( current_mode == 0){
        display.clearDisplay();
        print_line(String(modes[4]) ,1,0,0);
        delay(200);
      }
      else{
        display.clearDisplay();
        print_line(String(modes[current_mode-1]) ,1,0,0);
        delay(200);
      }
      display.clearDisplay();
      pressed = wait_for_button_press();
      delay(200);
    }
    Serial.println(current_mode);
    delay(200);
    run_mode(current_mode);
    
  }
}



void check_temp(void){
  TempAndHumidity data = dhtSensor.getTempAndHumidity();
  bool all_good = true;
  if (data.temperature > 32){
    all_good = false;
    digitalWrite(LED_2, HIGH);
    print_line("TEMP HIGH", 1, 40, 0);
  }
  else if(data.temperature < 24){
    all_good = false;
    digitalWrite(LED_2,HIGH);
    print_line("TEMP LOW", 1, 40, 0);
  }
  if(data.humidity > 80){
    all_good = false;
    digitalWrite(LED_2,HIGH);
    print_line("HUMD HIGH", 1, 50, 0);
  }
  else if(data.humidity < 65){
    all_good = false;
    digitalWrite(LED_2,HIGH);
    print_line("HUMP LOW", 1, 50, 0);
  }
  if (all_good){
    digitalWrite(LED_2, LOW);
  }
}
void update_time_with_check_alarm () {
  display.clearDisplay();
  update_time();
  print_time_now();
  bool alarm_enabled = false;

  if (alarm_active[0]==true){
    alarm_enabled = true;
  }
  else if (alarm_active[1]==true){
    alarm_enabled = true;
  }
  if (alarm_enabled ==  true){
    delay(500);
  }
  if (alarm_enabled ==  true){
    for (int i=0; i<n_alarms; i++){
      if (alarm_triggered[i] ==false & alarm_active[i] ==true & hours == alarm_hours[i] & minutes==alarm_minutes[i]){
        ring_alarm();
        alarm_triggered[i] = true;
        alarm_active[i] = false;
      }
    }
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(LED_ALARM,OUTPUT);
  pinMode(BUZZER,OUTPUT);
  pinMode(LED_2,OUTPUT);
  pinMode(CANCEL ,INPUT);
  pinMode(UP ,INPUT);
  pinMode(DOWN ,INPUT);
  pinMode(OK ,INPUT);
  std::cout << "connected";
  dhtSensor.setup(DHT, DHTesp::DHT22);
  servoMotor.setPeriodHertz(50);       // Standard servo PWM frequency
  servoMotor.attach(servoPin, 500, 2400);  // Pin, min/max pulse width in µs
  servoMotor.write(0); 
  std::cout << "connected";
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDERSS)){
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.display();
  delay(2000);

  display.clearDisplay();
  print_line("Welcome to Medibox", 2, 0, 0);
  delay(3000);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin("Wokwi-GUEST");
  while (WiFi.status() != WL_CONNECTED){
    delay(250);
    Serial.print(".");
    display.clearDisplay();
    print_line("Connecting to WiFi",2,0,0);
    std::cout<< "connected";
  }
  display.clearDisplay();
  print_line("Connected to wifi",2 ,0, 0);
  int UTC_Offset=UTC_OFFSET[0];
  configTime(UTC_Offset, UTC_OFFSET_DST, NTP_SERVER);
  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
 
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setCallback(receiveCallback);
  connectToBroker();
  lightIntensity();
  
}

void loop() {
   update_time_with_check_alarm();
  if (digitalRead(CANCEL) == LOW){
    delay(1000);
    Serial.println("Menu");
    go_to_menu();
  }
  if (!mqttClient.connected()){
    connectToBroker();
  }
  check_temp();
  
  mqttClient.loop();
   if (!mqttClient.connected()) {
    connectToBroker();
  }
  mqttClient.loop();
  updateTemperature();
  Serial.println(tempAr);
  mqttClient.publish("Temperature", tempAr);
  delay(1000);

  updateHumidity();
  Serial.println(humAr);
  mqttClient.publish("Humidity", humAr);
  delay(1000);

  servoMotor.write(ServoAngle);
  if (SamplingInterval<=(millis()/1000)-sec_sample){
      sec_sample = millis()/1000;
      lightIntensity();
      
  }
  
  if (SendingInterval<=(millis()/1000)-sec_send){
      sec_send = millis()/1000;
      light_intensity /= n;
      n=0;
      mqttClient.publish("Light intensity", String(light_intensity).c_str());

  } 
}
void receiveCallback(char* topic, byte* payload, unsigned int length){
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  char payloadCharAr[length];
  for(int i=0 ; i <length; i++){
    Serial.println((char)payload[i]);
    payloadCharAr[i]=(char)payload[i];

  }
  Serial.println();
  if(strcmp(topic,"Sampling_Period")==0){
     SamplingInterval = atof(payloadCharAr);
  }
  else if(strcmp(topic,"Sending_Period")==0){
    SendingInterval = atof(payloadCharAr);
  }  
  else if (strcmp(topic, "Publish_Servo_Angle") == 0) {
    ServoAngle = atof(payloadCharAr);
  }
}



void connectToBroker(){
  while(!mqttClient.connected()){
    Serial.println("Attempting MQTT connection...");
    display.clearDisplay();
    print_line("Attempting mqtt",2,0,0);
    delay(3000);
    if(mqttClient.connect("ESP32-463517108394")){
      Serial.println("connected");
      print_line("Connected to mqtt",2,0,0);
      delay(3000);
      display.clearDisplay();
      print_line("Connecting to subscribe",2,0,0);
      mqttClient.subscribe("Sampling_Period");
      mqttClient.subscribe("Sending_Period");
      mqttClient.subscribe("Publish_Servo_Angle");
      mqttClient.subscribe("MAIN-ON-OFF");
      
    }
    else{
      display.clearDisplay();
      print_line("failed mqtt",2,0,0);
      Serial.print("Failed to connect MQTT");
      Serial.print(mqttClient.state());
      delay(5000);
    }
  }
}

void lightIntensity(){
  float I = analogRead(ldr)*1.00;
  light_intensity += 1-(I/4096.00);
  n+=1;

}

void updateTemperature() {
  TempAndHumidity data = dhtSensor.getTempAndHumidity();
  if (isnan(data.temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    strcpy(tempAr, "NaN");
  } else {
    String(data.temperature, 2).toCharArray(tempAr, 6);
  }
}

void updateHumidity() {
  TempAndHumidity data = dhtSensor.getTempAndHumidity();
  if (isnan(data.humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    strcpy(humAr, "NaN");
  } else {
    String(data.humidity, 2).toCharArray(humAr, 6);
  }
}
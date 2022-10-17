//Required Library
#include <WiFi.h> //Required library for wifi connection
#include <WiFiManager.h>  //Required library for wifi management
#include <ArduinoJson.h> //Sometimes required for compiling
#include <FirebaseArduino.h> //Required library for firebase connection

//Firebase auth token
#define firebase_host_url "home-automation-8a1b6-default-rtdb.firebaseio.com"
#define firebase_secrect_code "SalajObYERHXNDWqMuCi524cjzpk6QgAT1JQtIVw"

//Some  PIN Defination
#define error_led D0
#define wifi_connected_led D1
#define firebase_status_led D2
#define switch1 D4
#define switch2 D3
#define pushbutton1 D5
#define pushbutton2 D6
#define resetbutton D7




//---------------------Function for Led Management------------------------------------//

void led_status(int error, int wifi, int firebase){
  digitalWrite(error_led, error);
  digitalWrite(wifi_connected_led, wifi);
  digitalWrite(firebase_status_led, firebase);
}

//Create wifimanager object
WiFiManager wifiManager;

//Function for establish connection with Wifi
void wifi_connection(){
  led_status(1, 0, 0);
  wifiManager.autoConnect("Home Automation");  
  Serial.println("Connected.....");
}
//Function for reset or erase saved AP from wifimanager
void reset_wifi(){
  wifiManager.resetSettings();
  led_status(1, 0, 0);
  wifi_connection();
}

//---------------------------Function for checking wifi connected or not--------------------------
void check_wifi(){
  if(WiFi.status() == WL_CONNECTED){
    digitalWrite(wifi_connected_led, 1);
    digitalWrite(error_led, 0);
  }
  else{
    digitalWrite(wifi_connected_led, 0);
    digitalWrite(error_led, 1);
  }
}

//------------Function for checking firebase is connected or not----------------//

void check_firebase(){
  String deviceStatus = Firebase.getString("deviceStatus");
  if(deviceStatus == "100"){
    Firebase.setInt("deviceStatus",200);
  }
  int firebase_status;
  if(Firebase.failed()){
    Serial.println(Firebase.error());
    firebase_status = 200;
  }
  
  Firebase.setInt("onlineStatus",onlineStatus);
  onlineStatus++;
  if(onlineStatus==10){
    onlineStatus = 1;
  }
  
  firebase_status = Firebase.getInt("okStatus");
  if(firebase_status == 100){
    Serial.println("Successfully connected with Firebase.");
    digitalWrite(firebase_status_led, 1);
    digitalWrite(error_led, 0);
  }
  else{
    Serial.println("Failed to connect Firebase.");
    digitalWrite(firebase_status_led, 0);
    digitalWrite(error_led, 1);
  }
}

//-------------------Function for checking firebase switch status and send feedback for App---------------------------//

void check_online_switch(){
  String feedback1 = Firebase.getString("feedback1");
  String feedback2 = Firebase.getString("feedback2");
  
  String sw1 = Firebase.getString("switch1");
  if(sw1 == "1"){
    digitalWrite(switch1, 0);//Active low relay 1 on
  }
  else{
    digitalWrite(switch1, 1);//Active low relay 1 off
  }
  if(feedback1 == "100"){
    Firebase.setInt("feedback1",200);
  }

  String sw2 = Firebase.getString("switch2");
  if(sw2 == "1"){
    digitalWrite(switch2, 0);//Active low relay
  }
  else{
    digitalWrite(switch2, 1);//Active low relay
  }
  if(feedback2 == "100"){
    Firebase.setInt("feedback2",200);
  }
}

//Function for checking pushbutton and send switch and feedback status to firebase
void check_physical_switch(){
  if(digitalRead(pushbutton1) == 0){
    String sw1 = Firebase.getString("switch1");
    if(sw1 == "1"){
      Firebase.setString("switch1", "0");
    }else{
      Firebase.setString("switch1", "1");
    }
    Firebase.setString("feedback1", "200");
  }
  if(digitalRead(pushbutton2) == 0){
    String sw2 = Firebase.getString("switch2");
    if(sw2 == "1"){
      Firebase.setString("switch2", "0");
    }else{
      Firebase.setString("switch2", "1");
    }
    Firebase.setString("feedback2", "200");
  }
  //For reseting wifi
  if(digitalRead(resetbutton) == 0){
    reset_wifi();
  }
}


void setup(){
  //All pin mode setup
  pinMode(error_led, OUTPUT);
  pinMode(wifi_connected_led, OUTPUT);
  pinMode(firebase_status_led, OUTPUT);
  pinMode(switch1, OUTPUT);
  pinMode(switch2, OUTPUT);
  pinMode(pushbutton1, INPUT);
  pinMode(pushbutton2, INPUT);
  pinMode(resetbutton, INPUT);
  
  Serial.begin(115200);
  
  wifi_connection();
  Firebase.begin(firebase_host_url, firebase_secrect_code);
}

void loop(){
  check_wifi();
  check_firebase();
  check_online_switch();
  check_physical_switch();
}

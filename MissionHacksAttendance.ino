#include <SoftwareSerial.h>
#define trigPin 13
#define echoPin 12
int duration, distance; //to measure the distance and time taken 
int Buzzer = 7; // Connect buzzer pin to 8
int count = 0;
int totalcount = 10;
String voice; 
String ID[] = {"one", "two","three","for", "five"};


void test(){
      digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    distance = (duration/2) / 29.1;
  if (distance >= 20 || distance <= 0) 
        {
        Serial.println("no object detected");
        digitalWrite(Buzzer, HIGH);
        delay(1000);
        }
  else {
        count = (count + 1);
        Serial.println("object detected \n");
//        Serial.print("distance= ");              
//        Serial.print(distance );       
        Serial.println(count);
        digitalWrite(Buzzer,LOW);
        delay(1000);
  }
}

void setup() {
        Serial.begin (9600); 
        //Define the output and input objects(devices)
        pinMode(trigPin, OUTPUT); 
        pinMode(echoPin, INPUT);
        pinMode(Buzzer, OUTPUT);
}

void loop() {
test();
//  while(Serial.available()>0)
//  {
//    delay(10);
//    char c= Serial.read();
//    if(c=='#')
//    {
//      break;
//    }
//    voice+=c;
//  }
//    
//   if (voice == "hear"){
//    voice = "";
//    count = count+1;
//    Serial.println(count);
//    digitalWrite(Buzzer, LOW);
//    delay(1000);
//    Serial.println("buzzed");
//   }
//
//   if (count == totalcount ) {
//    exit(0);
//   }
}

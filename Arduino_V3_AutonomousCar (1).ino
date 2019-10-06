#include <SoftwareSerial.h>
#include <IRremote.h>


#define echoPin 13
#define trigPin 12
#define IN1  7   //K1、K2 motor direction
#define IN2  8     //K1、K2 motor direction
#define IN3  9    //K3、K4 motor direction
#define IN4  10   //K3、K4 motor direction
#define ENA  5    // Needs to be a PWM pin to be able to control motor speed ENA
#define ENB  6    // Needs to be a PWM pin to be able to control motor speed ENA
#define IR_PIN    11 //IR receiver Signal pin connect to Arduino pin D2
IRrecv IR(IR_PIN);  //   IRrecv object  IR get code from IR remoter
decode_results IRresults;
#define LFSensor_1 2 //line follow sensor1
#define LFSensor_2 3 //line follow sensor2
char sensor[2]={0,0};
#define IR_ADVANCE       0x00FF18E7       //code from IR controller "▲" button
#define IR_BACK          0x00FF4AB5       //code from IR controller "▼" button
#define IR_RIGHT         0x00FF5AA5       //code from IR controller ">" button
#define IR_LEFT          0x00FF10EF       //code from IR controller "<" button
#define IR_LINEFOLLOW 0x00FF38C7       //code from IR controller "OK" button
// #define IR_LINEFOLLOW 0x00FFB04F       //code from IR controller "#" button
#define IR_STOP               0x00FF9867       //code from IR controller "0" button
String voice;
float duration, distance;
String string, conca;
int x;

enum DN
{
 GO_ADVANCE, //go forward
 GO_LEFT, //left turn
 GO_RIGHT,//right turn
 GO_BACK,//backward
 STOP_STOP,
 START_LINEFOLLOW,//start line follow
 STOP_LINEFOLLOW,//stop line follow
 DEF
}Drive_Num=DEF;

bool stopFlag = true;//set stop flag
bool JogFlag = false;
uint16_t JogTimeCnt = 0;
uint32_t JogTime=0;
uint8_t motor_update_flag = 0;



/*motor control*/
void go_ahead()  //motor rotate clockwise -->robot go ahead
{
 digitalWrite(IN1, HIGH);
 digitalWrite(IN2, LOW);
 digitalWrite(IN3, HIGH);
 digitalWrite(IN4,LOW);
}
void go_back(int t) //motor rotate counterclockwise -->robot go back
{
 digitalWrite(IN1, LOW);
 digitalWrite(IN2, HIGH);
 digitalWrite(IN3, LOW);
 digitalWrite(IN4,HIGH);
 delay(t);
}
void go_stop() //motor brake -->robot stop
{
 digitalWrite(IN1, LOW);
 digitalWrite(IN2, LOW);
 digitalWrite(IN3, LOW);
 digitalWrite(IN4,LOW);
}
void turn_right(int t)  //left motor rotate clockwise and right motor rotate counterclockwise -->robot turn right
{
 digitalWrite(IN1, LOW);
 digitalWrite(IN2, HIGH);
 digitalWrite(IN3, HIGH);
 digitalWrite(IN4, LOW);
 delay(t);
}
void turn_left(int t) //left motor rotate counterclockwise and right motor rotate clockwise -->robot turn left
{
 digitalWrite(IN1, HIGH);
 digitalWrite(IN2, LOW);
 digitalWrite(IN3, LOW);
 digitalWrite(IN4, HIGH);
 delay(t);
}
/*set motor speed */
void set_motorspeed(int lspeed,int rspeed) //change motor speed
{
 analogWrite(ENA,lspeed);//lspeed:0-255
 analogWrite(ENB,rspeed);//rspeed:0-255
}

void do_IR_Tick()
{
 if(IR.decode(&IRresults))
 {
   if(IRresults.value==IR_ADVANCE)
   {
     Drive_Num=GO_ADVANCE;
   }
   else if(IRresults.value==IR_RIGHT)
   {
      Drive_Num=GO_RIGHT;
   }
   else if(IRresults.value==IR_LEFT)
   {
      Drive_Num=GO_LEFT;
   }
   else if(IRresults.value==IR_BACK)
   {
       Drive_Num=GO_BACK;
   }
   else if(IRresults.value==IR_STOP)
   {
       Drive_Num=STOP_STOP;
   }
   else if(IRresults.value==IR_LINEFOLLOW)
    {
      Drive_Num=START_LINEFOLLOW;
    }
    else if(IRresults.value==IR_STOP)
    {
       Drive_Num=STOP_LINEFOLLOW;
    }
   IRresults.value = 0;
   IR.resume();
 }
}

void read_sensor_values()
{
  sensor[0]=digitalRead(LFSensor_1);
  sensor[1]=digitalRead(LFSensor_2);
}
void auto_tarcking(){
  read_sensor_values();
  if((sensor[0]==LOW)&&(sensor[1]==HIGH)){ //The right sensor is on the black line.The left sensor is on the white line
    set_motorspeed(250,250);
    turn_right(20);
  }
  else if((sensor[0]==HIGH)&&(sensor[1]==LOW)){//The right sensor is on the white line.The left sensor is on the black line
    set_motorspeed(250,250);
    turn_left(20);
  }
  else if((sensor[0]==LOW)&&(sensor[1]==LOW)){//The left an right sensor are on the white line.
    set_motorspeed(250,250);
    go_ahead();
  }
  else if((sensor[0]==HIGH)&&(sensor[1]==HIGH)){//The left an right sensor are on the black line.
    go_stop();
  }
 }

void do_Drive_Tick()
{
   switch (Drive_Num)
   {
//      case GO_ADVANCE:go_ahead();JogFlag = true;JogTimeCnt = 1;JogTime=millis();break;//if GO_ADVANCE code is detected, then go advance
     case GO_LEFT: turn_left(450);JogFlag = true;JogTimeCnt = 1;JogTime=millis();break;//if GO_LEFT code is detected, then turn left
     case GO_RIGHT:  turn_right(450);JogFlag = true;JogTimeCnt = 1;JogTime=millis();break;//if GO_RIGHT code is detected, then turn right
     case GO_BACK: go_back(2000);JogFlag = true;JogTimeCnt = 1;JogTime=millis();break;//if GO_BACK code is detected, then backward
     case STOP_STOP: go_stop();JogTime = 0;break;//stop
     default:break;
   }
   Drive_Num=DEF;
  //keep current moving mode for  200 millis seconds
   if(millis()-JogTime>=200)
   {
     JogTime=millis();
     if(JogFlag == true)
     {
       stopFlag = false;
       if(JogTimeCnt <= 0)
       {
         JogFlag = false; stopFlag = true;
       }
       JogTimeCnt--;
     }
     if(stopFlag == true)
     {
       JogTimeCnt=0;
       go_stop();
     }
   }
}
void setup() {
 /******L298N******/
 Serial.begin(9600);
 pinMode(LFSensor_1,INPUT);
  pinMode(LFSensor_2,INPUT);
 pinMode(IN1, OUTPUT);
 pinMode(IN2, OUTPUT);
 pinMode(IN3, OUTPUT);
 pinMode(IN4, OUTPUT);
 pinMode(ENA, OUTPUT);
 pinMode(ENB, OUTPUT);
 pinMode(trigPin, OUTPUT);
 pinMode(echoPin, INPUT);
 pinMode(IR_PIN, INPUT);
 digitalWrite(IR_PIN, HIGH);
 IR.enableIRIn();
 set_motorspeed(120,120);
}
void object_move(){
 distancefunc();
 if (distance <=20 && distance >= 1) {
 digitalWrite(IN1, LOW);
 digitalWrite(IN2, LOW);
 digitalWrite(IN3, LOW);
 digitalWrite(IN4,LOW);
     delay(500);
     Serial.println("stopping");
   digitalWrite(IN1, HIGH);
   digitalWrite(IN2, LOW);
   digitalWrite(IN3, LOW);
   digitalWrite(IN4, HIGH);
   delay(1000);
   Serial.println("turning");
 digitalWrite(IN1, LOW);
 digitalWrite(IN2, LOW);
 digitalWrite(IN3, LOW);
 digitalWrite(IN4,LOW);
     delay(600);
     Serial.println("stopping");
   digitalWrite(IN1, HIGH);
   digitalWrite(IN2, LOW);
   digitalWrite(IN3, HIGH);
   digitalWrite(IN4, LOW);
    Serial.println("forward");
 }

}

void distancefunc() {
 duration = pulseIn(echoPin, HIGH);
 distance = (duration / 2) * 0.0344;
 string = "Obst dist:";
 conca = string + distance;
// for (x = 0; x<=3; x++){
   Serial.println(distance);
   if (distance <=20) {
 digitalWrite(IN1, LOW);
 digitalWrite(IN2, LOW);
 digitalWrite(IN3, LOW);
 digitalWrite(IN4,LOW);
   }

//}
}
void loop() {
   go_ahead();
//   Serial.println("In object move\n");
   object_move();
while(Serial.available()>0)
 {
   delay(10);
   char c=Serial.read();
   if(c=='#')
   {
     break;
   }
   voice+=c;
   }
  if(voice=="back"){
   digitalWrite(IN1, LOW);
   digitalWrite(IN2, HIGH);
   digitalWrite(IN3, LOW);
   digitalWrite(IN4, HIGH);
    delay(5000);
   }
 else
   if(voice=="forward"){
   digitalWrite(IN1, HIGH);
   digitalWrite(IN2, LOW);
   digitalWrite(IN3, HIGH);
   digitalWrite(IN4, LOW);
  object_move();
   }
   else
   if(voice=="stop"){
   digitalWrite(IN1, LOW);
   digitalWrite(IN2, LOW);
   digitalWrite(IN3, LOW);
   digitalWrite(IN4, LOW);
  object_move();
   }
 else
  if(voice=="left"){
   digitalWrite(IN1, LOW);
   digitalWrite(IN2, HIGH);
   digitalWrite(IN3, HIGH);
   digitalWrite(IN4, LOW);
   delay(200);
   }
 else
  if(voice=="right"){
   digitalWrite(IN1, HIGH);
   digitalWrite(IN2, LOW);
   digitalWrite(IN3, LOW);
   digitalWrite(IN4, HIGH);
   delay(200);   }
  if(voice.length()>0)
   {
     Serial.println(voice);
      voice="";
   digitalWrite(IN1, LOW);
   digitalWrite(IN2, LOW);
   digitalWrite(IN3, LOW);
   digitalWrite(IN4, LOW);
   }
//    object_move();
 digitalWrite(trigPin, LOW);
//    object_move();
 delayMicroseconds(2);
//    object_move();
 digitalWrite(trigPin, HIGH);
 delayMicroseconds(10);
//    object_move();
 digitalWrite(trigPin, LOW);
//  object_move();
//  distance_func();
 do_IR_Tick();
 do_Drive_Tick();
 if(Drive_Num == START_LINEFOLLOW)
     auto_tarcking();
   else if(Drive_Num == STOP_LINEFOLLOW)
     go_stop();

}

#include <Servo.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <VL53L0X.h>


#define I2C_SLAVE_ADDRESS 8 // Slave address 

struct VisionData {
  float yaw_angle;
  float linear_distance;
  int green_y;
  int green_width;
  int red_y;
  int red_width;
};

VisionData visionData; // Global instance of the data structure

Servo myservo;
const int l = 2;
const int f = 1;
const int r = 0;
double Fdist = 300, Ldist = 300, Rdist = 300;

const int ENA = 2; // Digital pins for ENA2(puente H)
const int IN1 = 3; // Digital pins for IN1(puente H)
const int IN2 = 4; // Digital pins for IN2(puente H)

VL53L0X sensors[3]; //Number of distance sensors I have
const int xshutPins[] = {5, 6, 7}; //Digital pins for each sensor right-f-left
const uint8_t addresses[] = {0x30, 0x31, 0x32};  // Define unique I2C addresses for each sensor. Make sure these addresses do not conflict with other I2C devices

const int keyPin = 9; //the number of the key pin 9

char side = ' ';
short corners = 0;
int servo_direction = 0;
int actual_Dist= 0;
bool run = false;

void stop () //stop motors
{
  digitalWrite(IN2, LOW);
  digitalWrite(IN1, LOW);
}

void move (int speed) //Move forward and backward
{
  digitalWrite(IN2, LOW);
  digitalWrite(IN1, LOW);
  analogWrite(ENA,  abs(speed));
  if (speed >= 0)
      digitalWrite(IN1,  HIGH);
  else
      digitalWrite(IN2,  HIGH);
}

double dist (int space) // looks at the sensor distance
{
  return sensors[space].readRangeContinuousMillimeters();
}

 void turn(char opts)
{
  corners++;
  myservo.write(opts=='l'? 130: 50);
  /*if (opts == 'l')
     myservo.write(130);
  else if (opts == 'r') 
     myservo.write(50);*/
  //Serial.println(corners);
  delay(700);
}

void turn()
{
  corners++;
  move(100);
  myservo.write(servo_direction);
  delay((corners == 1? 1000: 700)); //check time
  myservo.write(90);
  delay(1000);
  //while (dist(side == 'l'? l : r) > 400);
  stop();
}

void pre_side_def()
{
  side =' ';
  move(100);
  while (side == ' ')
  {

   Ldist = dist(l);
   Rdist = dist(r);
   Fdist = dist(f);
   Serial.println("L");
   Serial.println(Ldist);
   Serial.println(Rdist);
   Serial.println(Fdist);
   delay(500);

      if ((Ldist > 1000) || (Rdist > 1000))
      {
            stop();
            side = (Ldist > 1000? 'l': 'r');
            servo_direction = (side == 'l'? 140: 40);
      }
  }
}

void non_free_round()
{
  corners = 0;
  while (1)
  {
    if (corners >= 12)
      break;
    move(50);
    Wire.requestFrom(I2C_SLAVE_ADDRESS, sizeof(VisionData));

    // Check if the slave sent the correct number of bytes
    if (Wire.available() == sizeof(VisionData)) {
      // Read the incoming bytes directly into the struct
      Wire.readBytes((byte*)&visionData, sizeof(VisionData));
      Fdist = visionData.linear_distance;
      if (Fdist < 600)
      {
        if (side != ' ')
            turn();
        else
        {
           Ldist = dist(l);
           Rdist = dist(r);
           stop();
           side = (Ldist > 1000? 'l': 'r');
           turn();
        }
      }
      else if (visionData.red_width >visionData.green_width)
        {

        }
      else if (visionData.red_width < visionData.green_width)
        {

        }  
      
    } else {
      Serial.println("Error: Did not receive expected number of bytes from slave.");
    }

    // Wait before the next request
    delay(250);
  }
}

void free_round()
{
  corners = 0;
  //int actual_distance = 0;
  while(1)
  {
    turn();
    if (corners < 12)
    {
      move (100);
      if (side == 'l')
      {
        actual_Dist = dist(l);
        while (actual_Dist < 1000)
        {
          actual_Dist = dist(l);
          if (actual_Dist < 200)  
            myservo.write(40);
          else if (actual_Dist > 300)
            myservo.write(140);
          delay (250);
          myservo.write(90);
        }
      }
      else
      {
        actual_Dist = dist(r);
        while (actual_Dist < 1000)
        {
          actual_Dist = dist(r);
          if (actual_Dist < 200)  
            myservo.write(140);
          else if (actual_Dist > 300)
            myservo.write(40);
          delay (250);
          myservo.write(90);
        }
      }
    }
    else
    {
      delay(500);
      stop();
      break;
    }
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin (9600);
  pinMode (IN1, OUTPUT);
  pinMode (IN2, OUTPUT);
  pinMode (ENA, OUTPUT);

  myservo.attach(8);//attachs the servo on pin 9 to servo object
  myservo.write(90);//back to 0 degrees
  delay(1000);//wait for a second
  
  pinMode(keyPin,INPUT);//initialize the key pin as inp

  Wire.begin();  //Serial.begin(115200);
   // Initialize each sensor
  for (int i = 0; i < sizeof(sensors) / sizeof(sensors[0]); i++) {
    pinMode(xshutPins[i], OUTPUT);
    digitalWrite(xshutPins[i], LOW);  // Keep the sensor in reset state
  }

  // Set up each sensor with a unique I2C address
  for (int i = 0; i < sizeof(sensors) / sizeof(sensors[0]); i++) {
    digitalWrite(xshutPins[i], HIGH);  // Bring the sensor out of reset
    delay(10);  // Allow time for the sensor to initialize

    sensors[i].init();
    sensors[i].setTimeout(500);
    sensors[i].setAddress(addresses[i]);
  }

  for (int i = 0; i < sizeof(sensors) / sizeof(sensors[0]); i++) {
  sensors[i].startContinuous();
  }
}


void loop() {
  /*
  myservo.write(90);//back to 0 degrees
  delay(10000);
  myservo.write(50);//back to 0 degrees
  delay(2000);
  myservo.write(130);//back to 0 degrees
  delay(2000);*/
  stop();

  while (!run)    // starting button
  {
   // run = DigitalRead(bpin    
   
    if(digitalRead(keyPin) ==HIGH )
      {
        stop();
        run = true; //validate the starting button
      }
      Serial.write("\nWaiting button...");
      stop();
    }

  while(0)
   {
   Ldist = dist(l);
   Rdist = dist(r);
   Fdist = dist(f);
   Serial.println("L");
   Serial.println(Ldist);
   Serial.println(Rdist);
   Serial.println(Fdist);
   delay(500);
   //break;
   }

  pre_side_def();
  free_round();
  run = false;

}

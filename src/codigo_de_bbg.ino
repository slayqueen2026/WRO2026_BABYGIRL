#include <Servo.h>
//#include <SoftwareSerial.h>
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
const int l = 2; //left pov
const int f = 1; // front pov
const int r = 0; // right pov
double Fdist = 300, Ldist = 300, Rdist = 300; // random values

//servo reference
const int CENTER_ANGLE  = 83;  //91
const int MIN_ANGLE     = 68;     //-15
const int MAX_ANGLE     = 98;    //+15

VL53L0X sensors[3]; //Number of distance sensors I have
const int xshutPins[]     = {5, 6, 7}; //Digital pins for each sensor r - f - l
const uint8_t addresses[] = {0x30, 0x31, 0x32};  // Define unique I2C addresses for each sensor. Make sure these addresses do not conflict with other I2C devices

const int keyPin = 9; //pin 9 for button

//L298 Control pins
const int ENA = 10;  // PWM speed
const int IN1 = 4;   // Direction A
const int IN2 = 3;   // Direction B

const int DEFAULT_SPEED = 255;//180;

char  side        = ' '; //para decir hacia adonde va la vuelta
short corners     = 0; // variable for coner counting
int   actual_Dist = 0;
bool  run         = false;

void stop() {
  analogWrite(ENA, 0);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
}

// + = forward, - = backward
void move(int speed, int duration_ms) {
  //speed = constrain(speed, -255, 255);

  if (speed > 0) {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
  } else if (speed < 0) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
  } else {
    stop();
    return;
  }

  analogWrite(ENA, abs(speed));
  delay(duration_ms);
}

double dist (int space) // looks at the sensor distance
{
  return sensors[space].readRangeContinuousMillimeters();
}

 void turn() // turn logic
{
  corners++;

  myservo.write(side=='l'? MIN_ANGLE: MAX_ANGLE);
  Serial.print("Corner :");
  Serial.println (corners);
  move(DEFAULT_SPEED, 600);
  delay(450);
  myservo.write(CENTER_ANGLE);
  while (dist(side == 'l'? l: r) > 500)
    {
    delay (50);
    continue;
    }
  Serial.println ("OUT corner ");
}

int calculateP(double current_distance, char side) {
    // Tuning values will be different here because they aren't scaled by time
    const double setpoint = 185.0;

    double error = setpoint - current_distance;

    int target_angle = CENTER_ANGLE;
    if (side == 'l')
      target_angle += (int)(error/setpoint * 15);
    else
      target_angle -= (int)(error/setpoint * 15);
    return constrain(target_angle, MIN_ANGLE, MAX_ANGLE);
    /*// Constrain
    if (target_angle < MIN_ANGLE) target_angle = MIN_ANGLE;
    if (target_angle > MAX_ANGLE) target_angle = MAX_ANGLE;
    return target_angle;*/
}

void pre_side_def() // tells you to which side you have to turn
{
  side =' ';
  while (side == ' ')
  {
   move(DEFAULT_SPEED, 10);
   Ldist = dist(l);
   Rdist = dist(r);
   Fdist = dist(f);
   Serial.println("LRF");
   Serial.println(Ldist);
   Serial.println(Rdist);
   Serial.println(Fdist);
      if ((Ldist > 1000) || (Rdist > 1000))
      {
            stop();
            side = (Ldist > 1000? 'l': 'r');
            Serial.println(side);
      }
  }
}

void free_round() //Stop instead of turn free round
{
  corners = 0;

  while (corners < 12)
  {
    move(DEFAULT_SPEED, 20);

    actual_Dist = (side == 'l') ? dist(l) : dist(r);

    // Control PID normal

    if (actual_Dist > 1000)
      {
        stop();
        turn();
      }  
    else
      {
        int servoPos = calculateP(actual_Dist, side);
        myservo.write(servoPos);
        //Serial.println(servoPos);
      }
  }
  stop();
}

void non_free_round()
{
  corners = 0;
  while (1)
  {
    if (corners >= 12)
    { //logic for parallel parking

      break;
    }
     
    move(DEFAULT_SPEED, 20);
    Wire.requestFrom(I2C_SLAVE_ADDRESS, sizeof(VisionData));

    // Check if the slave sent the correct number of bytes
    if (Wire.available() == sizeof(VisionData)) {
      // Read the incoming bytes directly into the struct
      Wire.readBytes((byte*)&visionData, sizeof(VisionData));
      Fdist = visionData.linear_distance;
      if (Fdist < 600)  //validate for middle section while turning.
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





void setup()
{
  // put your setup code here, to run once:
  Serial.begin (9600);
 

  myservo.attach(8);//attachs the servo on pin 8 to servo object
  myservo.write(CENTER_ANGLE);//back to center

  delay(1000);//wait for a second
 
  pinMode(keyPin,INPUT);//initialize the key pin as input

  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  stop();

  Wire.begin();  //Serial.begin(115200);

  short s_count = sizeof(sensors) / sizeof(sensors[0]);
   // Initialize each sensor
  for (int i = 0; i < s_count; i++) {
    pinMode(xshutPins[i], OUTPUT);
    digitalWrite(xshutPins[i], LOW);  // Keep the sensor in reset state
  }

  // Set up each sensor with a unique I2C address
  for (int i = 0; i < s_count ; i++) {
    digitalWrite(xshutPins[i], HIGH);  // Bring the sensor out of reset
    delay(10);  // Allow time for the sensor to initialize

    sensors[i].init();
    sensors[i].setTimeout(500);
    sensors[i].setAddress(addresses[i]);
  }

  for (int i = 0; i < s_count; i++) {
  sensors[i].startContinuous();
  }
  run = true;   //true -> false
}


void loop()
{
  stop();
 
  while (!run)    // starting button
  {
 
    if(digitalRead(keyPin) == LOW )  
      {
        stop();
        run = true; //validate the starting button
        break;
      }
    else
      run = false;
     
    Serial.write("\nWaiting button...");
    delay(500);
  }
 
    while (false)
    {
    Ldist = dist(l);
    Rdist = dist(r);
    Fdist = dist(f);
    Serial.println("LRF");
    Serial.println(Ldist);
    Serial.println(Rdist);
    Serial.println(Fdist);
    delay(1000);
    }

    pre_side_def();

    Fdist = 400; //dist(f);
    if (Fdist < 100) {
      non_free_round();
    } else {
      free_round();
    }
    run = false;

}

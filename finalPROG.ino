#include <Keypad.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(15, 16);//Rx,Tx
const int PIR_SENSOR_OUTPUT_PIN = 4;  /* PIR sensor O/P pin */
int warm_up=1;
int piezoPin = 17;
const byte ROWS = 4; //four rows
const byte COLS = 3; //four columns
char hexaKeys[ROWS][COLS] = {
 {'1','2','3'},
 {'4','5','6'},
 {'7','8','9'},
 {'*','0','#'}
};
 
byte rowPins[ROWS] = {12, 13, 14,18}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {11, 6, 5}; //connect to the column pinouts of the keypad
 int attempt=0;
    boolean shouldDisplay = true;
//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
int pir_output,sensor_output;
void setup()
{
  pinMode(PIR_SENSOR_OUTPUT_PIN, INPUT);
  //pinMode(switch1,INPUT);
  pinMode(piezoPin,OUTPUT);
  mySerial.begin(9600); 
Serial.begin(9600);
delay(100);
}

void loop()
{
    Serial.println(" Start ");
    //delay(1000);  
    my_keypad(); 
    pir_output = pir_sensor();
    if(pir_output == 1)
    {
      Buzzer_turn_on();
      call_through_GSM();
    }  
}
void SendMessage(String message)
{
  mySerial.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);  // Delay of 1000 milli seconds or 1 second
  mySerial.println("AT+CMGS=\"+918805094320\"\r"); // Replace x with mobile number
  delay(1000);
  mySerial.println(message);// The SMS text you want to send
  delay(100);
   mySerial.println((char)26);// ASCII code of CTRL+Z
  delay(1000);
}

int pir_sensor()
{
  sensor_output = digitalRead(PIR_SENSOR_OUTPUT_PIN);
    //delay(1000);
  if( sensor_output == LOW )
  {Serial.println("Sensor OP Low");
    if( warm_up == 1 )
     {
      Serial.print("Warming Up\n\n");
      warm_up = 0;
      delay(1000);
    }
    Serial.print("No object in sight\n\n");
    return 0;
  }
  else
  {
    Serial.print("Object detected\n\n");   
    return 1;
  }
}
void Buzzer_turn_on()
{ 
    tone(piezoPin, 1000, 500);
    delay(500);
}
void call_through_GSM()
{
    Serial.println("Calling through GSM Modem");
    //set the data rate for the SoftwareSerial port  
    mySerial.begin(9600);
    mySerial.println("ATD8805094320;"); // ATDxxxxxxxxxx; -- watch out here for semicolon at the end!!
    Serial.println("Called ATD8805094320");
    //delay(1000);
}
void my_keypad()
{
  char customKey = customKeypad.getKey();
  /*char secretKey;
  if(customKey!='*'){
    return;
  }*/
  if(shouldDisplay)
  {
  int digits=0;
  char attemptedStr[4] ;
  String str;
  String passwordStr = "1234"; 
  Serial.println("Enter 4 digit Passcode");
  for(digits=0;digits<4;){
  char customKey = customKeypad.getKey();
  if(customKey&&shouldDisplay)
    {
      attemptedStr[digits]=customKey;
      digits++;
    }  
  }
  str=String(attemptedStr);
  Serial.println(str);
 
  if(str.substring(0,4).equals(passwordStr))
  {
    //shouldDisplay=false;
    SendMessage("Correct Password! welcome home..");
    Serial.println("Correct Password! welcome home..");
  }
  else
  {
    //Serial.println(str);
    attempt++;
    Serial.println("Incorrect Password!");
  }
  if(attempt==3){
    SendMessage("Someone has entered incorrect password");    
    shouldDisplay=false;
    /*secretKey = customKeypad.getKey();
    if(secretKey == '#')
    {
      shouldDisplay = true;
    }*/
  }
}
}

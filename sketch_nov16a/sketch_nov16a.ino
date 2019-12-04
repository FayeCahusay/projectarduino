

//DoorLock
#include<Keypad.h>
#include<LiquidCrystal_I2C.h>
#include<EEPROM.h>
#include <SoftwareSerial.h>
#include <Wire.h>
SoftwareSerial mySerial(10,11);//SIM800L Tx & Rx is connected to Arduino #3 & #2
int attempts = 0;
int count = 0;

LiquidCrystal_I2C liquid_crystal_display(0x27,16,2 );


char password[4];
char initial_password[4],new_password[4];
int i=0;
int relay_pin = 7;
char key_pressed=0;
const byte rows = 4; 
const byte columns = 3; 
long random1;
char hexaKeys[rows][columns] = {
{'1','2','3'},
{'4','5','6'},
{'7','8','9'},
{'*','0','#'}
};

byte row_pins[rows] = {A0,A1,A2,A3};
byte column_pins[columns] = {4,3,2};   
Keypad keypad_key = Keypad( makeKeymap(hexaKeys), row_pins, column_pins, rows, columns);

void setup()

{
  Serial.begin(9600);
  randomSeed(analogRead(0));
 random1= random(1000, 3000);
//DoorLock
  pinMode(relay_pin, OUTPUT);
  LockedPosition(false);
  liquid_crystal_display.init();                      // initialize the lcd 
  liquid_crystal_display.backlight();

 

  liquid_crystal_display.print("BSIT 2-3");
  

  liquid_crystal_display.setCursor(0,1);

  liquid_crystal_display.print("Electronic Lock");

  delay(2000);

  liquid_crystal_display.clear();

  liquid_crystal_display.print("Enter Password");

  liquid_crystal_display.setCursor(0,1);

  initialpassword();
  
  Serial.begin(9600);
  
  mySerial.begin(9600);

  Serial.println("Initializing...");
  delay(1000);

//Testing AT Commands
  mySerial.println("AT"); //Once the handshake test is successful, it will back to OK
  updateSerial();
  mySerial.println("AT+CSQ"); //Signal quality test, value range is 0-31 , 31 is the best
  updateSerial();
  mySerial.println("AT+CCID"); //Read SIM information to confirm whether the SIM is plugged
  updateSerial();
  
 
  

 
}

void loop()

{
 
//DoorLock
  
  key_pressed = keypad_key.getKey();
  if(key_pressed == '*')
  {
    i = 3;
    LockedPosition(true); 
  }
  if(key_pressed=='#')

    change();

  if (key_pressed)

  {

    password[i++]=key_pressed;

    liquid_crystal_display.print(key_pressed);

   }else if(key_pressed)
   {
   random1= key_pressed;
   liquid_crystal_display.print(key_pressed);
      
   }
  if(i==4)

  {

    delay(200);

    for(int j=0;j<4;j++)
    initial_password[j]=EEPROM.read(j);

    if(!(strncmp(password, initial_password,4)))

    {

      liquid_crystal_display.clear();

      liquid_crystal_display.print("Pass Accepted");
      digitalWrite(relay_pin,LOW);
      digitalWrite(12, HIGH);
      
      

      delay(2000);

      liquid_crystal_display.setCursor(0,1);
      

      liquid_crystal_display.print("Press # to change");

      delay(2000);

      liquid_crystal_display.clear();

      liquid_crystal_display.print("Enter Password:");

      liquid_crystal_display.setCursor(0,1);
       mySerial.println("AT+CREG?"); //Check whether it has registered in the network
      updateSerial();
     mySerial.println("AT+CMGF=1"); // Configuring TEXT mode
      updateSerial();
      mySerial.println("AT+CMGS=\"+639162424895\"");//change ZZ with country code and xxxxxxxxxxx with phone number to sms
      updateSerial();
      mySerial.print(random1); //text content
      updateSerial();
      mySerial.write(26);
      
     
      
   
    

      i=0;
      
    }

    else

    {
     
      digitalWrite(relay_pin, HIGH);
      digitalWrite(12, LOW);
      
      liquid_crystal_display.clear();

      liquid_crystal_display.print("Wrong Password");

      liquid_crystal_display.setCursor(0,1);

      liquid_crystal_display.print("Pres # to Change");

      delay(2000);

      liquid_crystal_display.clear();

      liquid_crystal_display.print("Enter Password");

      liquid_crystal_display.setCursor(0,1);

      i=0;
      attempts++;
      if (attempts == 3 ){
       //Sending SMS
      Serial.println("Initializing...");
      mySerial.println("AT+CMGF=1"); // Configuring TEXT mode
      updateSerial();
      mySerial.println("AT+CMGS=\"+639162424895\"");//change ZZ with country code and xxxxxxxxxxx with phone number to sms
      updateSerial();
      mySerial.print("May Nagbabalak Pumasok!"); //text content
      updateSerial();
      mySerial.write(26);
      attempts= 0;
      }
      
    }
  }
  //GSM Module
  updateSerial(); 

}

void change()

{

  int j=0;

  liquid_crystal_display.clear();

  liquid_crystal_display.print("Current Password");

  liquid_crystal_display.setCursor(0,1);

  while(j<4)

  {

    char key=keypad_key.getKey();

    if(key)

    {
      new_password[j++]=key;

      liquid_crystal_display.print(key);
    }

    key=0;
  }

  delay(500);




  if((strncmp(new_password, initial_password, 4)))

  {

    liquid_crystal_display.clear();

    liquid_crystal_display.print("Wrong Password");

    liquid_crystal_display.setCursor(0,1);

    liquid_crystal_display.print("Try Again");

    delay(1000);

  }

  else

  {

    j=0;

    liquid_crystal_display.clear();

    liquid_crystal_display.print("New Password:");

    liquid_crystal_display.setCursor(0,1);

    while(j<4)

    {

      char key=keypad_key.getKey();

      if(key)

      {

        initial_password[j]=key;

        liquid_crystal_display.print(key);

        EEPROM.write(j,key);

        j++;
      }

    }

    liquid_crystal_display.print("Pass Changed");

    delay(1000);

  }

  liquid_crystal_display.clear();

  liquid_crystal_display.print("Enter Password");

  liquid_crystal_display.setCursor(0,1);

  key_pressed=0;

}

void initialpassword()
{

  for(int j=0;j<4;j++)

    EEPROM.write(j, j+49);

  for(int j=0;j<4;j++)

    initial_password[j]=EEPROM.read(j);

}
void updateSerial()
{
  delay(500);
  while (Serial.available()) 
  {
    mySerial.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while(mySerial.available()) 
  {
    Serial.write(mySerial.read());//Forward what Software Serial received to Serial Port
  }
}
void LockedPosition(int locked)
{
  if(locked)
  {
    digitalWrite(relay_pin, LOW);
  }else
  {
    digitalWrite(relay_pin, HIGH);
  }
}
void randomizer()
{
   random1 = random(3000);
}

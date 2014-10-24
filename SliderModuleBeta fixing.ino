///midi slider slide module code with virtualwire wireless functionality
//nateohlson

#include <VirtualWire.h>

///set up int variables for one slider
int potVal1 = 0;
int prevPotVal1 = 0;
//pin for analog reading
int analogPin1 = 0;
int difference1 = 0;
int buttonState = 0;
int buttonPin = 4;
int ledPin = 5;

int id1 = 1112; //this is the section for the sliders unique id assignment


char arrayMessage[7];
char id[4];

void setup()
{
  //set up virtual wire transfer rate. default 2000. experiment with higher rates
  vw_setup(2000);
  Serial.begin(9600);
  pinMode(buttonPin, INPUT); //pin for reading button
  pinMode(ledPin, OUTPUT); //pin for led
}

void loop()
{
  //read the value of the pot from the designated analog pin
  potVal1 = analogRead(analogPin1);

  //adjust values for a 1-127 range
  potVal1 = (potVal1 - 11)/ 8.05511811024;
  potVal1 = potVal1 + 1;
  ////////
  
  //check to make sure the values have changed
  difference1 = abs(potVal1 - prevPotVal1);
  buttonState = digitalRead(buttonPin); 
  if (buttonState == HIGH)
  {
    while (buttonState == HIGH)
    {
    digitalWrite(ledPin, HIGH); 
    itoa(id1, id, 10);
    sendId(id);
    delay(500);
    digitalWrite(ledPin, LOW); 
    delay(500);
    buttonState = digitalRead(buttonPin); 
    }
  digitalWrite(ledPin, LOW); 
  }
  if (difference1 >= 1)//<<<change accuracy of pot reading
  {
    sprintf(arrayMessage, "%04d%03d", id1, potVal1);
    send(arrayMessage);
    prevPotVal1 = potVal1;
  }
}


//function to send the message
void send(char *message)
{

  vw_send((uint8_t *)message, 7);
  
  
  /////////////////////////
  for (int k = 0; k < 7; k++)
  {
    Serial.write(message[k]);
  }
  Serial.println();
///////////////////////////
  vw_wait_tx();
}

void sendId(char *id)
{
  vw_send((uint8_t *)id, 4);
  for (int t = 0; t < 4; t++)
  {
    Serial.write(id[t]);
  }
  Serial.println();
  
  vw_wait_tx();
}

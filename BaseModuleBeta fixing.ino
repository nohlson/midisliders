///midi slider base module code with virtualwire wireless functionality
//nateohlson
//a slider id that is not connected is 0

#include <VirtualWire.h>
#include <EEPROM.h>
//create char array for incoming message
byte message[VW_MAX_MESSAGE_LEN];
byte messageLength = VW_MAX_MESSAGE_LEN;
byte sliderId[VW_MAX_MESSAGE_LEN];
//variable for midi command
int command = 176;
//variable for midi control
int control1 = 16;
//variable for slider value
int potVal1 = 0;
String strNum;
String strSlid;
int rawSliders[16];
int connectedSliders[4];
int buttonPin = 4;
int redledPin = 3;
int greenledPin = 2;
int buttonState = 0;
int resetPin = 8;





void loadSliders() //reads EEPROM memory and puts values into rawsliders array then converts those numbers to complete ints in conslide array
{
	for (int i = 0; i < 16; i++)
	{
		rawSliders[i] = EEPROM.read(i);
	}
	connectedSliders[0] = (rawSliders[0] * 1000) + (rawSliders[1] * 100) + (rawSliders[2] * 10) + rawSliders[3];
	connectedSliders[1] = (rawSliders[4] * 1000) + (rawSliders[5] * 100) + (rawSliders[6] * 10) + rawSliders[7];
	connectedSliders[2] = (rawSliders[8] * 1000) + (rawSliders[9] * 100) + (rawSliders[10] * 10) + rawSliders[11];
	connectedSliders[3] = (rawSliders[12] * 1000) + (rawSliders[13] * 100) + (rawSliders[14] * 10) + rawSliders[15];
// 	for (int r = 0; r < 4; r++)
// {
// 	Serial.println(connectedSliders[r]);
// }
}

void resetAllSliders() //resets the slider ids in memory to 0000
{
	for(int i = 0; i < 16; i++)
	{
		EEPROM.write(i, 1);
	}
	digitalWrite(redledPin, HIGH);
	delay(3000);
	digitalWrite(redledPin, LOW);
	loadSliders();
	loop();
}



void newSlider() //connect a new slider and acquire new slider id
{
/////////////
	int i = 0;
	int newSlider = 0;
	int recordPosition;
	digitalWrite(greenledPin, LOW); 
	digitalWrite(redledPin, HIGH); 
	
////////////
	delay(1000);
	while (i < 1)
	{	
		//Serial.println("in loop");
		if (vw_have_message()) //if there is a message available
		{
			vw_get_message(sliderId, &messageLength);//the message is read
			strSlid = ""; //initialize the string
			for (int i = 0; i < messageLength ; i++)
			{
   
				char incomingId[messageLength]; //split the char array into different array
				incomingId[i] = char(sliderId[i]);//for some reason i decided to do this
				strSlid = strSlid + incomingId[i]; //create a string with the characters
				newSlider = 0;
				newSlider = strSlid.toInt();//convert the string to a usable int
			}



		}
		//Serial.println(newSlider)
		//enter break statement to kill loop if button pushed
		//enter code to blink light to show that module is scanning for sliders
		if (newSlider != 0)
		{
			digitalWrite(redledPin, LOW);
			digitalWrite(greenledPin, HIGH);
			delay(300);
			digitalWrite(greenledPin, LOW);
			delay(300);
			digitalWrite(greenledPin, HIGH);
			delay(300);
			digitalWrite(greenledPin, LOW);
			delay(300); 
			digitalWrite(greenledPin, HIGH);
			delay(300);
			digitalWrite(greenledPin, LOW);
			delay(300); 
			break; //if a new slider is found break the loop
		}
	}

	loadSliders(); //put all the current slider id's into
	

	for (int i = 0; i < 4; i++)
	{
		if( connectedSliders[i] == 1111)
		{
				connectedSliders[i] = newSlider; //if there is a vacant connected sliders slot then put the new id in
				recordPosition = i;
				 //Serial.println("Got to this point");
				 //Serial.println(newSlider);
				break;
				 //Serial.println(i);
		}


	}

	///////next section to take values in all connected sliders array and spread them back into rawsliders
	int temp = 0;
	int f = 0;

	for (int i = 0; i < 4; i++) //iterates through the 4 slider id arrays
	{
		int j = 3;
		if (connectedSliders[i] == 0)
		{
			for(int m = 3; m > 0; m++)
			{
				rawSliders[m + f] = 0;
			}
		}
		else
		{
			while (connectedSliders[i] > 0)
			{
				temp = connectedSliders[i] % 10; //for this section i must credit rodrigo anadon. he is a beast
				connectedSliders[i] /= 10;
				rawSliders[j + f] = temp;
				j--;
			
			
			}
		}
		f = f + 4;
	}
	for (int d = 0; d < 16; d++)
	{
		EEPROM.write(d, rawSliders[d]);
	}
	loadSliders();
}




void setup()
{
  //Begin serial communication for sending midi and vm communication for receiving
  //values
	Serial.begin(31250); //31250
	vw_setup(2000);
	vw_rx_start();
	loadSliders(); //load the connected sliders to the connected sliders array database
	pinMode(4, INPUT); //switch to search for connected sliders
	pinMode(2, OUTPUT); //green led
	pinMode(3, OUTPUT); //red led
	
}

void loop()
{
	if (vw_have_message()) //if there is a message available
	{
		vw_get_message(message, &messageLength);//the message is read
		strNum = ""; //initialize the string
		int messageId = 0;
		char incomingMessage[messageLength];
		for (int i = 4; i < 8 ; i++)
		{
			 //split the char array into different array
			incomingMessage[i] = char(message[i]);//for some reason i decided to do this
			strNum = strNum + incomingMessage[i]; //create a string with the characters
			potVal1 = strNum.toInt();//convert the string to a usable int
		}
		strNum = "";
		for (int l = 0; l < 4 ; l++)
		{
			 //split the char array into different array
			incomingMessage[l] = char(message[l]);//for some reason i decided to do this
			strNum = strNum + incomingMessage[l]; //create a string with the characters
			messageId = strNum.toInt();//convert the string to a usable int
		}
		//Serial.println(messageId);
		// Serial.println(connectedSliders[0]);
		if (messageId == connectedSliders[0])
		{
			midiSend(16, potVal1);
		}
		if (messageId == connectedSliders[1])
				midiSend(17, potVal1);
		if (messageId == connectedSliders[2])
				midiSend(18, potVal1);
		if (messageId == connectedSliders[3])
				midiSend(19, potVal1);
	}
	buttonState = digitalRead(buttonPin); 
	if (buttonState == HIGH)
	{
		newSlider();
	}
	// if (digitalRead(resetPin) == HIGH)
	// {
	// 	resetAllSliders();
	// }
}

//function for sending values via serial over midi
void midiSend(int control, int value)
{
  Serial.write(command);
  Serial.write(control);
  Serial.write(value);

}


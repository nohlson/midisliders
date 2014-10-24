#include <EEPROM.h>
void setup()
{
	Serial.begin(9600);
	
}

void loop()
{
	int j = 0;
        int z = 99;
        while (z == 99){
	if (j == 0)
	{
	for(int i = 0; i < 16; i++)
		{
			Serial.println(EEPROM.read(i));
			EEPROM.write(i, 1);
		}
		j = 1;
	}
        }
}

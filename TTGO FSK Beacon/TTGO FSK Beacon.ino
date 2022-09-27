
#define GREEN_LED	25

void setup(void)
{
	pinMode(GREEN_LED,OUTPUT);
	digitalWrite(GREEN_LED,LOW);
}

void loop(void)
{
	digitalWrite(GREEN_LED,HIGH);
	delay(50);
	digitalWrite(GREEN_LED,LOW);
	delay(50);
	digitalWrite(GREEN_LED,HIGH);
	delay(50);
	digitalWrite(GREEN_LED,LOW);
	delay(750);
}


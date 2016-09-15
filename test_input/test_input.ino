/*

*/


#define ESP8266_LED 2 // THIS IS CORRECT, HIGH == OFF
#define FLASH_BTN_PIN 0  // Correct gpio for flash button
#define READ_AND_RST 5 // This is D1


volatile int isr_state = HIGH;
static int isr_prev = LOW;


void setup()
{
Serial.begin(115200);
delay(5);
Serial.println("Setting up");
attachInterrupt(FLASH_BTN_PIN, btn_isr, FALLING);
pinMode(ESP8266_LED, OUTPUT); 
pinMode(READ_AND_RST, INPUT); 
}

void loop() 
{
int reading;

reading = digitalRead(READ_AND_RST);
Serial.print("Reading: ");
Serial.println(reading);
digitalWrite(ESP8266_LED, reading); 

if(isr_state != isr_prev)
    {
    Serial.println("Update LED");
    isr_prev = isr_state;
    reset_circuit();
    }


delay(800);
}


void btn_isr()
{
if( isr_state == HIGH )
    {
    isr_state = LOW;
    }
else
    {
    isr_state = HIGH;
    }
};

void reset_circuit()
{
pinMode(READ_AND_RST, OUTPUT);
delay(100);
digitalWrite(READ_AND_RST, LOW); 
delay(1000);
pinMode(READ_AND_RST, INPUT);
}



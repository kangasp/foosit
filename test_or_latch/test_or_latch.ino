/*

Setup
    Make a 

loop
    Read input pin, print value,
    On interrupt - flash button - Set the reset pin low for a second, then set hi-z




*/


#define ESP8266_LED 2 // THIS IS CORRECT, HIGH == OFF
#define FLASH_BTN_PIN 0  // Correct gpio for flash button
#define READ_AND_RST 5 // Don't know this

volatile int isr_state = 0;


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

if(isr_state != 0)
    {
    isr_state = 0;
    update_led();
    delay(500);
    reset_circuit();
    update_led();
    }

delay(600);
Serial.print(".");
}

void update_led()
{
int read_rst_state;
read_rst_state = digitalRead(READ_AND_RST);
Serial.println("isr");
Serial.print("Read state: ");
Serial.println(read_rst_state);
digitalWrite(ESP8266_LED, read_rst_state); 
}

void reset_circuit()
{
pinMode(READ_AND_RST, OUTPUT);
digitalWrite(READ_AND_RST, LOW); 
delay(500);
pinMode(READ_AND_RST, INPUT);
}


void btn_isr()
{
isr_state++;
};



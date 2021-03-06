#include <Bounce2.h>
#include <RF24.h>
#include <SPI.h>
#include <printf.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4 // not used / nicht genutzt bei diesem Display
Adafruit_SSD1306 display(OLED_RESET);

#define BUTTON_Break 6

#define PYRUN_PIPE_HOST 0xC0F0E0F0E1LL
#define PYRUN_PIPE_NODE 0xA0ECCCF0D2LL

RF24 m_radio( 9,10);

// Instantiate a Bounce object
Bounce debouncer_break = Bounce(); 

void setup() {
  // initialize with the I2C addr 0x3C / mit I2C-Adresse 0x3c initialisieren
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  
  // Show image buffer on the display hardware
  display.display();

  // radio set
  m_radio.begin();
  m_radio.enableDynamicPayloads();
  m_radio.setAutoAck( true ) ;
  
  // Optionally, increase the delay between retries & # of retries
  m_radio.setRetries(2,15);
  
  // set speed
  m_radio.setDataRate( RF24_250KBPS );
  m_radio.powerUp();

  // write pipe
  m_radio.openWritingPipe( PYRUN_PIPE_HOST);
  m_radio.openReadingPipe(1, PYRUN_PIPE_NODE);

  // set up
  digitalWrite(A0, INPUT_PULLUP);
  digitalWrite(A1, INPUT_PULLUP);
  digitalWrite(A2, INPUT_PULLUP);
  digitalWrite(A3, INPUT_PULLUP);

  // Setup the button with an internal pull-up
  pinMode(BUTTON_Break,INPUT_PULLUP);
 
  // After setting up the button, setup the Bounce instance
  debouncer_break.attach(BUTTON_Break);
  debouncer_break.interval(5); // interval in ms
  
  // initialize serial communications at 9600 bps:
  Serial.begin(250000);

  // printf begin
  printf_begin();
}

int leseKnopf( int port) {
    // read the analog in value
  int sensorValue = analogRead( port);
  if( sensorValue < 250)
    return 1;
  return 0;
}

char *ftoa(char *a, double f, int precision)
{
  long p[] = {0,10,100,1000,10000,100000,1000000,10000000,100000000};
  
  char *ret = a;
  long heiltal = (long)f;
  itoa(heiltal, a, 10);
  while (*a != '\0') a++;
  *a++ = '.';
  long desimal = abs((long)((f - heiltal) * p[precision]));
  itoa(desimal, a, 10);
  return ret;
}

char p_voltage[16];
char p_throttle[24];


int p_soll_throttle = 0;
int p_current = 0;
int p_temp = 20;
int p_temperature = 28;

void loop() {
  int l_a, l_b, l_x, l_y;
  int l_break;

  l_a = leseKnopf( A1);
  l_b = leseKnopf( A0);
  l_x = leseKnopf( A2);
  l_y = leseKnopf( A3);  
  
  // Update the Bounce instance :
  debouncer_break.update();

  l_break = debouncer_break.read()==1?0:1;

  char msg2[16];
  sprintf( msg2,"%d;%d;%d;%d;%d;%d;", l_a, l_b, l_x, l_y, 0, l_break);
  m_radio.stopListening();
  m_radio.write( &msg2, strlen(msg2));
  m_radio.startListening();
  // falls was empfangen
  if( m_radio.available() ) {
    char t_message[64];
    int t_len = m_radio.getDynamicPayloadSize();
    
    // auslesen
    m_radio.read( &t_message, t_len);
    // auslesen
    Serial.print( "Empfange...");
    
    // split
    int t_voltage = atoi(strtok( t_message, ";" ));
    float voltage = t_voltage * (5.0 / 1023.0)*7.78;
    p_current = atoi(strtok( NULL, ";" ));
    p_soll_throttle = atoi(strtok( NULL, ";" ));
    p_temperature = atoi(strtok( NULL, ";" ));

    char string_voltage[24];
    ftoa( string_voltage, voltage, 1);
    sprintf( p_voltage,"%sV", string_voltage);
    
    Serial.println( t_message);
  }

  
  // display
  display.clearDisplay();
  // set text color / Textfarbe setzen
  display.setTextColor(WHITE);
  display.setTextSize(1);
  // set text cursor position / Textstartposition einstellen

  // temperature
  display.setTextSize(1);
  display.setCursor(60,0);
  display.println("Temperature");
  display.setTextSize(2);
  display.setCursor(80,10);
  display.println(p_temperature);
  display.setTextSize(2);
  display.setCursor(112,10);
  display.println("C");
  display.drawCircle(108, 12, 3, WHITE);
  
  // voltage
  display.setTextSize(1);
  display.setCursor(1,0);
  display.println("Voltage");
  display.setTextSize(2);
  display.setCursor(0,10);
  display.println(p_voltage);

  // throttle
  display.setTextSize(1);
  display.setCursor( 0,38);
  display.println( "Throttle i/s");
  display.setTextSize(2);
  display.setCursor( 0,48);
  sprintf( p_throttle,"%d/%d", p_current, p_soll_throttle);
  display.println( p_throttle);

  // credits
  display.setTextSize(1);
  display.setCursor( 90,44);
  display.println( "Pyrun");
  display.setCursor( 90,54);
  display.println( "@2017");
  
  // flip
  display.display();
    
  Serial.print(msg2);
  Serial.print( " Length: ");
  Serial.println( strlen(msg2));

  delay(2);
}

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
 
  m_radio.begin();
  m_radio.enableDynamicPayloads();
  m_radio.setAutoAck( true ) ;
  // Optionally, increase the delay between retries & # of retries
  //m_radio->setRetries(2,15);
  // set speed
  m_radio.setDataRate( RF24_250KBPS );
  m_radio.powerUp();

  m_radio.openWritingPipe( PYRUN_PIPE_HOST);
  m_radio.openReadingPipe(1, PYRUN_PIPE_NODE);

  digitalWrite(A0, INPUT_PULLUP);
  digitalWrite(A1, INPUT_PULLUP);
  digitalWrite(A2, INPUT_PULLUP);
  digitalWrite(A3, INPUT_PULLUP);

  // Setup the button with an internal pull-up :
  pinMode(BUTTON_Break,INPUT_PULLUP);
 
  // After setting up the button, setup the Bounce instance
  debouncer_break.attach(BUTTON_Break);
  debouncer_break.interval(5); // interval in ms
  
  // initialize serial communications at 9600 bps:
  Serial.begin(250000);
  
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
    char t_message[255];
    int t_len = m_radio.getDynamicPayloadSize();
    
    // auslesen
    m_radio.read( &t_message, t_len);
    // auslesen
    Serial.print( "Empfange...");
    // split
    int t_voltage = atoi(strtok( t_message, ";" ));
    float voltage = t_voltage * (5.0 / 1023.0)*7.78;
    int current = atoi(strtok( NULL, ";" ));
    int soll_throttle = atoi(strtok( NULL, ";" ));


    // display
    display.clearDisplay();
    // set text color / Textfarbe setzen
    display.setTextColor(WHITE);
    display.setTextSize(1);
    // set text cursor position / Textstartposition einstellen
    display.setCursor(1,0);
    // show text / Text anzeigen
    display.println("Pyrun - Longboard");


    
    
    // show text / Text anzeigen
    display.setCursor(0,8);
    char msg_voltage[24];
    char string_voltage[24];
    ftoa( string_voltage, voltage, 1);
    sprintf( msg_voltage,"Voltage: %sV", string_voltage);
    display.println(msg_voltage);

    display.setCursor(0,16);
    char msg_throttle[24];
    sprintf( msg_throttle,"Throttle: i%d/s%d", current, soll_throttle);
    display.println(msg_throttle);
    
    /*display.setCursor(1,24);
    char msg_length[24];
    sprintf( msg_length,"???Km");
    display.println(msg_length);*/
    display.display();
    
    Serial.println( t_message);
  }
  Serial.print(msg2);
  Serial.print( " Length: ");
  Serial.println( strlen(msg2));

  delay(2);
}

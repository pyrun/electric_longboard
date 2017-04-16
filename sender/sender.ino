#include <Bounce2.h>
#include <RF24.h>
#include <SPI.h>
#include <printf.h>

#define BUTTON_Break 6
#define BUTTON_Fire 7

#define PYRUN_PIPE_HOST 0xC0F0E0F0E1LL
#define PYRUN_PIPE_NODE 0xA0ECCCF0D2LL

RF24 m_radio( 9,10);

// Instantiate a Bounce object
Bounce debouncer_fire = Bounce(); 
Bounce debouncer_break = Bounce(); 

void setup() {

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
  pinMode(BUTTON_Fire,INPUT_PULLUP);
 
  // After setting up the button, setup the Bounce instance :
  debouncer_fire.attach(BUTTON_Fire);
  debouncer_fire.interval(5); // interval in ms
  debouncer_break.attach(BUTTON_Break);
  debouncer_break.interval(5); // interval in ms
  
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);
  
  printf_begin();
}

int leseKnopf( int port) {
    // read the analog in value
  int sensorValue = analogRead( port);
  if( sensorValue < 250)
    return 1;
  return 0;
}

void loop() {
  int l_a, l_b, l_x, l_y;
  int l_fire, l_break;

  l_a = leseKnopf( A1);
  l_b = leseKnopf( A0);
  l_x = leseKnopf( A2);
  l_y = leseKnopf( A3);

  
  
  // Update the Bounce instance :
  debouncer_break.update();
  debouncer_fire.update();

  l_break = debouncer_break.read()==1?0:1;
  l_fire = debouncer_fire.read()==1?0:1;

  char msg2[16];
  sprintf( msg2,"%d;%d;%d;%d;%d;%d;", l_a, l_b, l_x, l_y, l_fire, l_break);
  m_radio.stopListening();
  m_radio.write( &msg2, strlen(msg2));
  m_radio.startListening();

  Serial.print(msg2);
  Serial.print( " Length: ");
  Serial.println( strlen(msg2));
  
  delay(2);
}

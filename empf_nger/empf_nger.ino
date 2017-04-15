
/*
 * ArduinoNunchukDemo.ino
 *
 * Copyright 2011-2013 Gabriel Bianconi, http://www.gabrielbianconi.com/
 *
 * Project URL: http://www.gabrielbianconi.com/projects/arduinonunchuk/
 *
 */

#include <Wire.h>
#include <SPI.h>
#include <RF24.h>
#include <printf.h>
#include <Servo.h> 

#define BAUDRATE 250000

#define PYRUN_PIPE_HOST 0xC0F0E0F0E1LL
#define PYRUN_PIPE_NODE 0xA0ECCCF0D2LL


// ESC
Servo m_esc;

int m_escPin = 6;
int m_minP = 1000;
int m_maxP = 2000;
int m_waitTrottleUp = 20; // ms
int m_waitTrottleDown = 10; // ms

int m_sollThrottle = 0;

// Radio
RF24 m_radio( 9,10);

void setup()
{
  Serial.begin(BAUDRATE);

  m_radio.begin();
  m_radio.enableDynamicPayloads();
  m_radio.setAutoAck( true ) ;
  // Optionally, increase the delay between retries & # of retries
  //m_radio->setRetries(2,15);
  // set speed
  m_radio.setDataRate( RF24_250KBPS );
  m_radio.powerUp();

  m_radio.openWritingPipe( PYRUN_PIPE_NODE);
  m_radio.openReadingPipe(1, PYRUN_PIPE_HOST );

  // ESC anschließen und anfagns und end rate angeben
  m_esc.attach( m_escPin, m_minP, m_maxP); 

  // 0 schreiben damit der ESC in grundeinstellung ist
  m_esc.write(0);
  
  printf_begin();
}

void loop()
{
   // Warten auf input
  /*if (Serial.available() > 0) {
    
    // Zahl lesen
    int serial = Serial.parseInt();
    
    // ausgeben
    Serial.print("Throttle gesetzt auf: ");
    Serial.println(serial);

    m_sollThrottle = serial;
  }*/

  // aktuelen werd lesen
  int currentThrottle = m_esc.read();
  
  // enscheiden ob hoch oder runter geht
  int step = 1;
  if( m_sollThrottle < currentThrottle )
    step = -1;
  
  // langsam dem wert anpassen
  if( currentThrottle != m_sollThrottle ) {
    m_esc.write(currentThrottle + step);
    currentThrottle = m_esc.read();
    //Serial.println(currentThrottle);
    if( step == 1)
      delay(m_waitTrottleUp);
    else
      delay(m_waitTrottleDown);
  }
  /*char msg2[64];
  sprintf( msg2,"%d;%d;%d;%d;", nunchuk.analogY, nunchuk.analogX, nunchuk.zButton, nunchuk.cButton);
  m_radio.stopListening();
  m_radio.write( &msg2, strlen(msg2));
  m_radio.startListening();
  delay(10);*/

  m_radio.startListening();
  // falls was empfangen
  if( m_radio.available() ) {
    char t_message[255];
    int t_len = m_radio.getDynamicPayloadSize();
    
    // auslesen
    m_radio.read( &t_message, t_len);
  
    // split
    int achse_x;
    int achse_y;
    int b_fire;
    int b_break;
  
    achse_x = atoi(strtok( t_message, ";" ));
    achse_y = atoi(strtok( NULL, ";" ));
    b_fire = atoi(strtok( NULL, ";" ));
    b_break = atoi(strtok( NULL, ";" ));

    if( achse_x > 220 && b_break)
      m_sollThrottle++;
    else if( !b_break && m_sollThrottle > 0)
      m_sollThrottle--;
    Serial.println(m_sollThrottle);
  
    Serial.println( achse_x);
    Serial.println( achse_y);
    Serial.println( b_fire);
    Serial.println( b_break);
   
    m_radio.stopListening();
  }
}


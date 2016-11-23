
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

#define BAUDRATE 9600

#define PYRUN_PIPE_HOST 0xC0F0E0F0E1LL
#define PYRUN_PIPE_NODE 0xA0ECCCF0D2LL

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

  printf_begin();
}

void loop()
{
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
  Serial.println( t_message);
  m_radio.stopListening();
  }
}

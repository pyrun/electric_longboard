/*
 * Empfänger
 *
 * Project URL: https://github.com/pyrun/electric_longboard
 *
 */
#include <Wire.h>
#include <SPI.h>
#include <RF24.h>
#include <printf.h>
#include <Servo.h> 
#include "AnalogSmooth.h"

#define BAUDRATE 250000

#define PYRUN_PIPE_HOST 0xC0F0E0F0E1LL
#define PYRUN_PIPE_NODE 0xA0ECCCF0D2LL

// 59 = gang 4
// 57 = gang 3
// 55 = gang 2
// 50 = gang 1
// 45 = leerlauf
// 0 = bremse

int leerlauf = 45;

int voltage_pin = A1;

int sensorPin = 0;
AnalogSmooth temp_sensor = AnalogSmooth(20);

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
  
  delay( 1000); // 1 sekunde warten

  m_esc.write(1);

  delay( 2000); // 1 sekunde warten
   
  printf_begin();

  m_radio.startListening();
}

int watchdog = 0;
int step = 1;
int currentThrottle = 0;
int sensorValue = 0;

#define WATCHDOG_CIRCLES 1000

void loop()
{
  // watchdog number incress
  watchdog++;
  
  //getting the voltage reading from the temperature sensor
   float reading = temp_sensor.analogReadSmooth(sensorPin); // analogRead(sensorPin);  
   
   // converting that reading to voltage, for 3.3v arduino use 3.3
   float voltage = reading * 5.0;
   voltage /= 1024.0;
   
   // now print out the temperature
   float temperatureC = (voltage - 0.5) * 100 ;  //converting from 10 mv per degree wit 500 mV offset
                                                 //to degrees ((voltage - 500mV) times 100)
   // Warten auf input
  if (Serial.available() > 0) {
    // Zahl lesen
    int serial = Serial.parseInt();
    
    // setzte neue Throttle
    m_sollThrottle = serial;
  }

  // read the input pin
  sensorValue = analogRead( voltage_pin);    
  /*float voltage = sensorValue * (5.0 / 1023.0);
  Serial.println(voltage);             // debug value*/
  
  // aktuelen werd lesen
  currentThrottle = m_esc.read();
  
  // enscheiden ob hoch oder runter geht
  if( m_sollThrottle < currentThrottle )
    step = -1;
  else
    step = 1;
  
  // langsam dem wert anpassen
  if( currentThrottle != m_sollThrottle ) {
    m_esc.write(currentThrottle + step);
    currentThrottle = m_esc.read();
    // stepp up oder down
    if( step == 1)
      delay(m_waitTrottleUp);
    else
      delay(m_waitTrottleDown);
  }
  
  // falls was empfangen
  if( m_radio.available() ) {
    char t_message[255];
    int t_len = m_radio.getDynamicPayloadSize();
    
    // auslesen
    Serial.print( "Empfange...");
    m_radio.read( &t_message, t_len);

    // split
    int l_a;
    int l_b;
    int l_x;
    int l_y;
    int l_fire;
    int l_break;
    l_a = atoi(strtok( t_message, ";" ));
    l_b = atoi(strtok( NULL, ";" ));
    l_x = atoi(strtok( NULL, ";" ));
    l_y = atoi(strtok( NULL, ";" ));
    l_fire = atoi(strtok( NULL, ";" ));
    l_break = atoi(strtok( NULL, ";" ));

    Serial.print("Nachricht erhalten...Decode:");
    Serial.print( l_a); Serial.print( " ");
    Serial.print( l_b); Serial.print( " ");
    Serial.print( l_x); Serial.print( " ");
    Serial.print( l_y); Serial.print( " ");
    Serial.print( l_fire); Serial.print( " ");
    Serial.print( l_break); Serial.println( " ");

    // reset des watchdog
    watchdog = 0;

    // logic knöpfe -> uiuiui
    if( l_y == 1 && l_x == 1)
      m_sollThrottle++;
    else if( !l_x && m_sollThrottle > leerlauf)
      m_sollThrottle--;
    if( l_a)
      m_sollThrottle = m_sollThrottle/2;

    // senden des Status -> nachricht verfassen
    char msg2[16];
    char str_temp[6];
    dtostrf( temperatureC, 2, 1, str_temp);
    sprintf( msg2,"%d;%d;%d;%s;", sensorValue, currentThrottle, m_sollThrottle, str_temp);
    m_radio.stopListening();

    // senden der nachricht
    delay( 2); // nicht sofort sonst hängt sich das senden auf
    Serial.println( msg2);
    m_radio.write( &msg2, strlen(msg2));
  }
  // lauschen
  m_radio.startListening();

  // watchdog prüfen
  if( watchdog > WATCHDOG_CIRCLES) {
    if( m_sollThrottle >= leerlauf)
      m_sollThrottle = leerlauf;
    else
      m_sollThrottle = 0;
  }
}


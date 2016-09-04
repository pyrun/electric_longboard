#include <Servo.h> 

Servo m_esc;

int m_escPin = 9;
int m_minP = 1000;
int m_maxP = 2000;
int m_waitTrottleUp = 100; // ms
int m_waitTrottleDown = 50; // ms

int m_sollThrottle = 0;

void setup() {
  
  Serial.begin(9600);
  Serial.setTimeout(500);
  
  // ESC anschließen und anfagns und end rate angeben
  m_esc.attach( m_escPin, m_minP, m_maxP); 
  
  // 0 schreiben damit der ESC in grundeinstellung ist
  m_esc.write(0);
  
}

void loop() {

  // Warten auf input
  if (Serial.available() > 0) {
    
    // Zahl lesen
    int serial = Serial.parseInt();
    
    // ausgeben
    Serial.print("Throttle gesetzt auf: ");
    Serial.println(serial);

    m_sollThrottle = serial;
  }

  

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
    Serial.println(currentThrottle);
    if( step == 1)
      delay(m_waitTrottleUp);
    else
      delay(m_waitTrottleDown);
  }

}

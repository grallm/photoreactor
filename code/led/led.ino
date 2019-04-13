// Gestion de la LED

const
int CAL_LED_MIN = 0,
    CAL_LED_MAX = 1023, // Calibrage de led (0-100%)
    P_HPLED = 9; // Pin LED

// -- Fonctions LED de puissance --
void powLED(int puiss = 0){
  int ledPuiss = map(puiss, 0,100, CAL_LED_MIN,CAL_LED_MAX);
  analogWrite(P_HPLED,ledPuiss);
}
// ----------

void setup() {
  pinMode(P_HPLED, OUTPUT);
  powLED(100);
}

void loop() {
  
}

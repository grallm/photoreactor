// Contrôle ventilateur
// Paramètres
int ecart_temp = 1; // Ecart de température durant l'expérience avec la température de départ

// Constantes
const
int CAL_LUM_MIN = 0,
    CAL_LUM_MAX = 1023, // Calibrage Min et Max de la photoresistance
    CAL_LUM_ON = 1023, // Calibrage lorsque LED allumée (en %)
    CAL_VENTILO_MIN = 0,
    CAL_VENTILO_MAX = 180, // Calibrage Min et Max de la photoresistance
    VENTILO_PUISS_DEF = 10, // Vitesse ventilo lorsque température bonne pour garder température fixe
    // Pins
    P_VENTILO = 11,
    P_TEMP = A4, // Pin du capteur de température
    P_LUM = A5; // Capteur de lumière

// Variables utiles
float init_temp;

// -- Fonctions Ventilo --
// Allumer le ventilateur à une certaine puissance
void Ventilo(int puissance = 0){
  int puissInt = map(puissance, 0,100, CAL_VENTILO_MIN,CAL_VENTILO_MAX); // Puissance en pourcentage vers intensite
  analogWrite(P_VENTILO, puissInt);
}
// -------

void setup() {
  Serial.begin(9600);
  Serial.println("START");

  pinMode(P_VENTILO, OUTPUT);

  // Température initiale au démarrage
  analogRead(P_TEMP); // Lecture dans le vide car valeur inexacte
  int valTherm = analogRead(P_TEMP);
  // --- LM35DZ
  init_temp = valTherm * (5.0 / 1023.0 * 100.0);
  // ----------
  // --- TMP36
  //int t=map(valTherm,0,1023,0,5000);// Tension entre 0 et 5000 mV
  //init_temp = map(t,0,1750,-50,125); // Tension de 0 à 1750mV en température de -50°C à 125°C;
  // ----------
  Serial.println(init_temp);
}

void loop() {
  // --- Température - thermistor ---
  analogRead(P_TEMP); // Lecture dans le vide car valeur inexacte
  int valTherm = analogRead(P_TEMP);
  // --- LM35DZ
  float temp = valTherm * (5.0 / 1023.0 * 100.0); // Conversion en degrés Celsius
  // ----------
  // --- TMP36
  //int t=map(valTherm,0,1023,0,5000);// Tension entre 0 et 5000 mV
  //int temp = map(t,0,1750,-50,125); // Tension de 0 à 1750mV en température de -50°C à 125°C;
  // ----------
  Serial.println(temp);

  // Effets selon température:
  //  > temp init + écart -> 100% ventilateur
  //  > temp init -> 50% ventilateur
  //  < temp init -> 0% ventilateur
  //  bonne temp (+-0.5) -> quel % ? 10% ?
  if(temp > init_temp + ecart_temp){
    /*
     * TODO
     * - éteindre LED
     */
    Ventilo(100);
    
  }else if(temp > init_temp+.5 || temp < init_temp-.5){
    Ventilo(VENTILO_PUISS_DEF);
    
  }else if(temp > init_temp){
    Ventilo(50);
    
  }else if(temp < init_temp){
    Ventilo(0);
    
  }
  // ----------
  

  // --- Luminosité - photorésistance ---
  int valLum = analogRead(P_LUM);
  int lum = map(valLum, CAL_LUM_MIN, CAL_LUM_MAX, 0,100); // Eclairage en %
  Serial.println(lum);

  // Erreur si -10% CAL_LUM_ON
  if(lum < CAL_LUM_ON-10){
    /*
     * TODO
     * - ERREUR
     * - ERR si +10 % ?
     */
  }
  // ----------

  delay(500);
}

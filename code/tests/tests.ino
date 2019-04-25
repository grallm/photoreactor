// TESTS VENTILO
/*const short PIN = 9;

void setup() {
  Serial.begin(9600);
  Serial.println("START");
  pinMode(PIN,OUTPUT);
  analogWrite(PIN, 255);
}

void loop() {
}*/

// TESTS CAPTEURS
const short PIN = 9;
const int PIN_THERM = A2;
const int P_LUM = A3;

int CAL_LUM_MIN = 5,
    CAL_LUM_MAX = 213; // Calibrage Min et Max de la photoresistance slon LED

void setup(){
  Serial.begin(9600);
  Serial.println("START");
  pinMode(PIN,OUTPUT);
  analogWrite(PIN, 255);
}

void loop() {
  analogRead(PIN_THERM); // Lecture dans le vide car valeur inexacte
  int temp = analogRead(PIN_THERM);
  //Serial.println(valTherm);
  // --- LM35DZ
  //float temp = valTherm * (5.0 / 1023.0 * 100.0); // Conversion en degrés Celsius
  // ----------
  // --- TMP36
  Serial.println(temp);
  temp = map(temp,0,1023,0,5000);// Tension entre 0 et 5000 mV
  temp = map(temp,0,1750,-50,125); // Tension de 0 à 1750mV en température de -50°C à 125°C;
  Serial.println(temp);
  // ----------


  // --- Luminosité - photorésistance ---
  /*int valLum = analogRead(P_LUM);
  Serial.println(valLum);
  int lum = map(valLum, CAL_LUM_MIN, CAL_LUM_MAX, 0,100); // Eclairage en %
  Serial.println(lum);*/
  Serial.println();

  
  delay(1000);
}

// TESTS FONCTIONS TRANSFORMATION SECONDES
// -- Fonction générales --
// Transformer temps secondes en HH:MM:SS
/* String time_sec_toStr(unsigned long timeSec){
  unsigned long time[4];
  String timeStr[4];

  // Heures
  time[0] = timeSec/3600;
  timeSec -= 3600*time[0];
  timeStr[0] = String(time[0]);
  timeStr[0] = (time[0]<10)? "0"+timeStr[0] : timeStr[0]; // Rajouter 0 si <10

  // Minutes
  time[1] = timeSec/60;
  timeSec -= 60*time[1];
  timeStr[1] = String(time[1]);
  timeStr[1] = (time[1]<10)? "0"+timeStr[1] : timeStr[1];

  // Secondes
  time[2] = timeSec;
  timeStr[2] = String(time[2]);
  timeStr[2] = (time[2]<10)? "0"+timeStr[2] : timeStr[2];

  // Retourner 00:00:00
  return timeStr[0] +":"+ timeStr[1] +":"+ timeStr[2];
}

// Mettre le temps paramétré en secondes
unsigned long time_to_sec(int* time){
  unsigned long timeSec = 0;

  // Secondes
  timeSec += time[0];
  timeSec += 10*time[1];

  // Minutes
  timeSec += 60*time[2];
  timeSec += 10*60*time[3];

  // Heures
  timeSec += 3600*time[4];
  unsigned long temp_time = time[5]; // Dépassement de capacité
  timeSec += 10*(3600*temp_time);

  return timeSec;
}
// ----------------

void setup(){
  Serial.begin(9600);
  Serial.println("START");

  int test_sec_transf[7] = {0,0,0,1,0,0};
  Serial.println(String(test_sec_transf[5])+String(test_sec_transf[4]) +":"+ String(test_sec_transf[3])+String(test_sec_transf[2]) +":"+ String(test_sec_transf[1])+String(test_sec_transf[0]));
  Serial.println(time_to_sec(test_sec_transf));
  Serial.println(String(time_to_sec(test_sec_transf)-600));
}

void loop(){

} */


// TESTS CLIGNOTEMENT
/*const int LED_R = 6, // Pin LED encodeur rouge
    LED_G = 5; // Pin LED encodeur verte

short led_twinkle = 0; // Stocker PIN
bool TWINKLE_STATE = false; // Savoir si éteinte ou allumée
unsigned long last_twinkle = 0; // Stockage millis clignotement pour ne pas clignoter extêmement vite

void setup(){
  pinMode(LED_G,OUTPUT);
  pinMode(LED_R,OUTPUT);
  led_twinkle = LED_G;
}

void loop(){
  // Clignement LED
  if(led_twinkle!=0 && (millis()%500)==0 && (last_twinkle>millis()+5 || last_twinkle<millis()-5)){ // Si clignotement activé allumer ou éteindre selon étant toutes les 0.5s
    Serial.println(String(millis()));
    if(TWINKLE_STATE){
      digitalWrite(led_twinkle, LOW);
      TWINKLE_STATE = false;
      Serial.println("OFF");
    }else{
      digitalWrite(led_twinkle, HIGH);
      TWINKLE_STATE = true;
      Serial.println("ON");
    }
    last_twinkle = millis();
  }

}
*/

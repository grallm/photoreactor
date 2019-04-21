// TESTS VENTILO
void setup() {
  Serial.begin(9600);
  Serial.println("START");
  pinMode(11,OUTPUT);
  analogWrite(11, 255);
}

void loop() {
}

// TESTS THERMISTOR
/*
const int P_TEMP = A2;
const int P_LUM = A3;

int CAL_LUM_MIN = 5,
    CAL_LUM_MAX = 132; // Calibrage Min et Max de la photoresistance slon LED

void setup(){
  Serial.begin(19200);
  Serial.println("START");
}

void loop() {
  analogRead(P_TEMP); // Lecture dans le vide car valeur inexacte
  int valTherm = analogRead(P_TEMP);
  //Serial.println(valTherm);
  // --- LM35DZ
  //float temp = valTherm * (5.0 / 1023.0 * 100.0); // Conversion en degrés Celsius
  // ----------
  // --- TMP36
  int t=map(valTherm,0,1023,0,5000);// Tension entre 0 et 5000 mV
  int temp = map(t,0,1750,-50,125); // Tension de 0 à 1750mV en température de -50°C à 125°C;
  // ----------
  //Serial.println("Temp: " + temp);


  // --- Luminosité - photorésistance ---
  int valLum = analogRead(P_LUM);
  Serial.println(valLum);
  int lum = map(valLum, CAL_LUM_MIN, CAL_LUM_MAX, 0,100); // Eclairage en %
  Serial.println(lum);
  Serial.println();

  
  delay(1000);
}*/

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

}
 */

// TESTS VENTILO
void setup() {
  Serial.begin(9600);
  Serial.println("START");
  pinMode(11,OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  analogWrite(11, 255);
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

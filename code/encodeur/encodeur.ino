const int ENC_ROT_1 = 2, // Encodeur rotatif
    ENC_ROT_2 = 3, // Encodeur rotatif
    ENC_INTERRUPTOR = 4; // Interrupteur

// Localisation
String LOC = "home";

// Sélection
bool select = true; // Curseur activé
int CURSOR = 0; // Valeur sélecteur (0 = neutre)
int maxSelect = 3; // Valeur max de sélection (nombre de sélections possibles)

// Variables interrupteur
bool pushedPrev = true;
bool pushed = false; // Appuie 1 fois


// -- Fonctions et variables encodeur --
/* TODO
 * - faciliter select
 * - actions slect
 * - actions click
 */

// Variables rotation encodeur
volatile int lastEncoded = 0;
volatile long encoderValue = 0; // 1 tour = 98 crans
long lastencoderValue = 0;
int lastMSB = 0;
int lastLSB = 0;
int valEncodeur = 0; // Valeur utilisable (0-9, 10 crans)
int valEncodeurPrev = 0;



// Gestion du click et actions
void clickGestionary(){

}

// Gestion de la rotation et actions (1 rotation horaire/trigo = H/T)
void rotationGestionary(char rotDir){
  if(select){
    // Ajouter ou retirer 1 au curseur
    if(rotDir == 'H'){
      CURSOR = (CURSOR == maxSelect) ? 1 : CURSOR+1;
    }else if(rotDir == 'T'){
      CURSOR = (CURSOR == 1) ? maxSelect : CURSOR-1;
    }
  }
}

// Appelée lors de la rotation de l'encodeur
void updateEncoder(){
  int MSB = digitalRead(ENC_ROT_1); //MSB = most significant bit
  int LSB = digitalRead(ENC_ROT_2); //LSB = least significant bit
  
  int encoded = (MSB << 1) |LSB; //converting the 2 pin value to single number
  int sum = (lastEncoded << 2) | encoded; //adding it to the previous encoded value
  if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) encoderValue ++;
  if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) (encoderValue == 0) ? encoderValue+=99 : encoderValue--;
  lastEncoded = encoded; //store this value for next time
  
  encoderValue = encoderValue % 100; // Limiter valeurs 0-99
  valEncodeur = map(encoderValue, 0,100, 0,10); // Séparation rotation en 10 crans

  if(valEncodeur > valEncodeurPrev || (valEncodeur == 0 && valEncodeurPrev == 9)){ // Rotation sens horaire
    rotationGestionary('H');
  }else if(valEncodeur < valEncodeurPrev || (valEncodeur == 9 && valEncodeurPrev == 0)){ // Sens trigonométrique
    rotationGestionary('T');
  }

  valEncodeurPrev = valEncodeur;
}
// ----------------

void setup() {
  Serial.begin (9600);
  
  // ---- ENCODEUR ----
  // Activer l'interrupteur
  pinMode(ENC_INTERRUPTOR, INPUT_PULLUP);
  
  // Activer la détéction de la rotation de l'encodeur
  pinMode(ENC_ROT_1, INPUT_PULLUP);
  pinMode(ENC_ROT_2, INPUT_PULLUP);
  attachInterrupt(0, updateEncoder, CHANGE);
  attachInterrupt(1, updateEncoder, CHANGE);
  // ------------------
}

void loop(){
  bool pushed = !digitalRead(ENC_INTERRUPTOR);
  bool clicked = (pushed && !pushedPrev) ? true : false;  // Appuie 1 fois
  pushedPrev = pushed;

  if(clicked){
    Serial.println("Pushed");
  }
  Serial.println(CURSOR);
   
  //Serial.println(valEncodeur);
  delay(100); //just here to slow down the output, and show it will work even during a delay
}

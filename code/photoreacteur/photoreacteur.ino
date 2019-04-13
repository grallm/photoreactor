// Code photoréacteur 0.1
const String version = "0.1";
const String version_hardware = "1.0";
/*
 * - Menu(s) de lancement du fonctionnement
 * - Menu d'erreur
 * - Gestion des LEDs encodeur
 * - Gestion de la LED
 * - Gestion du CLICK -> fonction globale qui dirige (permet de généraliser la lecture)
 */

#include <Wire.h>
#include <I2C_LCD.h>

I2C_LCD LCD;
uint8_t I2C_LCD_ADDRESS = 0x51; // Adresse écran

const int luminosite = 100, // en %
    LED_R = 6, // Pin LED encodeur rouge
    LED_G = 5, // Pin LED encodeur verte
    ENC_ROT_1 = 2, // Encodeur rotatif
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



// -- Fonctions de formattage --
/*
 * TODO
 * - pouvoir aligner sur Y en haut et bas
 */
// Donne X pour placer texte centre, droite ou gauche (Gauche=L, Droite=R, Centre=C) (donner le texte et la hauteur des lettres)
int FF_placeX(String text, int fontX, String place = "C"){
  if(place=="C"){
    return 128/2-text.length()*fontX/2-1;
  }else if(place=="R"){
    return 128-text.length()*fontX-1;
  }else{
    return 0;
  }
}

// Donne Y pour centrer texte (Haut=T, Bas=B, Centre=C)
int FF_placeY(String text, int fontY, String place = "T"){
  if(place=="C"){
    return 64/2-fontY/2-1;
  }else if(place=="B"){
    return 63-fontY-1;
  }else{
    return 0;
  }
}
// ----------------


// -- Fonctions de menus --
int MF_spacing_title = 6; // Nombre de pixels entre titre et texte
int MF_spacing = 2; // Nombre de pixels entre chaque ligne
int MF_spacing_button = 2; // Nombre de pixels après bouton
int MF_pos[3] = {0,0}; // Position d'écriture dans les menus

bool refreshScreen = false; // Rafraichissement de l'écran

// Réinitialiser la position et nettoyer écran
void MF_reset(){
  LCD.CleanAll(WHITE);
  MF_pos[0]=0;
  MF_pos[1]=0;
  refreshScreen = false;
}

// Configurer les variables des LEDs selon Menu
void MF_leds(int led, bool twinkle = false){
  /*
   * TODO
   * - variables des LEDs et agir sur celles-ci
   */
}

// Titre d'un menu
void MF_title(String text){
  LCD.FontModeConf(Font_6x8, FM_ANL_AAA, BLACK_NO_BAC);
  
  LCD.DispStringAt(text.c_str(), FF_placeX(text,6), 0);
  
  MF_pos[0]=0;
  MF_pos[1]=8+MF_spacing_title;
}

// Ecrire du texte avec balises et params (placer texte X et Y)
void MF_text(String text, String place = "HL"){
  LCD.FontModeConf(Font_6x8, FM_ANL_AAA, BLACK_NO_BAC);

  int posX = 0;
  int posY = MF_pos[1];
  if(place=="BR"){ // Placer le texte
    posX = FF_placeX(text,6,"R");
    posY = FF_placeY(text,6,"B");
  }else if(place=="C"){
    posX = FF_placeX(text,6,"C");
  }
  
  LCD.DispStringAt(text.c_str(), posX, posY);
  
  MF_pos[0]=0;
  MF_pos[1]+=8+MF_spacing;
}

// Boutton avec cadre, noir si sélectionné, avec une action (redirection, éteindre)
void MF_button(String text, String action, bool select = false, bool center=true){
  /*
   * TODO
   * - actions
   */
  int posX=0;
  if(center){ // Centrer le texte
    posX = FF_placeX(text,6);
  }
  
  if(select){ // Si sélectionné carré rempli
    LCD.DrawRectangleAt(posX-2, MF_pos[1], 4+text.length()*6, 4+8, BLACK_FILL);
  }else{
    LCD.DrawRectangleAt(posX-2, MF_pos[1], 4+text.length()*6, 4+8, BLACK_NO_FILL);
  }
  
  if(select){ // Si sélectionné écrit en blanc
    LCD.FontModeConf(Font_6x8, FM_ANL_AAA, WHITE_NO_BAC);
  }else{
    LCD.FontModeConf(Font_6x8, FM_ANL_AAA, BLACK_NO_BAC);
  }

  LCD.DispStringAt(text.c_str(), posX, MF_pos[1]+2);

  MF_pos[0]=0;
  MF_pos[1]+=12+MF_spacing_button;
}
// ----------------


// -- Les menus --
// Menu principal avec quel bouton sélectionné (0=aucun, 1-3)
void M_Menu(int selected = 0){
  select = true;
  MF_leds(LED_G);
  MF_reset();
  MF_title("MENU");
  MF_button("Lancer Experience", "", (selected==1)? true:false);
  MF_button("Aide", "", (selected==2)? true:false);
  MF_button("Infos", "", (selected==3)? true:false);
  MF_text("v"+version, "BR");
}

// Sélection durée
/*
 * TODO
 * - curseur
 * - sélectionner valeur en tournant
 * - passe de 9 à 10
 * - changer de chiffre
 */
void M_Duration(int selected=1){
  select = true;
  MF_leds(LED_G);
  MF_reset();
  MF_title("Duree exp.");
}

// Informations sur le photoréacteur
void M_Infos(){
  MF_leds(LED_G);
  MF_reset();
  MF_title("INFORMATIONS");
  MF_text("Vers. logiciel: "+version);
  MF_text("Vers. prod.: "+version_hardware);
  MF_text("Cree par", "C");
  MF_text("Malo G., Theo B.", "C");
  MF_text("Moustafa K.", "C");
}

// Erreurs
/*
 * TODO
 * - lister différentes erreurs possibles
 * - explication pour chacune (concise)
 * - retour menu
 * - comment résoudre (concis)
 */
void M_Error(int errID=0){
  MF_leds(LED_R);
  MF_reset();
  MF_title("ERREUR");
}
// ----------------



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
    refreshScreen = true;
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
  Wire.begin();
  
  Serial.begin(9600);
  Serial.println("START");

  // ---- ENCODEUR ----
  // Activer l'interrupteur
  pinMode(ENC_INTERRUPTOR, INPUT_PULLUP);
  
  // Activer la détéction de la rotation de l'encodeur
  pinMode(ENC_ROT_1, INPUT_PULLUP);
  pinMode(ENC_ROT_2, INPUT_PULLUP);
  attachInterrupt(0, updateEncoder, CHANGE);
  attachInterrupt(1, updateEncoder, CHANGE);
  // ------------------

  // ---- ECRAN ----
  LCD.CleanAll(WHITE);
  LCD.BacklightConf(LOAD_TO_EEPROM,map(luminosite,0,100,0,127)); // Luminosité (0-127) sauvegardée au reboot
  LCD.WorkingModeConf(OFF, ON, WM_CharMode); // Pas LOGO, Rétro éclairage,

  M_Menu();
  // ------------------
}

void loop() {
  bool pushed = !digitalRead(ENC_INTERRUPTOR);
  bool clicked = (pushed && !pushedPrev) ? true : false;  // Appuie 1 fois
  pushedPrev = pushed;

  // Renvoie à la fonction de gestion de click lors d'un click
  if(clicked){
    clickGestionary();
  }

  // Rafraîchissement de l'écran
  if(refreshScreen){
    if(LOC == "home"){
      Serial.println(CURSOR);
      M_Menu(CURSOR);
    }
  }
}

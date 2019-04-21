// Code photoréacteur 0.1
const String version = "0.1";
const String version_hardware = "1.0";
/*
 * TODO
 * - Menu(s) de lancement du fonctionnement
 * - Menu d'erreur
 * - Gestion des LEDs encodeur
 * - Gestion de la LED
 * - Gestion du CLICK -> fonction globale qui dirige (permet de généraliser la lecture)
 * 
 * - Décompte en secondes, affichage (prendre valeur confi durée)
 *
 * AMÉLIORATIONS
 * - Passer de 60s à 1mn et 60mn à 1h
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

bool select_click = false; // Curseur click activé
int CURSOR_CLICK = 1; // Sélection avec le click
int maxSelect_click = 8; // Valeur max de sélection (nombre de sélections possibles)

// Variables interrupteur
bool pushedPrev = true;
bool pushed = false; // Appuie 1 fois

// Variables de fonctionnement
int duration_val[7] = {0,0,0,0,0,0}; // Temps de durée réglé
bool reacting = false;
unsigned long time_left = 0; // Temps restant en secondes d'expérience
bool start_ventilo = false; // Savoir si à démarrer
bool VENTILO_STATE = false; // Etat actuel
bool start_led = false; // Savoir si à démarrer
bool LED_STATE = false; // Etat actuel (savoir si à démarrer)



// -- Fonction générales --
// Transformer temps secondes en HH:MM:SS
String time_sec_toStr(unsigned long timeSec){
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



// -- Fonctions de formattage --
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
  LCD.CursorConf(OFF, 0);
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
void MF_text(String text, String place = "HL", bool jump=true){
  LCD.FontModeConf(Font_6x8, FM_ANL_AAA, BLACK_NO_BAC);

  int posX = 0;
  int posY = MF_pos[1];
  if(place=="BR"){ // Placer le texte
    posX = FF_placeX(text,6,"R");
    posY = FF_placeY(text,8,"B");
  }else if(place=="C"){
    posX = FF_placeX(text,6,"C");
  }else if(place=="BL" || place=="B"){
    posY = FF_placeY(text,8,"B");
  }else if(place == "R"){
    posX = FF_placeX(text,6,"R");
  }
  
  LCD.DispStringAt(text.c_str(), posX, posY);
  
  MF_pos[0]=0;
  if(jump){
    MF_pos[1]+=8+MF_spacing;
  }
}
// Ecrire du texte plus gros
void MF_text_big(String text, String place = "HL"){
  LCD.FontModeConf(Font_8x16_1, FM_ANL_AAA, BLACK_NO_BAC);

  int posX = 0;
  int posY = MF_pos[1]-2;
  if(place=="BR"){ // Placer le texte
    posX = FF_placeX(text,8,"R");
    posY = FF_placeY(text,16,"B");
  }else if(place=="C"){
    posX = FF_placeX(text,8,"C");
  }else if(place=="BL" || place=="B"){
    posY = FF_placeY(text,16,"B");
  }
  
  LCD.DispStringAt(text.c_str(), posX, posY);
  
  MF_pos[0]=0;
  MF_pos[1]+=14;
}

// Boutton avec cadre, noir si sélectionné
void MF_button(String text, bool select = false, String place="C", bool jump=true){
  int posX=2;
  int posY=MF_pos[1];
  if(place=="C"){ // Centrer le texte
    posX = FF_placeX(text,6);
  }else if(place=="BR"){
    posX = FF_placeX(text,6, "R")-2;
    posY = FF_placeY(text,8, "B")-2;
  }else if(place=="BL"){
    posY = FF_placeY(text,8, "B")-2;
  }
  
  if(select){ // Si sélectionné carré rempli
    LCD.DrawRectangleAt(posX-2, posY, 4+text.length()*6, 4+8, BLACK_FILL);
  }else{
    LCD.DrawRectangleAt(posX-2, posY, 4+text.length()*6, 4+8, BLACK_NO_FILL);
  }
  
  if(select){ // Si sélectionné écrit en blanc
    LCD.FontModeConf(Font_6x8, FM_ANL_AAA, WHITE_NO_BAC);
  }else{
    LCD.FontModeConf(Font_6x8, FM_ANL_AAA, BLACK_NO_BAC);
  }

  LCD.DispStringAt(text.c_str(), posX, posY+2);

  if(jump){
    MF_pos[1]+=12+MF_spacing_button;
  }
}
// ----------------


// -- Les menus --
// Menu principal avec quel bouton sélectionné (0=aucun, 1-3)
void M_Menu(int selected = 0){
  LOC = "home";
  select = true;
  maxSelect = 3;
  MF_leds(LED_G);
  MF_reset();
  MF_title("MENU");
  MF_button("Lancer Experience", (selected==1)? true:false);
  MF_button("Aide", (selected==2)? true:false);
  MF_button("Infos", (selected==3)? true:false);
  MF_text("v"+version, "BR");
}

// En cours de réaction
/*
 * TODO
 * - Température actuelle
 * - Pourcentage lumineux
 * - Pause
 * - Arrêter
 * - Message "NE PAS OUVRIR !!!"
 * - Actualisation chaque seconde
 */
void M_Started(int selected=0, bool pause = false, String temp = "??", String perc_lum = "??"){
  LOC = "started";
  select = true;
  maxSelect = 2;
  MF_leds(LED_R);
  MF_reset();
  String pause_txt = (pause)? "PAUSE" : "REACTION EN COURS...";
  MF_title(pause_txt);
  MF_text("Passe: "+ time_sec_toStr(time_to_sec(duration_val)-time_left) +"  |  "+ String(map(time_to_sec(duration_val)-time_left, 0,time_to_sec(duration_val), 0,100)) +"%");
  MF_text("Restant: "+ time_sec_toStr(time_left));
  MF_text("Fin: "+ time_sec_toStr(time_to_sec(duration_val)));
  MF_text("Temp: "+ temp +" C", "L", false);
  MF_text("Lum: "+ perc_lum +"%", "R");
  pause_txt = (pause)? "Reprendre" : "Pause";
  MF_button("Pause", (selected==1)? true:false, "BL", false);
  MF_button("Arreter", (selected==2)? true:false, "BR");
}

// Sélection durée
/*
 * TODO
 * - Limiter minutes et secondes
 */
void M_Duration(int selected=1, int value=1){
  LOC = "duration";
  select = true;
  maxSelect = 10;
  static int selectedBefore; // Set selectedBefore if not set
  if(selectedBefore != value && (selected==7 || selected==8)){ // Si valeur différente précédente et sur Confirmer ou Annuler = menu
    if(selected==7){
      time_left = 0;
      // Secondes
      time_left += duration_val[0];
      time_left += 10*duration_val[1];

      // Minutes
      time_left += 60*duration_val[2];
      time_left += 10*60*duration_val[3];

      // Heures
      time_left += 3600*duration_val[4];
      unsigned long temp_time = duration_val[5]; // Dépassement de capacité
      time_left += 10*(3600*temp_time);
      
      if(time_left < 10){ // Durée minimum pour lancer
        /* ERREUR A EXPLIQUER ?? -> DURÉE TROP PETITE */
      }else{
        CURSOR = 0; // Rien sélecté
        M_Started();
      }
    }else if(selected==8){
      CURSOR = 1;
      M_Menu();
    }
  }else{
    MF_leds(LED_G);
    MF_reset();
    MF_title("DUREE EXPERIENCE");
    MF_text("HH:MM:SS", "C");
    if(selected < 7 && selected > 0){
      // Placer le curseur sur le chiffre à modifier
      int charCursor = 0; // Bien placer curseur
      if(selected == 3 || selected == 4){
        charCursor = 7-selected;
      }else if(selected == 1 || selected == 2){
        charCursor = 8-selected;
      }else if(selected == 5 || selected == 6){
        charCursor = 6-selected;
      }
      LCD.CursorGotoXY(128/2-8*8/2-1 + 8*charCursor, MF_pos[1]-2, 8, 16);
      LCD.CursorConf(ON, 10);

      // Affecteur/Afficher la valeur choisie
      duration_val[selected-1] = value-1;
    }else
    {
      LCD.CursorConf(OFF, 10);
    }
    MF_text_big(String(duration_val[5])+String(duration_val[4]) +":"+ String(duration_val[3])+String(duration_val[2]) +":"+ String(duration_val[1])+String(duration_val[0]), "C");

    MF_button("Confirmer", (selected==7)? true:false);
    MF_button("Annuler", (selected==8)? true:false);
  }

  // Sauvegarder valeur précédente de la rotation
  selectedBefore = value;
}

// Informations sur le photoréacteur
void M_Infos(){
  LOC = "infos";
  select = false;
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
  LOC = "error";
  select = false;
  MF_leds(LED_R);
  MF_reset();
  MF_title("ERREUR");
}
// ----------------



// -- Fonctions et variables encodeur --
/* TODO
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
  if(LOC == "home"){
    switch (CURSOR)
    {
      case 1:
        CURSOR_CLICK = 1;
        M_Duration();
        break;
      case 3:
        M_Infos();
        break;
    }
  }else if(LOC == "infos"){
    CURSOR = 3;
    M_Menu();
  }else if(LOC == "duration"){
    // Place curseur sur bouton ou chiffre
    CURSOR_CLICK = (CURSOR_CLICK == maxSelect_click)? 1 : CURSOR_CLICK+1;
    // Affecte la valeur au curseur
    if(CURSOR_CLICK > 0 && CURSOR_CLICK < 7){
      CURSOR = duration_val[CURSOR_CLICK-1]+1;
    }

    M_Duration(CURSOR_CLICK, CURSOR);
  }else if(LOC == "started"){
    switch (CURSOR)
    {
    case 1:
      /* PAUSE */
      break;
    
    case 2:
      /* ANNULER */
      break;
    }
  }
}

// Gestion de la rotation et actions (1 rotation horaire/trigo = H/T)
void rotationGestionary(char rotDir){
  if(select){
    refreshScreen = true;
    // Ajouter ou retirer 1 au curseur
    if(rotDir == 'H'){
      CURSOR = (CURSOR >= maxSelect) ? 1 : CURSOR+1;
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
  // M_Started();
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
      M_Menu(CURSOR);
    }else if(LOC == "duration"){
      M_Duration(CURSOR_CLICK, CURSOR);
    }
  }
}

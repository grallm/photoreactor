// Code photoréacteur 0.1
const String version = "0.1";
const String version_hardware = "1.0";
/*
 * PRIORITY
 * - Faire fonctionner la lecture du thermomètre
 * - calibrer thermomètre
 * - Utiliser floats pour température
 * - Différence lumière non fonctionnelle
 * 
 * TODO
 * - Remettre en place blocage depuis thermomètre et photorésistance
 * - Gestion température
 * - Gestion température et Luminosité moyennes
 * - Vérifier si le thermomètre est fonctionnel
 * - Trouver d'où vient le saut entre secondes et minutes (quand baisse secondes)
 * - Afficher température en pause et lors rotation -> variables globales
 * - Vérifier arrondi température
 * 
 *
 * AMÉLIORATIONS
 * - Passer de 60s à 1mn et 60mn à 1h
 * - Bonne gestion ventilateur selon température
 * - Meilleure actualisation écran -> refresh que les parties nécessaires
 * - Message "NE PAS OUVRIR !!!" lorsqu'en réaction
 * - Veille écran
 * - optimiser le nombre/type de variables et d'expressions
 * - Plus grande différence de température
 * - Faire fonctionner les erreurs de température et de lumière
 * - Faire plus grande différence de température et de lumière
 * - N'actualiser que les données sur le M_Started -> pas les boutons pause et arrêter
 * - Calibrer le thermomètre
 * - Corriger clignotement
 * - Meilleur curseur (menu principal, started, duration)
 */

#include <Wire.h>
#include <I2C_LCD.h>

I2C_LCD LCD;
uint8_t I2C_LCD_ADDRESS = 0x51; // Adresse écran

/* CONFIGURATION */
const short luminosite = 100, // en %
    LED_R = 6, // Pin LED encodeur rouge
    LED_G = 5, // Pin LED encodeur verte
    LED_HP = 9, // Pin contrôle LED haute puissance
    VENTILO = 11, // Pin contrôle ventilo
    ENC_ROT_1 = 2, // Encodeur rotatif
    ENC_ROT_2 = 3, // Encodeur rotatif
    ENC_INTERRUPTOR = 4, // Interrupteur
    PIN_THERM = A0, // Pin capteur température
    PIN_LUM = A3, // Pin capteur lumière
    CAL_LUM_MIN = 7,
    CAL_LUM_MAX = 146, // Calibrage Min et Max de la photoresistance slon LED
    DOUBLE_LED_SECU = true; // Double sécurité LED (allumée que lorsqu'en réaction)

// Localisation
String LOC = "home";

// Sélection
bool select = true; // Curseur activé
short CURSOR = 0; // Valeur sélecteur (0 = neutre)
short maxSelect = 3; // Valeur max de sélection (nombre de sélections possibles)

bool select_click = false; // Curseur click activé
short CURSOR_CLICK = 1; // Sélection avec le click
short maxSelect_click = 8; // Valeur max de sélection (nombre de sélections possibles)

// Variables interrupteur
bool pushedPrev = true;
bool pushed = false; // Appuie 1 fois

// Variables de fonctionnement
short duration_val[7] = {0,0,0,0,0,0}; // Temps de durée réglé
unsigned long time_left = 0; // Temps restant en secondes d'expérience
short REACTING = 0; // Etat (0: OFF / 1: ON / 2: To start / >=3: To stop)
short VENTILO_STATE = false; // Etat (0: OFF / 1: ON / 2: To start / >=3: To stop)
short LED_STATE = false; // Etat (0: OFF / 1: ON / 2: To start / >=3: To stop)
unsigned long last_sec_millis = millis(); // Sauvegarde dernière valeur lors décompte 1 seconde
short led_twinkle = 0; // Stocker PIN
bool TWINKLE_STATE = false; // Savoir si éteinte ou allumée
unsigned long last_twinkle = 0; // Stockage millis clignotement pour ne pas clignoter extêmement vite

// Variables capteurs
short start_temp = 0; // Température au démarrage
float moy_temp_accu = 0; // Accumulateur température chaque seconde pour calculer moyenne
float max_diff_temp = 0; // Ecart maximum avec la moyenne
unsigned long moy_lumi_accu = 0; // Accumulateur lumière chaque seconde pour calculer moyenne
short max_diff_lumi = 0; // Ecart maximum avec la moyenne
float temp_mes = -1; // Température mesurée en degrés
int lum_mes = -1; // Valeur mesurée en % capteur lumière



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
  digitalWrite(LED_G, LOW);
  digitalWrite(LED_R, LOW);

  digitalWrite(led, HIGH);
  if(twinkle){
    led_twinkle = led;
    TWINKLE_STATE = true;
  }else{
    led_twinkle = 0;
  }
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

// Fin d'expérience
/* TODO
 * - Afficher temp et lum moy
 */
void M_Finish(unsigned long time, bool aborted = false, String moy_temp = "??", String moy_perc_lum = "??"){
  LOC = "finish";
  select = false;
  MF_leds(LED_G);
  MF_reset();
  MF_title("EXPERIENCE TERMINEE");
  MF_text("Temps d'exposition", "C");
  MF_text(time_sec_toStr(time), "C");
  MF_text("Temp. moy.: "+ moy_temp +" C"); // 
  MF_text("Lumi. moy.: "+ moy_perc_lum +" %");
  if(aborted){
    MF_text("ARRETEE", "C");
  }
}

// En cours de réaction
void M_Started(int selected=0, bool pause = false, String temp = "??", String perc_lum = "??"){
  LOC = "started";
  select = true;
  maxSelect = 2;
  MF_leds(LED_G, (pause)?false:true);
  MF_reset();
  String pause_txt = (pause)? "PAUSE" : "REACTION EN COURS...";
  MF_title(pause_txt);
  MF_text("Passe: "+ time_sec_toStr(time_to_sec(duration_val)-time_left) +"  |  "+ String(map(time_to_sec(duration_val)-time_left, 0,time_to_sec(duration_val), 0,100)) +"%");
  MF_text("Restant: "+ time_sec_toStr(time_left));
  MF_text("Fin: "+ time_sec_toStr(time_to_sec(duration_val)));
  MF_text("Temp: "+ temp +"C", "L", false);
  MF_text("Lum: "+ perc_lum +"%", "R");
  pause_txt = (pause)? "Reprendre" : "Pause";
  MF_button(pause_txt, (selected==1)? true:false, "BL", false);
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
      
      if(time_left > 1){ // Durée minimum pour lancer
        CURSOR = 0; // Rien sélectionné
        // Démarrer réaction
        REACTING = 2;

        moy_temp_accu = 0;
        max_diff_temp = 0;
        moy_lumi_accu = 0;
        max_diff_lumi = 0;
        temp_mes = -1;
        lum_mes = -1;
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
    MF_text_big(time_sec_toStr(time_to_sec(duration_val)), "C");

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
void M_Error(int errID=0){
  String erreurs[7] = {"Erreur inconnue", "Sécurité LED", "LED ON pas M_Started", "Temperature haute", "Temperature basse", "Disfonctionnement LED"};
  String solutions[7] = {"", "Relancer expérience", "", "Temp. trop haute", "Temp. trop basse", "Verifier LED/branch."};
  String solution = solutions[errID];
  if(solution == ""){ // Erreur défaut
    solution = "Eteindre/Rallumer";
  }
  
  LOC = "error";
  select = false;
  MF_leds(LED_R);
  MF_reset();
  MF_title("ERREUR n" + String(errID));
  MF_text("PROBLEME:");
  MF_text(erreurs[errID]);
  MF_text("SOLUTION:");
  MF_text(solution);
  MF_text("IDEALEMENT SIGNALER", "C");
}
// ----------------



// -- Fonctions et variables encodeur --
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
    case 1: // Pause/Reprendre
      if(REACTING == 1){ // En marche -> pause
        REACTING = 3; // Arrêter
        M_Started(CURSOR, true, (temp_mes!=-1)?String(temp_mes):"??", (lum_mes!=-1)?String(lum_mes):"??");

        last_sec_millis = millis()-last_sec_millis; // Sauvegarde la différence pour remettre même lors repris = précision
      }else{  // En pause -> En marche
        REACTING = 2; // Démarrer
        M_Started(CURSOR, false, (temp_mes!=-1)?String(temp_mes):"??", (lum_mes!=-1)?String(lum_mes):"??");

        last_sec_millis = millis()-last_sec_millis; // Remettre à la même différence pour reprise précise
      }
      break;
    
    case 2: // Arrêté
      REACTING = 3;
      unsigned long time_passed = time_to_sec(duration_val)-time_left - 2;
      M_Finish(time_to_sec(duration_val)-time_left,true, String(round((moy_temp_accu/time_passed)*10)/10)+"+"+String(max_diff_temp), String(round(moy_lumi_accu/time_passed))+"+"+String(max_diff_lumi));
      break;
    }
  }else if(LOC == "error"){
    CURSOR = 0;
    M_Menu();
  }else if(LOC == "finish"){
    CURSOR = 0;
    M_Menu();
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

  // - LEDS ET VENTILO -
  pinMode(LED_HP,OUTPUT);
  pinMode(LED_G,OUTPUT);
  pinMode(LED_R,OUTPUT);
  pinMode(VENTILO,OUTPUT);
  digitalWrite(LED_HP,LOW); // Eteindre au démarrage
  // -------------------

  // ---- CAPTEURS ----
  pinMode(PIN_THERM, INPUT);
  pinMode(PIN_LUM, INPUT);

  // Lecture température
  int valTemp = analogRead(PIN_THERM);
  // Capteur LM35DZ
  start_temp = valTemp * (5.0 / 1023.0 * 100.0); // Conversion en degrés Celsius
  start_temp = round(start_temp*10)/10; // Arrondir à 1 décimale
  // -------------------

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

  // Gestion temps, décompte
  if(REACTING==1 && time_left > 0 && (millis()-last_sec_millis)>=1000){ // En cours, baisser compteur
    last_sec_millis = millis();
    time_left--;

    unsigned long time_passed = time_to_sec(duration_val)-time_left;

    // Valeurs capteurs: photorésistance et thermomètre
    /* TODO
    * - faire fonctionner thermomètre
    * - capteur lum attend que LED s'allume
    * - Vérifier si temp et lum non définies ne s'affichent pas avec valeur nawak
    * - limiter décimales température
    */
    if(time_passed>2){ // Attendre 2s avant de commencer à capter
      // Lecture température
      int valTemp = analogRead(PIN_THERM);
      // Capteur LM35DZ
      temp_mes = valTemp * (5.0 / 1023.0 * 100.0); // Conversion en degrés Celsius
      temp_mes = round(temp_mes*10.0)/10.0; // Arrondir à 1 décimale

      moy_temp_accu += temp_mes;
      
      time_passed -= 2; // On retire les 2s qu'on a attendu au démarrage
      if(abs(temp_mes - (round((moy_temp_accu/time_passed)*10)/10)) > max_diff_temp){ // Plus gros écart température
        max_diff_temp = abs(temp_mes-(round((moy_temp_accu/time_passed)*10)/10));
      }

      /* if(temp>start_temp+10 || temp>50){ // Températures extrêmes
        REACTING = 3;
        M_Error(3);
      }else if(temp<start_temp-10 || temp<1){
        REACTING = 3;
        M_Error(4);
      } */


      // Lecture pourcentage éclairage
      lum_mes = analogRead(PIN_LUM);
      lum_mes = map(lum_mes, CAL_LUM_MIN, CAL_LUM_MAX, 0,100); // Eclairage en %

      moy_lumi_accu += lum_mes;

      if(abs(lum_mes- (round((moy_lumi_accu/time_passed)*10)/10))>max_diff_lumi){ // Plus gros écart lumière
        max_diff_lumi = abs(lum_mes- (round((moy_lumi_accu/time_passed)*10)/10));
      }

      /* if(lum>120 || lum<30){ // Disfonctionnement LED
        REACTING = 3;
        M_Error(5);
      } */
    }
    // -----------------

    refreshScreen = true;
  
  }else if(REACTING==1 && time_left <= 0 && (millis()-last_sec_millis)>=1000){ // Terminé, tout arrêter (pas ventilo ?)
    last_sec_millis = millis();
    time_left = 0;
    REACTING = 3; // Arrêter
    
    unsigned long time_passed = time_to_sec(duration_val)-time_left - 2;
    M_Finish(time_to_sec(duration_val), false, String(round((moy_temp_accu/time_passed)*10)/10)+"+"+String(max_diff_temp), String(round(moy_lumi_accu/time_passed))+"+"+String(max_diff_lumi));

    // Reset temps configuré
    for(short i=0; i<6; i++){
      duration_val[i] = 0;
    }
  }

  // Gestion réaction (et tout)
  else if(REACTING==2){ // Tout allumer
    LED_STATE = 2; // Allumer LED
    VENTILO_STATE = 2; // Allumer ventilo

    REACTING = 1; // En réaction
  }else if(REACTING >= 3 || (REACTING==1 && LOC != "started")){ // Tout éteindre, arrêter réaction
    LED_STATE = 3; // Eteindre LED
    VENTILO_STATE = 3; // Eteindre ventilo

    REACTING = 0; // Hors réaction

    if(REACTING==1 && LOC != "started"){ // En réaction alors que pas dans menu started
      M_Error(2);
    }
  }

  
  // Clignement LED
  if(led_twinkle!=0){ // Si clignotement activé allumer ou éteindre selon étant toutes les 0.5s
    if((millis()%1000)==0 && last_twinkle!=millis() && (last_twinkle>millis()+900 || last_twinkle<millis()-900)){
      if(TWINKLE_STATE){
        TWINKLE_STATE = false;
        digitalWrite(led_twinkle, LOW);
      }else{
        digitalWrite(led_twinkle, HIGH);
        TWINKLE_STATE = true;
      }
      last_twinkle = millis();
    }
  }


  // Gestion HP LED
  if(LED_STATE==2 && ((DOUBLE_LED_SECU && REACTING==1) || !DOUBLE_LED_SECU)){ // Allumer LED si éteinte
    analogWrite(LED_HP, 255);
    LED_STATE = 1; // Allumée
  }else if(LED_STATE==2 && (DOUBLE_LED_SECU && REACTING!=1)){ // Vouloir allumer LED hors réaction
    analogWrite(LED_HP, 0);
    analogWrite(VENTILO, 0);
    REACTING = 3; // Tout éteindre

    M_Error(1); // Erreur de sécurite: Double sécu activée et pas en réaction

  }else if(LED_STATE>=3){ // Eteindre LED
    analogWrite(LED_HP, 0);
    LED_STATE = 0;
  }

  // Gestion ventilateur
  if(VENTILO_STATE==2){ // Allumer ventilo si éteint
    analogWrite(VENTILO, 255);
    VENTILO_STATE = 1; // Allumé
  }else if(VENTILO_STATE==3){ // Eteindre VENTILO
    analogWrite(VENTILO, 0);
    VENTILO_STATE = 0;
  }

  // Rafraîchissement de l'écran
  if(refreshScreen){
    if(LOC == "home"){
      M_Menu(CURSOR);
    }else if(LOC == "duration"){
      M_Duration(CURSOR_CLICK, CURSOR);
    }else if(LOC == "started"){
      M_Started(CURSOR, (REACTING==1)?false:true, (temp_mes!=-1)?String(temp_mes):"??", (lum_mes!=-1)?String(lum_mes):"??");
    }
  }
}

// Menu photoréacteur 0.1
const String version = "0.1";
const String version_hardware = "1.0";
/*
 * - Menu principal avec différents boutons
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
    LED_G = 5; // Pin LED encodeur verte

// Localisation
String LOC = "home";

// Sélection
bool select = true;
int maxSelect = 3;

// Variables de fonctionnement
bool reacting = false;
unsigned long time_left = 0; // Temps restant en secondes d'expérience
bool start_ventilo = false; // Savoir si à démarrer
bool VENTILO_STATE = false; // Etat actuel
bool start_led = false; // Savoir si à démarrer
bool LED_STATE = false; // Etat actuel (savoir si à démarrer)

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
    Serial.println(63-fontY-1);
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
  Serial.println(place);
  Serial.println(place=="C");
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

// Sélection durée
/*
 * TODO
 * - Pouvoir confirmer et annuler
 * - Limiter minutes et secondes
 * - Sauvegarder pour durée
 */
int M_Duration_val[7] = {0,0,0,0,0,0};
void M_Duration(int selected=1, int value=1){
  LOC = "duration";
  select = true;
  maxSelect = 10;
  static int selectedBefore; // Set selectedBefore if not set
  if(selectedBefore != value && (selected==7 || selected==8)){ // Si valeur différente précédente et sur Confirmer ou Annuler = menu
    if(selected==7){
      /* SUITE */
    }else if(selected==8){
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
      M_Duration_val[selected-1] = value-1;
    }else
    {
      LCD.CursorConf(OFF, 10);
    }
    MF_text_big(String(M_Duration_val[5])+String(M_Duration_val[4]) +":"+ String(M_Duration_val[3])+String(M_Duration_val[2]) +":"+ String(M_Duration_val[1])+String(M_Duration_val[0]), "C");

    MF_button("Confirmer", (selected==7)? true:false);
    MF_button("Annuler", (selected==8)? true:false);
  }

  // Sauvegarder valeur précédente de la rotation
  selectedBefore = value;
}

// En cours de réaction
/*
 * TODO
 * - Temps restant
 * - Température actuelle
 * - Pourcentage lumineux
 * - Pause
 * - Arrêter
 * - Message "NE PAS OUVRIR !!!"
 * - 
 */
void M_Started(int selected=0){
  LOC = "started";
  select = true;
  maxSelect = 2;
  MF_leds(LED_R);
  MF_reset();
  MF_title("REACTION EN COURS...");
  MF_text("Passe: "+String(M_Duration_val[5])+String(M_Duration_val[4]) +":"+ String(M_Duration_val[3])+String(M_Duration_val[2]) +":"+ String(M_Duration_val[1])+String(M_Duration_val[0]));
  MF_text("Restant: "+String(M_Duration_val[5])+String(M_Duration_val[4]) +":"+ String(M_Duration_val[3])+String(M_Duration_val[2]) +":"+ String(M_Duration_val[1])+String(M_Duration_val[0]));
  MF_text("Fin: "+String(M_Duration_val[5])+String(M_Duration_val[4]) +":"+ String(M_Duration_val[3])+String(M_Duration_val[2]) +":"+ String(M_Duration_val[1])+String(M_Duration_val[0]));
  MF_text("Temp: 35 C", "L", false);
  MF_text("Lum: 100%", "R");
  MF_button("Pause", (selected==1)? true:false, "BL", false);
  MF_button("Arreter", (selected==2)? true:false, "BR");
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


void setup() {
  Wire.begin();
  
  Serial.begin(9600);
  Serial.println("START");
  
  LCD.CleanAll(WHITE);
  LCD.BacklightConf(LOAD_TO_EEPROM,map(luminosite,0,100,0,127)); // Luminosité (0-127) sauvegardée au reboot
  LCD.WorkingModeConf(OFF, ON, WM_CharMode); // Pas LOGO, Rétro éclairage,


  M_Started();
}

void loop() {
  if(refreshScreen){
    
  }
}

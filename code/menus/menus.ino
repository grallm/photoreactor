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

// Réinitialiser la position et nettoyer écran
void MF_reset(){
  LCD.CleanAll(WHITE);
  MF_pos[0]=0;
  MF_pos[1]=0;
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
  MF_leds(LED_G);
  MF_reset();
  MF_title("MENU");
  MF_button("Lancer Experience", "", (selected==1)? true:false);
  MF_button("Aide", "", (selected==2)? true:false);
  MF_button("Infos", "", (selected==3)? true:false);
  MF_text("v"+version, "BR");
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


void setup() {
  Wire.begin();
  
  Serial.begin(9600);
  Serial.println("START");
  
  LCD.CleanAll(WHITE);
  LCD.BacklightConf(LOAD_TO_EEPROM,map(luminosite,0,100,0,127)); // Luminosité (0-127) sauvegardée au reboot
  LCD.WorkingModeConf(OFF, ON, WM_CharMode); // Pas LOGO, Rétro éclairage,

  /*LCD.DispStringAt("Sparking...", 0, 0);
  LCD.DispStringAt("Sparking...", 0, 10);
  LCD.DispStringAt("Sparking...", centerX("Sparking...",6), 0);*/

  M_Menu();
}

void loop() {
  
}

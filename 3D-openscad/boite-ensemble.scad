use <tubes-bouchons.scad>;
use <cylindre-grille.scad>;
use <modules_achetes.scad>;
use <tiroir_radiateur.scad>;
use <capot_ecran.scad>;

include <nutsnbolts/cyl_head_bolt.scad>;

$fn=75;

largeur=152 + 2*4 + 5;
longueur=195;
hauteur=95/2 + 50;
longueurHaut=126+20;
rTrouVis=2.2;

/*
-= inverser bBoard et trou ventilateur
-= réduire taille trou ventilo
-= pavés pour visser côtés (trou vis long + trou écrou)
-= afficher tiroir
*/


module fond(){
    /*
    -= place pour tous composants -> gérer espace
    -= trous pour composants
    -= trous ventilo
    - pieds (coller caoutchouc ?)
    -= trous N&B pour fixer face gauche
    */
    union(){
        difference(){
            cube([longueur,largeur,2]);
            translate([longueur-80,20,-1]){
                cube(60,6);
            }
            
            // Trous Abaisseur
            translate([20+48,largeur-23-10,0]){
                rotate(90){
                    translate([1.6+1,6+1,-1]) cylinder(5,rTrouVis,rTrouVis);
                    translate([23-1.6-1,48-6-1,-1]) cylinder(5,rTrouVis,rTrouVis);
                }
            }
            // Trous ventilateur
            translate([longueurHaut/2-40/2,largeur/2-40/2]){
                translate([4,4,-1]) cylinder(5,rTrouVis,rTrouVis);
                translate([40-4,4,-1]) cylinder(5,rTrouVis,rTrouVis);
                translate([4,40-4,-1]) cylinder(5,rTrouVis,rTrouVis);
                translate([40-4,40-4,-1]) cylinder(5,rTrouVis,rTrouVis);
            }
            // Trous contrôleur ventilateur
            translate([70,10,0]){
                translate([43.5-1.9-2,1.9+2,-1]) cylinder(5,rTrouVis,rTrouVis);
                translate([1.9+2,43.5-1.9-2,-1]) cylinder(5,rTrouVis,rTrouVis);
                translate([1.9+2,1.9+2,-1]) cylinder(5,rTrouVis,rTrouVis);
                translate([43.5-1.9-2,43.5-1.9-2,-1]) cylinder(5,rTrouVis,rTrouVis);
            }
        }
        
        // Grille avec "lames" à l'avant
        translate([longueur-20,20+60,0]){
            rotate([0,0,180]){
                import("ventilo-grille-lames.stl");
            }
        }
        
        // Supports pour fixer face gauche
         // Bas Droite
        translate([2+10,largeur-32,10+2]){
            rotate([0,180,0]){
                difference(){
                    cube([10,30,10]);
                    translate([-1,0,-25]) rotate([55,0,0]) cube([30,30,30]);
                    
                    translate([5,34,5]){
                        // Trou vis
                        rotate([90,0,0]) cylinder(20,rTrouVis,rTrouVis);
                        // Trou écrou
                        //translate([0,-11.5,0]) rotate([90,90,0]) nutcatch_sidecut("M4", l=20, clk=-.1);
                        translate([0,-30,0]) rotate([90,90,0]) nutcatch_parallel("M4", l=23, clk=-.1);
                    }
                }
            }
        }
         // Bas Gauche
        translate([longueur-2,largeur-32,10+2]){
            rotate([0,180,0]){
                difference(){
                    cube([10,30,10]);
                    translate([-1,0,-25]) rotate([55,0,0]) cube([30,30,30]);
                    
                    translate([5,34,5]){
                        // Trou vis
                        rotate([90,0,0]) cylinder(20,rTrouVis,rTrouVis);
                        // Trou écrou
                        //translate([0,-11.5,0]) rotate([90,90,0]) nutcatch_sidecut("M4", l=20, clk=-.1);
                        translate([0,-30,0]) rotate([90,90,0]) nutcatch_parallel("M4", l=23, clk=-.1);
                    }
                }
            }
        }
    }
}

module arriere(){
    /*
    */
    
    difference(){
        cube([hauteur-4,largeur,2]);
        // Interrupteur
        translate([5,40,-1]){
            cube([20,23,10]);
        }
        // Alim femelle
        translate([12.5,25,-1]){
            cylinder(10,9/2,9/2);
        }
    }
}

module faceEcran(){
    /*
    -= trou cables écran + encodeur
    -= trous attachent écran
    */
    
    largHauteur=longueur-longueurHaut; // (h=l=49)
    longueurCalc=sqrt(2*pow(largHauteur,2))+5; // 69.3
    
    difference(){
        cube([longueurCalc-2,largeur,2]);
        
        // Trous vis
        translate([72,largeur/2-136/2,-1]){
            rotate(90){
                translate([6,6,0]){
                    cylinder(5,2.1,2.1);
                }
                translate([136-6,6,0]){
                    cylinder(5,2.1,2.1);
                }
                translate([6,72-6,0]){
                    cylinder(5,2.1,2.1);
                }
                translate([136-6,72-6,0]){
                    cylinder(5,2.1,2.1);
                }
            }
        }
        // Trou câbles
        translate([45,110,-1]){
            cube([15,15,4]);
        }
    }
}

module face(){
    cube([hauteur-4-(longueur-longueurHaut),largeur,2]);
}

module haut(){
    /*
    -= rails pour tiroir
    -= trous N&B vis et écrou pattes cylindre
    -= vérifier trous écrous N&B
    -= trous face gauche
    -= trou pour faire passer câbles thermomètre et photorésistance
    */
    R=63+7; // R cylindre
    rTrou=2.3; // R trous vis
    //lLang=;
    
    difference(){
        union(){
            cube([longueurHaut,largeur,2]);
            
            // Rails tiroir + trous écrous cylindre
            translate([0,0,-10]){
                // Droite (X ligne trous = longueurHaut/2-60+12)
                translate([longueurHaut/2-60+12-10,0,0]){
                    difference(){
                        cube([27,largeur-7,10]);
                        // Rail
                        translate([17,52,5]){
                            cube([50,largeur+10,5]);
                        }
                    }
                }
                // Gauche (X ligne trous = longueurHaut/2+60-12)
                translate([longueurHaut/2+60-12-17, ,0]){
                    difference(){
                        cube([27,largeur-7,10]);
                        // Rail
                        translate([-1,52,5]){
                            cube([10,largeur+10,5]);
                        }
                    }
                }
            }
            // Blocs pour fixer face détachée
            // Haut Droite
            translate([2,largeur-32,-10]){
                difference(){
                    cube([10,30,10]);
                    translate([-1,0,-25]) rotate([55,0,0]) cube([30,30,30]);
                    
                    translate([5,0,5]){
                        // Trou vis
                        translate([0,35,0]) rotate([90,0,0]) cylinder(20,rTrouVis,rTrouVis);
                        
                        // Trou écrou
                        rotate([90,0,0]) nutcatch_parallel("M4", l=25, clk=-.1);
                        //translate([0,-11.5,0]) rotate([90,90,0]) nutcatch_sidecut("M4", l=20, clk=-.1);
                    }
                }
            }
            // Haut Gauche
            translate([longueurHaut-3-10,largeur-32,-10]){
                difference(){
                    cube([10,30,10]);
                    translate([-1,0,-25]) rotate([55,0,0]) cube([30,30,30]);
                    
                    translate([5,0,5]){
                        // Trou vis
                        translate([0,35,0]) rotate([90,0,0]) cylinder(20,rTrouVis,rTrouVis);
                        // Trou écrou
                        rotate([90,0,0]) nutcatch_parallel("M4", l=25, clk=-.1);
                        //translate([0,-11.5,0]) rotate([90,90,0]) nutcatch_sidecut("M4", l=20, clk=-.1);
                    }
                }
            }
        }
        
        translate([longueurHaut/2,largeur/2,0]){
            // Trou LED cylindre
            translate([0,0,-1]) cylinder(6,20,20);
        
            // Trou thermomètre + photorésistance
            translate([20,-40,-1]) cylinder(4,4,4);
            
            // Trous attaches cylindre
            // 4 pattes
            rotate(45){
                for(i=[1:1:4]){
                    translate([(R-2)*cos(i*90),(R-2)*sin(i*90),-15]){
                        cylinder(20,rTrou,rTrou);
                        translate([0,0,+13]) nutcatch_parallel("M4", l=50, clk=-.1);
                        // Trou écrou
                        rotEc = (i==3 || i==4) ? 135 : -45;
                        //translate([0,0,14]) rotate(rotEc) nutcatch_sidecut("M4", l=20, clk=-.1);
                    }
                }
            }
            // 6 pattes
            /*rotate(60){
                for(i=[1:1:6]){
                    translate([(R-2)*cos(i*60),(R-2)*sin(i*60),0]){
                        cylinder(6,rTrou,rTrou);
                    }
                }
            }*/
        }
    }
}

module droite(){
    /*
    */
    largHauteur=longueur-longueurHaut; // (h=l=49)
    longueurCalc=sqrt(2*pow(largHauteur,2)); // 69.3
    
    difference(){
        cube([longueur-2,hauteur-4,2]);
        // Face écran (anglée)
        translate([longueur,hauteur-largHauteur-6,-2]){
            rotate([0,0,45]){
                cube([longueurCalc,100,8]);
            }
        }
    }
}

module gauche(){
    /*
    -= modification taille des 2mm -> regarder gauche
    -= 2 epaisseurs pour "s'encocher"
    -= trous pour accrocher
    -= système pour bloquer tiroir quand fermé: pied ?
    */
    largHauteur=longueur-longueurHaut; // (h=l=49)
    longueurCalc=sqrt(2*pow(largHauteur,2)); // 69.3
    
    difference(){
        union(){
            translate([-2,-2,-2]){
                color("red") difference(){
                    cube([longueur,hauteur,2]);
                    // Face écran (anglée)
                    translate([longueur+2,hauteur-largHauteur-6+2,-2]){
                        rotate([0,0,45]){
                            cube([longueurCalc,100,8]);
                        }
                    }
                }
            }
            // Partie encoche
            difference(){
                cube([longueur-4,hauteur-4,2]);
                // Face écran (anglée)
                translate([longueur-1.6,hauteur-largHauteur-6-1.6,-2]){
                    rotate([0,0,45]){
                        cube([longueurCalc,100,8]);
                    }
                }
            }
            // Pied (temporaire) pour bloquer tiroir
            translate([50,hauteur-20,0]) cube([10,10,53]);
        }
        // Trous vis pour fixer face
        translate([5,5,-3]) cylinder(10,rTrouVis,rTrouVis);
        translate([longueur-5-4,5,-3]) cylinder(10,rTrouVis,rTrouVis);
        translate([5,hauteur-5-4,-3]) cylinder(10,rTrouVis,rTrouVis);
        translate([longueurHaut-10,hauteur-5-4,-3]) cylinder(10,rTrouVis,rTrouVis);
    }
}

//---------- Boîte ----------
module boite(){
    /*
    -= pavé d'accroche sur angles pour faces côtés
    -= trous vis avec place écrous (Nuts and Bolts, questions Frédéric ?)
    */
    difference(){
        // Toutes faces
        union(){
            // Arrière
            translate([2,0,2]){
                rotate([0,-90,0]){
                    color("orange") arriere();
                }
            }

            // Fond
            color("red") fond();
            
            // Haut
            translate([0,0,hauteur-2]){
                haut();
            }
            
            // Face écran
            translate([longueurHaut-3,0,hauteur-2]){
                rotate([0,45,0]){
                    color("orange") faceEcran();
                }
            }
            
            // Face bas
            translate([longueur,0,2]){
                rotate([0,-90,0]){
                    color("orange") face();
                }
            }
            
            // Droite
            translate([2,2,2]){
                rotate([90,0,0]){
                    color("orange") droite();
                }
            }
        }
        
        // Couper dépassements face écran
        translate([longueur,-1,0]){
            cube([10,200,100]);
        }
        translate([longueurHaut-4,-1,hauteur+2]){
            rotate([0,45,0]){
                cube([10,200,20]);
            }
        }
    }
}


// ---------- Modélisation --------
module photoreacteur(){
    // Boîte
    boite();
    
    // Capot écran+encodeur
    translate([longueur,largeur/2-136/2,hauteur/2+2]){
        rotate([45,0,90]){
            color("lightgray") capot();
        }
    }
  
    // Gauche
    translate([2,largeur,2]){
        rotate([90,0,0]){
            //color("lightblue") gauche();
        }
    }

    // Cylindre
    translate([10+63,largeur/2,hauteur]){
        rotate(45){
            ensembleCylindre();
        }
    }
}


//boite();

/*translate([0,200,0]){
    photoreacteur();
    
    // Tiroir
    translate([longueurHaut/2+78.5/2,largeur/2-51.5/2
,hauteur-22]){
        rotate(90){
            tiroirPlein();
        }
    }
    
    // Items
    color("purple") { // Au fond
        translate([95,largeur-10-57,2]){
            bBoard();
        }
        translate([70,10,2]){
            moteurs();
        }
        translate([48+20,largeur-10-23,2]){
            rotate(90) abaisseur();
        }
        translate([longueur-12,largeur/2-136/2+15,hauteur/2+23]){
            rotate([45,0,90]){
                ecran();
            }
        }
        translate([longueur-26,largeur-47,hauteur/2+27]){
            rotate([45,0,90]){
                encodeur();
            }
        }
    }
    translate([longueurHaut/2-40/2,largeur/2-40/2,hauteur/2+10]){
        ventilateur();
    }
}*/

//droite();

/*faceEcran();
translate([72,largeur/2-136/2,2]){
    rotate(90){
        color("lightgreen") capot();
        translate([15-.1,72-37.83-10-.7,0]){
            ecran();
        }
        translate([130-20-7,36-7,0]){
            color("purple") encodeur();
        }
    }
}*/

/*capot();
translate([15-.1,72-37.83-10-.7,0]) ecran();
translate([-20,0,0]) encodeur();¨*/

gauche();
//tubeGrosBouchon();
//haut();
//fond();
//capot();
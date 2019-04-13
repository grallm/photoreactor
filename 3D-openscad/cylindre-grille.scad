use <tubes-bouchons.scad>;
$fn = 75;

/*
    -= détacher poignée -> coller
    -= trous d'aération entre tubes collés extérieur
    -= capuchon descendant avec 2mm de jour
    -= 4 pattes d'accroche (6 bloquent tiroir + trop long)
    - capote pour écran et encodeur
*/


// Grille tenir tubes: 120x15
module supportTubes() {
    R = 60; // rayon support (+1cm)
    rTrous = 12; // rayons des trous pour tubes (+1mm)
    rPosTrous=40; // Rayon sur lequel trous positionnés
    hCone=9;
    rCone=28;
    
    union(){
        difference(){ // Trous + surface
            union(){
                translate([0,0,3]){
                    cylinder(3, R+3, R+3);
                }
                cylinder(3, R-1, R-1);
            }
            for(i=[0:1:7]){
                translate([rPosTrous*cos(i*60),rPosTrous*sin(i*60),-0.5]){
                    cylinder(10, rTrous,rTrous);
                }
            }
            
            // Trous aération
            for(i=[-1:2:1]){
                translate([50*cos(i*30),50*sin(i*30),-0.5]){
                    cylinder(10,3,3);
                }
                translate([50*cos(i*150),50*sin(i*150),-0.5]){
                    cylinder(10,3,3);
                }
                translate([50*cos(i*90),50*sin(i*90),-0.5]){
                    cylinder(10,3,3);
                }
            }
        }
        translate([0,0,-hCone]){ // Cône de réflexion
            cylinder(hCone,0,rCone);
        }
    }
}

module poignee(){
    // Languette pour grille + capuchon
    cube([10,40,20]);
}

// Cylindre pour pilluliers: 152(126+2*13)x95 (languettes: h=H/2 l=15)
module cylindrePilluliers(){
    R=62; // R grille + épaisseur
    H=95;
    lLanguette=12;
    hLang=0;
    rTrou=2;
    
    difference(){
        union(){
            cylinder(H,R,R);
            
            // 4 pattes
            for(i=[1:1:4]){
                translate([(R-2)*cos(i*90),(R-2)*sin(i*90),0]){
                    rotate(i*90){
                        translate([0,-lLanguette/2,0]){
                            difference(){
                                
                                cube([lLanguette,lLanguette,3]);
                                translate([lLanguette/2+1,lLanguette/2,-1]){
                                    cylinder(5,rTrou,rTrou);
                                }
                            }
                        }
                    }
                }
            }
            
            // 6 pattes
            /*for(i=[1:1:6]){
                translate([(R-2)*cos(i*60),(R-2)*sin(i*60),0]){
                    rotate(i*60){
                        translate([0,-lLanguette/2,0]){
                            difference(){
                                
                                cube([lLanguette,lLanguette,3]);
                                translate([lLanguette/2+1,lLanguette/2,-1]){
                                    cylinder(5,rTrou,rTrou);
                                }
                            }
                        }
                    }
                }
            }*/
        }
        /*translate([0,0,-5]){
            cylinder(H+10,R-3,R-3);
        }*/
        
        // Creux
        translate([0,0,-1]){
            cylinder(H+5,R-2,R-2);
        }
        // Fond
        /*translate([0,0,-3]){
            cylinder(H+6,R/2,R/2);
        }*/
    }
}

module capuchon(){
    H=95;
    R=61+5;
    rPosPieds=55;
    hPieds=20;
    rPieds=2;
    
    union(){
        difference(){
            cylinder(H,R,R);
            
            // Creux
            translate([0,0,-1]){
                cylinder(H-2,R-2,R-2);
            }
        }
        
        // Pieds
        for(i=[1:1:6]){
            translate([rPosPieds*cos(i*60),rPosPieds*sin(i*60),H-hPieds-3]){
                cylinder(hPieds,rPieds,rPieds);
            }
        }
    }
}


module ensembleCylindre(){
    translate([0,0,92]){
        supportTubes();
        translate([-5,-20,6]){ // Languette grille
            poignee();
        }
        
        // Dessiner cercle de 8 tubes
        for(i=[0:1:7]){
            translate([40*cos(i*60),40*sin(i*60),-8]){
                color("white") tubeGrosBouchon();
            }
        }
        
        translate([-5,-20,29]){ // Languette capuchon
            color("gray") poignee();
        }
    }

    color("green") cylindrePilluliers();
    
    translate([0,0,26]){
        color("gray") capuchon();
    }
}


//rotate([180,0,0]){
//supportTubes();
//}
//translate([70,0,-1]){
//    rotate([0,90,0]){
//poignee();
//}}
//cylindrePilluliers();

rotate([0,180,0]){
    capuchon();
}

//ensembleCylindre();
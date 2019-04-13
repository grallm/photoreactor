
longueur=60;

module grilleVentilo(){
    difference(){
        union(){
            // Contours
            difference(){
                cube([longueur,longueur,2]);
                translate([2,2,-1]){
                    cube([longueur-4,longueur-4,6]);
                }
            }
            
            // Lames
            nbLames=10;
            ecart=(longueur-4-2)/nbLames;
            for(i=[0:1:nbLames]){
                translate([i*ecart,0,2]){
                    rotate([0,30,0]){
                        cube([5.65,longueur,.5]);
                    }
                }
            }
        }
        // Couper restes lames
        translate([-1,-1,2]){
            cube([longueur+2,longueur+2,3]);
        }
        translate([0,-1,-2]){
            cube([longueur+2,longueur+2,2]);
        }
    }
}

grilleVentilo();
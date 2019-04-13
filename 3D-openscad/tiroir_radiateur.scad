use <modules_achetes.scad>

// 1 mm de marge pour mettre radiateur
module tiroir(){
    /*
    -= pattes pour rails
    -= faire passer câbles pour branchement LED
    -= contour avec soutien radiateur
    */
    
    long=58.5;
    larg=57;
    haut=19;
    
    union(){
        translate([0,10,0]){
            difference(){
                cube([larg,long,haut]);
                // Place radiateur
                translate([2,2,2]){
                    cube([larg-4,long-4,haut+2]);
                }
                // Croix
                translate([7,2,-1]){
                    cube([larg-6-8,long-4,5]);
                }
                translate([2,10,-1]){
                    cube([larg-4,long-20,5]);
                }
                // Trou câbles
                translate([long-7,10,haut-5]) cube([10,10,6]);
            }
        }
        // Pattes rails
        translate([0,0,haut-3]){
            cube([larg,10,3]);
        }
        translate([0,10+long,haut-3]){
            cube([larg,10,3]);
        }
    }
}


module tiroirPlein(){
    tiroir();
    // Radiateur: 51x51.5x10
    translate([3.5,13.5,3]){
        ledRadiateur();
    }
}

//rotate([180,0,0]) tiroir();
tiroirPlein();
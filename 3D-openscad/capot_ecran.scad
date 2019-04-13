use <modules_achetes.scad>;

$fn=75;

// Capot pour tenir écran + encodeur
module capot(){
    /*
    -= trou écran
    -= trou encodeur
    -= trous attachent écran-capot-boîte
    -= cube creux pour bloquer rotation encodeur
    -= déplacer écran en haut: laisser place gauche et bas pour cable
    */
    union(){
        difference(){
            cube([136,72,11]);
            translate([2,2,-1]){
                cube([132,68,10]);
            }
            // Trou encodeur
            translate([130-20,43,7]){
                cylinder(5,5,5);
            }
            // Trou écran
            translate([20.5,72-37.83-10,7]){
                cube([52,36,5]);
            }
            // Trous vis pour accrocher capot
            translate([6,6,8]){
                cylinder(5,2.1,2.1);
            }
            translate([136-6,6,8]){
                cylinder(5,2.1,2.1);
            }
            translate([6,72-6,8]){
                cylinder(5,2.1,2.1);
            }
            translate([136-6,72-6,8]){
                cylinder(5,2.1,2.1);
            }
        }
        // Pins pour accrocher écran
        translate([15-.1,72-39-10,9]){
            translate([0.9+1.2,1.2+1.2,-4]){
                cylinder(4,1,1);
            }
            translate([63.27-0.9-1.2,1.2+1.2,-4]){
                cylinder(4,1,1);
            }
            translate([63.27-0.9-1.2,38-1.2-1.2,-4]){
                color("red") cylinder(4,1,1);
            }
            translate([0.9+1.2,38-1.2-1.2,-4]){
                color("red") cylinder(4,1,1);
            }
        }
        // Cube bloque encodeur
        difference(){
            translate([130-20-8.25,43-8,4]){
                cube([16.5,16,7]);
            }
            translate([130-20-7.25,43-7,2]){
                cube([14.5,14,10]);
            }
        }
    }
}


capot();
//translate([130-20-7,43-6.75,4]) encodeur();
//ecran ();
//translate([15-.1,72-37.83-10-.7,2.5]) ecran();
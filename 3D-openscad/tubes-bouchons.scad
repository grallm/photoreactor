hTube=50; // Hauteur pullulier
rTube=10.5; // Rayon

// Pillulier (H:50mm, D:21mm), fond plat
module tube(){
    difference() {
        cylinder(hTube,rTube,rTube,true);
        translate([0,0,2]){
            cylinder(hTube-1,rTube-1.5,rTube-1.5,true);
        }
    };
}

// Bouchon (bords +2mm) de pillulier large permettant blocage
module tubeGrosBouchon(){
    color("lightgray");
    translate([0,0,hTube/2-5.5]){
        difference(){
            cylinder(15, rTube+2,rTube+2,true);
            translate([0,0,-1]){
                cylinder(14, rTube,rTube,true);
            }
        }
    }
    tube();
}

color("lightgray") tubeGrosBouchon();
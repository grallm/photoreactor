$fn=75;

// LED: 2x3,45 / Pad: 20x21,5
module led(){
    color("blue")
    union(){
        cube([20,21.5,2]);
        translate([8.275,9.025,2]){
            cube([3.45,3.45,2]);
        }
    }
}

// Radiateur: 51x51,5x10
module radiateur(){
    color("purple") cube([51,51.5,10]);
}

// Ventilo: 30x30x10
module ventilateur(){
    union(){
    difference(){
        color("black") cube([40,40,10]);
        // Trous vis
        translate([4,4,-1]) cylinder(15,2,2);
        translate([40-4,4,-1]) cylinder(15,2,2);
        translate([4,40-4,-1]) cylinder(15,2,2);
        translate([40-4,40-4,-1]) cylinder(15,2,2);
    }
    // Centre
    translate([40/2,40/2,9]) color("lightgray") cylinder(2,1,1);
    }
}

module ledRadiateur(){
    radiateur();
    translate([25.5-10,25.75-10.5,10]){
        led();
    }
}

// Breadbord pour arduino (avec Arduino)
module bBoard(){
    /*
    // Big bBoard Cédric
    cube([125,57,9.8]);
    // Arduino
    translate([0,57/2-9,9.8]){
        cube([45,18,6]);
    }*/
    
    // bBoard 30 pins
    cube([85,57,9.8]);
    // Arduino
    translate([0,57/2-9,9.8]){
        cube([45,18,6]);
    }
}

// Modules d'alim Arduino + contôle ventilo
module moteurs(){
    difference(){
        cube([43.5,43.5,27]);
        // Trous vis
        translate([43.5-1.9-2,1.9+2,-1]) cylinder(30,2,2);
        translate([1.9+2,43.5-1.9-2,-1]) cylinder(30,2,2);
        translate([1.9+2,1.9+2,-1]) cylinder(30,2,2);
        translate([43.5-1.9-2,43.5-1.9-2,-1]) cylinder(30,2,2);
    }
}

// Abaisseur tension LED
module abaisseur(){
    difference(){
        cube([23,48,12.5]);
        // Trous vis
        translate([1.6+1,6+1,-1]) cylinder(15,2,2);
        translate([23-1.6-1,48-6-1,-1]) cylinder(15,2,2);
    }
}

module driverLed(){
    
}

module encodeur(){
    union(){
        // Bas
        cube([14,13.5,7]);
        // Haut
        translate([7,6.75,7]){
            cylinder(18,3.4,3.4);
        }
    }
}

module ecran(){
    difference(){
        union(){
            color("purple") cube([63.11,37.83,6.3]);
            translate([63.27/2-51.12/2,37.59/2-34.87/2,6.3]){
                color("green") cube([51.12,34.87,2]);
            }
        }
        // Trous vis
        translate([0.9+1.2,1.2+1.2,-1]){
            cylinder(10,1.2,1.2);
        }
        translate([63.27-0.9-1.2,1.2+1.2,-1]){
            cylinder(10,1.2,1.2);
        }
        translate([63.27-0.9-1.2,37.83-1.2-1.2,-1]){
            cylinder(10,1.2,1.2);
        }
        translate([0.9+1.2,37.83-1.2-1.2,-1]){
            cylinder(10,1.2,1.2);
        }
    }
}

/*ventilateur();
translate([-25.5+15,-25.75+15,15]){
    ledRadiateur();
}*/

//ventilateur();
//moteurs();
//ecran();
encodeur();

//bBoard();
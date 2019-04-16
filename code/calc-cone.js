// Vérifier si la hauteur et le rayon du cône sont bonnes
function verifCone(h, r){
	// Rayon incident (réflexion)
	const i = Math.PI/2 - Math.atan(r / h);
	
	// Piluliers avec 5mm d'écart du cône la réflexion la
	// plus basse pour 80% de puissance
	const hSousPil = Math.tan(2 * Math.atan(r / h) - Math.PI/2) * r;
	const hauteur80 = h + hSousPil;

	// Le niveau 80% est-il sous les tubes (4.5cm) et au-dessus de 5cm ?
	if(hauteur80 <= 4.9 && hauteur80 >= 4.7){
		return [hauteur80, i];
	}else{
		return false;
	}
}

for(let h=0; h<4.5; h+=0.1){ // Faire varier la hauteur
	for(let r=0; r<4.5; r+=0.1){ // Faire varier le rayon
		const tests = verifCone(h, r);
		if(tests){ // Si les valeurs correspondent aux attentes
			console.log("H: "+h); // Hauteur
			console.log("R: "+r); // Rayon
			console.log("H80: "+tests[0]); // Niveau 80%
			console.log("i: "+tests[1]); // Angle incident
			console.log("-----");
		}
	}
}
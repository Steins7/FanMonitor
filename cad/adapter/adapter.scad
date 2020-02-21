$fn = 100;

// element dimensions
w = 60;
d = 30;
h = 4;

// aditionnal parameters
e = 5;

// internal values
we = w + e;
de = d + e;

translate([0,0,e/2]);
cube([we,de,e/3], true);
module tab() {
    translate([-e/4,0,(e/3+h)/2]) {
        cube([e/2,2*e,h], true);
        translate([-1.75*e,-e,h/2])
        rotate([0,-90,-90])
        linear_extrude(height = 2*e)
        polygon([[0,1.2*e],[e/4,1.2*e],[0,2*e],[e/2,2*e]],
                [[0,1,3,2]]);
    }
}

translate([we/2,-de/2+e,0])
tab();

translate([-we/2,0,0])
rotate([0,0,180])
tab();

translate([8,de/2,0])
rotate([0,0,90])
tab();

translate([0,-de/2,0])
rotate([0,0,-90])
tab();

$fn = 100;

// element dimensions
w = 59;
d = 28;
h = 4;

// aditionnal parameters
e = 3;

translate([0,0,e/2]);
cube([w,d,e/2], true);
module tab() {
    translate([-e/4,0,(e/2+h)/2]) {
        cube([e/2,2*e,h], true);
        translate([-1.75*e,-e,h/2])
        rotate([0,-90,-90])
        linear_extrude(height = 2*e)
        polygon([[0,e],[e/4,e],[0,2*e],[e/2,2*e]],
                [[0,1,3,2]]);
    }
}

translate([w/2,-0.35*d,0])
tab();

translate([-w/2,0,0])
rotate([0,0,180])
tab();

translate([8,d/2,0])
rotate([0,0,90])
tab();

translate([0,-d/2,0])
rotate([0,0,-90])
tab();

//$fn = 100;

r = 12;
h = 12;

center_r = 3.2;

mol_r = 7;
mol_off = 6;

difference() {
    union() {
        difference() {
            cylinder(h=h, r=r, center=true);
            translate([0,0,h/2])
                cylinder(h=15, r=center_r, center=true);
            for(i=[0:45:359]) {
                echo((r+mol_off*sin(i)));
                translate([(r+mol_off)*cos(i),(r+mol_off)
                *sin(i),0])
                    cylinder(h=h, r=mol_r, center=true);
            }
        }
        translate([3,0,0])
            cube([3,2*center_r,h], center=true);
    }
    difference() {
        translate([0,0,0.7])
            cylinder(h=h, r=0.8*r, center=true);
        translate([0,0,0.7])
            cylinder(h=h, r=1.4*center_r, center=true);
        
    }
}
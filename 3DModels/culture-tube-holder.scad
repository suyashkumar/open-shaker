/*
culture-tube-holder.scad
A modular culture tube holder (in progress)
@author: Suyash Kumar
*/
$fn = 30;
// Parameters (all lens in mm) 

// Base
base_height = 50; 
base_x = 20;
base_y = 20;

// Tube
tube_cutout_height = 40; // Tube is 75 mm
tube_outer_diameter = 12; 

// Instrumentation
led_radius = 2;
led_tip_radius = 1.5; 
led_height = base_height - 25;

module make_base() {
    difference(){
        translate([0, 0, base_height/2])cube([base_x, base_y, base_height], center=true);
        translate([0, 0, base_height-tube_cutout_height])cylinder(d=12, h=tube_cutout_height);
    }
}

module instrumentation_volume() {
    
    union() {
        translate([0,base_y/2,led_height])rotate([90, 0, 0])cylinder(r=led_radius, h=(base_x/2)-tube_outer_diameter/2);
        mirror([0,1,0])translate([0,base_y/2,led_height])rotate([90, 0, 0])cylinder(r=led_radius, h=(base_x/2)-tube_outer_diameter/2);
    }

}

module make_holder() {
    difference() {
        make_base();
        instrumentation_volume();
    }
}

make_holder();
//instrumentation_volume();
screw_d = 4.2/3*4.2; // mm
distance_screw = 52; // mm
thinkness_srcew = 5;

screw_motor = 25;

thinkness = 3;


distance_axis = 62;
thinkness_axis = 16+1.4;
axis_thinkness_factor = 2;

difference() {
    //form
    //cube( [ distance_screw+thinkness_srcew*2, distance_screw+thinkness_srcew*2, //thinkness]);
    
    translate() {    
    // kreuz neu
    hull() {
        // loch mitte
        translate( [ distance_screw/2, distance_screw/2, 0])
            cylinder( h = thinkness, d = screw_motor*1.9);
        
        translate( [ distance_screw, distance_screw, 0 ])
                cylinder( d = distance_screw/3+thinkness_srcew/2, h = thinkness);
        translate( [ 0, 0, 0 ])
                cylinder( d = distance_screw/3+thinkness_srcew/2, h = thinkness);
    }
    
    translate( [ 0, distance_screw, thinkness])
    rotate( [ 180, 0, 0])
    hull() {
        // loch mitte
        translate( [ distance_screw/2+thinkness_srcew, distance_screw/2+thinkness_srcew, 0])
            cylinder( h = thinkness, d = screw_motor*1.9);
        
        translate( [ distance_screw, distance_screw, 0 ])
                cylinder( d = distance_screw/3+thinkness_srcew/2, h = thinkness);
        translate( [ 0, 0, 0 ])
                cylinder( d = distance_screw/3+thinkness_srcew/2, h = thinkness);
    }
    }

/* old cube
    translate( [0, 0, -1 ]) {
        cube( [ distance_screw/3, distance_screw/3, thinkness+2]);
    }
    translate( [distance_screw/2+thinkness_srcew*2+distance_screw/6, 0, -1 ])
        cube( [ distance_screw/3, distance_screw/3, thinkness+2]);
    translate( [0, distance_screw/2+thinkness_srcew*2+distance_screw/6, -1 ])
        cube( [ distance_screw/3, distance_screw/3, thinkness+2]);
    translate( [distance_screw/2+thinkness_srcew*2+distance_screw/6, distance_screw/2+thinkness_srcew*2+distance_screw/6, -1 ])
        cube( [ distance_screw/3, distance_screw/3, thinkness+2]);
    
    }
  */  
    
    translate( [ distance_screw, 0, -1])
        cylinder( h = thinkness+2, $fn = 10, d = screw_d);
    translate( [ distance_screw, distance_screw, -1])
        cylinder( h = thinkness+2, $fn = 10, d = screw_d);
    translate( [ 0, distance_screw, -1])
        cylinder( h = thinkness+2, $fn = 10, d = screw_d);
    translate( [ 0, 0, -1])
        cylinder( h = thinkness+2, $fn = 10, d = screw_d);
        
    // loch mitte
    translate( [ distance_screw/2, distance_screw/2, -1])
        cylinder( h = thinkness+2, d = screw_motor);
    
}


difference() {
    translate() { 
        hull() {
            translate( [ distance_screw/2, -distance_axis, 0])
                cylinder( d = thinkness_axis*1.8, h = thinkness);
            translate( [ distance_screw/2, -thinkness_axis/1.8, 0])
                cylinder( d = thinkness_axis*2.3, h = thinkness);
        }
        hull() {
            translate( [ distance_screw/2, -distance_axis, 0])
                cylinder( d = thinkness_axis*1.8, h = thinkness*axis_thinkness_factor);
            translate( [ distance_screw/2, -thinkness_axis/0.5, 0])
                cylinder( d = thinkness_axis*2.06, h = thinkness);
        }
    }
    translate( [ distance_screw/2, -distance_axis, -1])
        cylinder( d = thinkness_axis, h = thinkness*axis_thinkness_factor+2);
}

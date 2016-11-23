space_inside = 48;
space_inside_long = 72;
space_inside_high  = 30;

space = 4;


difference() {
hull() {
translate( [ 0, -2, 2])
    rotate( [-90, 0, 0])
        cylinder( d = space+1, $fn = 10, h = space_inside_long+4);
    translate( [ 0, -2, space_inside_high-space])
    rotate( [-90, 0, 0])
        cylinder( d = space+1, $fn = 10, h = space_inside_long+4);
    
    
translate( [ space_inside, -2, 2])
    rotate( [-90, 0, 0])
        cylinder( d = space+1, $fn = 10, h = space_inside_long+4);
    
    translate( [ space_inside, -2, space_inside_high-space])
    rotate( [-90, 0, 0])
        cylinder( d = space+1, $fn = 10, h = space_inside_long+4);
    
translate( [ space_inside/2, -2, 0])
    rotate( [-90, 0, 0])
        cylinder( d = space, $fn = 10, h = space_inside_long+4);
    
    // bakcside
    translate( [0, -2, 0])
    rotate( [0, 90, 0])
    cylinder( d = space, $fn = 10, h = space_inside);
    translate( [0, -2, 0])
    rotate( [0, 90, 0])
    cylinder( d = space, $fn = 10, h = space_inside);
    
    
}
cube( [space_inside, space_inside_long, space_inside_high] );

}

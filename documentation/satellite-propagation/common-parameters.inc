usepackage("latex-commands");

// calculate size in inches (due to IEEE latex template):
//real in = 2.54cm;
//size(0, 2.5in);

size(8cm, Aspect);

// font size:
defaultpen(fontsize(7));

real Arrow_s = 5; // arrow size
pen point_color = black;

// origin:
dot("$\Origin$", (-14, -6), W);

// x0:
pair x0 = (0,-3);

// plot x_0:
dot("$\rx_0$", x0, W);

// street
pair street_direction = (-2, 4);
pair street(real t) {
	return x0 + t * street_direction;
}

// x(t):
pair xp = street(2.5); // x(t = 1.5)
draw(street(-1)--street(4));

// x(t) 
dot(xp, point_color);
label("$\rx(t)=\bl{x}_0+\rxspeed t$", xp, W);

// canopy volume (potatoe-like shaped):
real ca = 11; // approx. dimension of it
pair cc = (20, 19); // approx. center of it
real facx = .8;
real facy = .4;
path canopy_volume = cc+(-ca, 0){dir(-55)}..cc+(facx * ca, facy * -ca){dir(20)}..cc+(facx * ca, facy * ca){dir(40)}..cc+(0, .8ca)..cc+(-0.9ca, .7ca)..cycle;
//path canopy_volume = cc+(-ca, 0){dir(-55)}..cc+(.7ca, -.5ca){dir(20)}..cc+(.7ca, .5ca){dir(40)}..cc+(0, ca)..cc+(-0.9ca, .8ca)..cycle;
// dot(canopy_volume); // draw dots of Bezier curve
draw(canopy_volume, longdashed + deepgreen);
label("$\CanVol$", canopy_volume , NW, deepgreen);

// canopy center of gravity:
pair z0 = (22, 18);

// z_\ell
pair zl = z0 + (-10, 5);
label("$\slo$", zl, N);
dot(zl, point_color);

// transmitter T
pair zt = (34, -5);
dot(zt, point_color);
label("$\tx$", zt, E);

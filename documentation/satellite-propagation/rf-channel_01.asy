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
dot("$\Origin$", (-2, -2), W);

// x0:
pair x0 = (0,-1);

// plot x_0:
dot("$\rx(t_n)$", x0, W);

// street
pair street_direction = (1, 4);
pair street(real t) {
	return x0 + t * street_direction;
}

// x(t):
pair xp = street(.5); // x(t = 1.5)
draw(street(-.2)--street(1.5));

// x(t)
dot(xp, point_color);
label("$\rx(t_{n+1})$", xp, W);

// z_\ell
pair zl = (3, 3);
label("$\slo$", zl, N);
dot(zl, point_color);

// transmitter T
pair zt = (24000, 24000);
//dot(zt, point_color);
//label("$\tx$", zt, E);

real long_arrow_length = 3;

// draw long arrow x_0 to T:
pair e_x0_T = (zt - x0) / abs(zt - x0);
draw(x0--(x0 + long_arrow_length * e_x0_T), E, black, Arrow); //Arrow(Arrow_s)
label("to $\tx$", (x0 + long_arrow_length * e_x0_T), E, black);

// unit vector x_0 to T:
draw(x0--(x0+e_x0_T), E, blue, Arrow); //Arrow(Arrow_s)

// draw long arrow r to T:
pair e_r_T = (zt - zl) / abs(zt - zl);
draw(zl--(zl + long_arrow_length * e_r_T), E, black, Arrow); //Arrow(Arrow_s)
label("to $\tx$", (zl + long_arrow_length * e_r_T), E, black);

// x_n to x_n+1:
pair xn_xnp1 = (xp-x0)

currentpicture.fit();
shipout(format="pdf");

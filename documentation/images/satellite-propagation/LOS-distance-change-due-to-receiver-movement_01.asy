usepackage("latex-commands");

// calculate size in inches (due to IEEE latex template):
//real in = 2.54cm;
//size(200, 200);

real long_arrow_length = 5;

void draw_receiver(real idx, pair x0, pair xp, pair zt, string T_label, bool last_call) {

	// plot x_0:
	dot("$\rx(t_" + string(idx) + ")$", x0, E);

	// draw long arrow x_0 to T:
	pair e_x0_T = (zt - x0) / abs(zt - x0);
	draw(x0--(x0 + long_arrow_length * e_x0_T), E, black, Arrow);
	label(T_label, (x0 + long_arrow_length * e_x0_T), E, black);

	// unit vector x_0 to T:
	draw(x0--(x0+e_x0_T), blue, Arrow);
	label("$\bl{u}_{\rx,\tx(t_" + string(idx) + ")}$", x0+.8e_x0_T, SE, blue);

	// draw long arrow xp to T:
	pair e_xp_T = (zt - xp) / abs(zt - xp);

	// dashed lines perpendicular to "to T" arrows:
	// vector perpendicular to Tx direction:
	pair tx_perp_direction = (e_x0_T.y, -e_x0_T.x);

	// function for the perpendicular Tx direction:
	pair tx_perp(pair start_point, real t) {
		return start_point + t * tx_perp_direction;
	}

	real perp_length = 4;

	// perpendicular line through x0:
	draw(tx_perp(x0, -perp_length)--tx_perp(x0, perp_length), dashed);

	// x_n to x_n+1:
	pair xn_xnp1 = (xp-x0);
	//draw("$\bl{v}_{\rx(t_" + string(idx) + "),\rx(t_" + string(idx+1) + ")}$", x0--(x0 + xn_xnp1), W, blue, Arrow);
	draw("$\bl{v}_{\rx_{" + string(idx) + "," + string(idx+1) + "}}$", x0--(x0 + xn_xnp1), W, blue, Arrow);

	pair delta_0 = dot(e_xp_T, xn_xnp1);

	// start point for Delta arrow:
	pair delta_rx = abs(delta_0) * e_x0_T;
	pair delta_rx_start = xp - delta_rx;

	draw(delta_rx_start--(delta_rx_start + delta_rx), red, Arrow);
	label("$\Delta_\rx(t_" + string(idx) + ")$", (delta_rx_start + .5delta_rx), NW, red);

	if (last_call == true) {
		// perpendicular line through xp:
		draw(tx_perp(xp, -perp_length)--tx_perp(xp, perp_length), dashed);

		// draw last point:
		dot(xp);
		label("$\rx(t_2)$", xp, E);

		// draw last T arrow:
		pair e_xp_T = (zt - xp) / abs(zt - xp);
		draw(xp--(xp + long_arrow_length * e_xp_T), E, black, Arrow);
		label("to $\tx(t_" + string(idx) + ")$, $\tx(t_" + string(idx+1) + ")$", (xp + long_arrow_length * e_xp_T), E, black);
	}

}

size(10cm, Aspect);

// font size:
defaultpen(fontsize(7));

real Arrow_s = 5; // arrow size
pen point_color = black;

// origin:
dot("$\Origin$", (-2, -2), W);

// x0:
pair x0 = (0,-1);

// street
pair street_direction = (1, 6);
pair street(real t) {
	return x0 + t * street_direction;
}

// x(t):
pair xp = street(.7); // x(t = 1.5)
pair xp2 = street(.9) + (-1, 3); // x(t = 1.5)

// transmitter T
pair zt = (24000, 24000);
//dot(zt, point_color);
//label("$\tx$", zt, E);

draw_receiver(0, x0, xp, zt, "to $\tx(t_0)$", false);
draw_receiver(1, xp, xp2, zt, "to $\tx(t_0)$, $\tx(t_1)$", true);

currentpicture.fit();
shipout(format="pdf");
// shipout(format="png");
// converted to PNG with
// convert -density 300 LOS-distance-change-due-to-receiver-movement_01.pdf -size 800x -flatten LOS-distance-change-due-to-receiver-movement_01.png

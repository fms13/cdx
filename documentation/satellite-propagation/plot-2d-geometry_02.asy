include 'common-parameters.inc';

// z_T to z_\ell
draw(zt--zl, blue+dashed); //Arrow(Arrow_s)
label("$\di(\slo)$", zt--zl, 2S+W, blue);

// zl to x
draw("$\dii(t, \slo)$", zl--xp, blue+dashed); //Arrow(Arrow_s)

label("$\CenOfGrav$", z0, NE);
dot(z0, point_color);

// x(t) to canopy center:
draw("$\DistRxCenter(t)$", xp--z0, SE, black);

// canopy center to \slo_\ell
draw("$\bl{\tilde{r}}$", z0--zl, Arrow(Arrow_s));

// z_T to z_0
draw("$\DistTxCenter$", zt--z0, black);

// e_z_T,0:
pair ezend = zt + 4 * (z0-zt) / length(z0-zt);
draw("$\eztn$", zt--ezend, NE, red, Arrow(Arrow_s));

// e_d_0:
pair edend = xp + 4 * (z0-xp) / length(z0-xp);
draw("$\exrc$", xp--edend, SE, red, Arrow(Arrow_s));
currentpicture.fit();
shipout(format="pdf");

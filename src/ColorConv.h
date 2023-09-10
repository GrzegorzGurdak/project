#pragma once

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

namespace ColorConv {

	sf::Color hslToRgb(double h, double s, double l)
	{
		double c = (1 - fabs(2 * l - 1)) * s;
		double hp = h * 360 / 60;
		double x = c * (1 - fabs(fmod(hp, 2) - 1));
		double r1, g1, b1;
		if (0 < hp && hp < 1) { r1 = c; g1 = x; b1 = 0; }
		else if (hp < 2) { r1 = x; g1 = c; b1 = 0; }
		else if (hp < 3) { r1 = 0; g1 = c; b1 = x; }
		else if (hp < 4) { r1 = 0; g1 = x; b1 = c; }
		else if (hp < 5) { r1 = x; g1 = 0; b1 = c; }
		else if (hp <= 6) { r1 = c; g1 = 0; b1 = x; }

		double m = l - c / 2;

		//std::cout << "c=" << c << "\n";
		//std::cout << "x=" << x << "\n";
		//std::cout << "m=" << m << "\n";
		return sf::Color((r1 + m) * 255, (g1 + m) * 255, (b1 + m) * 255);
	}

	sf::Color hsvToRgb(double h, double s, double v)
	{
		double c = v * s;
		double hp = h * 360 / 60;
		double x = c * (1 - fabs(fmod(hp, 2) - 1));
		double r1, g1, b1;
		if (0 < hp && hp < 1) { r1 = c; g1 = x; b1 = 0; }
		else if (hp < 2) { r1 = x; g1 = c; b1 = 0; }
		else if (hp < 3) { r1 = 0; g1 = c; b1 = x; }
		else if (hp < 4) { r1 = 0; g1 = x; b1 = c; }
		else if (hp < 5) { r1 = x; g1 = 0; b1 = c; }
		else if (hp <= 6) { r1 = c; g1 = 0; b1 = x; }

		double m = v - c;

		//std::cout << "c=" << c << "\n";
		//std::cout << "x=" << x << "\n";
		//std::cout << "m=" << m << "\n";
		return sf::Color((r1 + m) * 255, (g1 + m) * 255, (b1 + m) * 255);
	}

	sf::Color cmyToRgb(double c, double m, double y)
	{
		double k = 0;
		double r, g, b;
		r = 255 * (1 - c) * (1 - k);
		g = 255 * (1 - m) * (1 - k);
		b = 255 * (1 - y) * (1 - k);


		//std::cout << "c=" << c << "\n";
		//std::cout << "x=" << x << "\n";
		//std::cout << "m=" << m << "\n";
		return sf::Color(r, g, b);
	}

}
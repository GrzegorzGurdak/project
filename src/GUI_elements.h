#pragma once

#include <SFML/Graphics.hpp>

class SimStat : public sf::Drawable {
public:

	SimStat(sf::Font& font) :
		objectAmountText{ sf::Text("0", font) }, simTime{ sf::Text("", font) }, fps_text{ sf::Text("", font)}, youWonCongratulation{ sf::Text("You won!", font) }
	{
		simTime.setPosition(0, 30);
		fps_text.setPosition(0, 60);
		youWonCongratulation.setPosition(350, 350);
		youWonCongratulation.setCharacterSize(50);
	}
	void objectAmountUpdate(size_t objectAmount) { objectAmountText.setString(std::to_string(objectAmount)); }
	void simTimeAdd(int addSimTimeValue) { simTimeValue += addSimTimeValue; }

	void update(sf::Event& event) {
		if (fClock.getElapsedTime().asSeconds() >= 1.0f)
		{
			if (frame_counter != 0) {
				simTime.setString(std::to_string(simTimeValue / frame_counter) + "us");
				simTimeValue = 0;
			}

			FPS = (unsigned int)((float)frame_counter / fClock.getElapsedTime().asSeconds());
			fps_text.setString("fps: " + std::to_string(FPS));
			fClock.restart();
			frame_counter = 0;
		}
		frame_counter++;
	}


	void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
		target.draw(objectAmountText,states);
		target.draw(simTime, states);
		target.draw(fps_text, states);
		if (winStatus) {
			target.draw(youWonCongratulation, states);
		}
	}

	void setWinStatus(bool status) { winStatus = status; }

	// bool isLMBPressed() { return isLMBPressed; }
	// bool isRMBPressed() { return isRMBPressed; }
	// bool isShiftPressed() { return isShiftPressed; }
	// bool isSpacePressed() { return isSpacePressed; }
	// bool isCtrlPressed() { return isCtrlPressed; }


private:
	bool winStatus = false;
	sf::Clock fClock;

	unsigned int FPS = 0, frame_counter = 0;
	int simTimeValue{}, frame_count{};

	sf::Text objectAmountText;
	sf::Text simTime;
	sf::Text fps_text;
	sf::Text youWonCongratulation;

	bool isLMBPressed = false;
	bool isRMBPressed = false;
	bool isShiftPressed = false;
	bool isSpacePressed = false;
	bool isCtrlPressed = false;
};



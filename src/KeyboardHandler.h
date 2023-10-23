#ifndef KEYBOARDHANDLER_H
#define KEYBOARDHANDLER_H

#include <SFML/Graphics.hpp>
#include <functional>

#include "MVec.h"

class KeyboardHandler {
public:
    KeyboardHandler(sf::RenderWindow& window): window{window} {}; //TODO: get window instead of event
    ~KeyboardHandler() = default;

    void update();
    void handleKeyboard(sf::Event& event);

    std::function<void(void)> resize = [](void) {};

    std::function<void(KeyboardHandler&)> mouseMoved = [](KeyboardHandler&) {};

    std::function<void(KeyboardHandler&)> pressedLMB = [](KeyboardHandler&) {};
    std::function<void(KeyboardHandler&)> pressedRMB = [](KeyboardHandler&) {};

    std::function<void(KeyboardHandler&)> onLMB = [](KeyboardHandler&) {};
    std::function<void(KeyboardHandler&)> onRMB = [](KeyboardHandler&) {};

    std::function<void(KeyboardHandler&)> releasedLMB = [](KeyboardHandler&) {};
    std::function<void(KeyboardHandler&)> releasedRMB = [](KeyboardHandler&) {};

    std::function<void(KeyboardHandler&)> pressedUp = [](KeyboardHandler&) {};
    std::function<void(KeyboardHandler&)> pressedDown = [](KeyboardHandler&) {};
    std::function<void(KeyboardHandler&)> pressedLeft = [](KeyboardHandler&) {};
    std::function<void(KeyboardHandler&)> pressedRight = [](KeyboardHandler&) {};
    std::function<void(KeyboardHandler&)> pressedL = [](KeyboardHandler&) {};

    std::function<void(KeyboardHandler&)> onUp = [](KeyboardHandler&) {};
    std::function<void(KeyboardHandler&)> onDown = [](KeyboardHandler&) {};
    std::function<void(KeyboardHandler&)> onLeft = [](KeyboardHandler&) {};
    std::function<void(KeyboardHandler&)> onRight = [](KeyboardHandler&) {};
    std::function<void(KeyboardHandler&)> onDash = [](KeyboardHandler&) {};
    std::function<void(KeyboardHandler&)> onEqual = [](KeyboardHandler&) {};

    std::function<void(KeyboardHandler&)> releasedUp = [](KeyboardHandler&) {};
    std::function<void(KeyboardHandler&)> releasedDown = [](KeyboardHandler&) {};
    std::function<void(KeyboardHandler&)> releasedLeft = [](KeyboardHandler&) {};
    std::function<void(KeyboardHandler&)> releasedRight = [](KeyboardHandler&) {};

    bool lmbPressed = false;
    bool rmbPressed = false;
    Vec2 mousePosition;

    bool upState = false;
    bool downState = false;
    bool leftState = false;
    bool rightState = false;
    bool dashState = false;
    bool equalState = false;
    bool shiftState = false;
    bool ctrlState = false;
private:
    sf::RenderWindow& window;
};

#endif // KEYBOARDHANDLER_H
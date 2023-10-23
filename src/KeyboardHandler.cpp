#include "KeyboardHandler.h"

void KeyboardHandler::update() {
    if (lmbPressed) onLMB(*this);
    if (rmbPressed) onRMB(*this);

    if (upState) onUp(*this);
    if (downState) onDown(*this);
    if (leftState) onLeft(*this);
    if (rightState) onRight(*this);
    if (dashState) onDash(*this);
    if (equalState) onEqual(*this);

    sf::Event event;

    while (window.pollEvent(event)) {
        handleKeyboard(event);
    }
}

void KeyboardHandler::handleKeyboard(sf::Event& event) {
    if (event.type == sf::Event::Closed) window.close();

    else if (event.type == sf::Event::MouseButtonPressed){
        if(event.mouseButton.button == sf::Mouse::Left){
            lmbPressed = true;
            mousePosition.set(event.mouseButton.x, event.mouseButton.y);
            pressedLMB(*this);
        }
        else if(event.mouseButton.button == sf::Mouse::Right){
            rmbPressed = true;
            mousePosition.set(event.mouseButton.x, event.mouseButton.y);
            pressedRMB(*this);
        }
    }
    else if (event.type == sf::Event::MouseButtonReleased){
        if(event.mouseButton.button == sf::Mouse::Left){
            lmbPressed = false;
            mousePosition.set(event.mouseButton.x, event.mouseButton.y);
            releasedLMB(*this);
        }
        else if(event.mouseButton.button == sf::Mouse::Right){
            rmbPressed = false;
            mousePosition.set(event.mouseButton.x, event.mouseButton.y);
            releasedRMB(*this);
        }
    }
    else if (event.type == sf::Event::MouseMoved) {
        mousePosition.set(event.mouseMove.x, event.mouseMove.y);
        mouseMoved(*this);
    }
    else if (event.type == sf::Event::MouseMoved) {
        mousePosition.set(event.mouseMove.x, event.mouseMove.y);
        if (lmbPressed) onLMB(*this);
        if (rmbPressed) onRMB(*this);
    }

    else if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Escape)
            window.close();
        else if (event.type == sf::Event::Resized) {
            resize();
        }
        else{
            switch (event.key.code) {
                case sf::Keyboard::LShift:
                    shiftState = true;
                    break;
                case sf::Keyboard::LControl:
                    ctrlState = true;
                    break;
                case sf::Keyboard::Up:
                    pressedUp(*this);
                    upState = true;
                    break;
                case sf::Keyboard::Down:
                    pressedDown(*this);
                    downState = true;
                    break;
                case sf::Keyboard::Left:
                    pressedLeft(*this);
                    leftState = true;
                    break;
                case sf::Keyboard::Right:
                    pressedRight(*this);
                    rightState = true;
                    break;
                case sf::Keyboard::Dash:
                    dashState = true;
                    break;
                case sf::Keyboard::Equal:
                    equalState = true;
                    break;
                case sf::Keyboard::L:
                    pressedL(*this);
                    break;
                default:
                    break;
            }
        }
    }
    else if (event.type == sf::Event::KeyReleased) {
        switch (event.key.code) {
            case sf::Keyboard::LShift:
                shiftState = false;
                break;
            case sf::Keyboard::LControl:
                ctrlState = false;
                break;
            case sf::Keyboard::Up:
                releasedUp(*this);
                upState = false;
                break;
            case sf::Keyboard::Down:
                releasedDown(*this);
                downState = false;
                break;
            case sf::Keyboard::Left:
                releasedLeft(*this);
                leftState = false;
                break;
            case sf::Keyboard::Right:
                releasedRight(*this);
                rightState = false;
                break;
            case sf::Keyboard::Dash:
                dashState = false;
                break;
            case sf::Keyboard::Equal:
                equalState = false;
                break;
            default:
                break;
        }
    }
}
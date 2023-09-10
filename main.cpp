#include <SFML/Graphics.hpp>
#include <iostream>

int main()
{
    // sf::Clock Clock;
    // while (Clock.GetElapsedTime() < 5.f)
    // {
    //     std::cout << Clock.GetElapsedTime() << std::endl;
    //     sf::Sleep(0.5f);
    // }

    sf::RenderWindow window(sf::VideoMode(800, 600, 32), "SFML Window");
    sf::Event event;
    sf::err().rdbuf(NULL);
    while (window.isOpen())
    {
        window.clear();
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        window.display();
    }

    return 0;
}
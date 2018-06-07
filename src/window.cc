#include "window.h"
#include <SFML/Graphics.hpp>

Window::Window(int scaling_factor) : sf_(scaling_factor) {
    window = std::make_unique<sf::RenderWindow>(sf::VideoMode(size_*sf_, info_box_height_+size_*sf_), "");
}

void Window::Draw(const std::vector<geometry::Point>& poly, const std::string& info) {
    // Event loop
    sf::Event event;
    while (window->pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window->close();
        }
    }

    window->clear(sf::Color::White);

    int n = poly.size();
    for (int i = 0; i < n; i++) {

        // Draw a polygon edge
        int nxt = (i + 1) % n;
        sf::Vertex edge[] = {
            sf::Vertex(sf::Vector2f(poly[i].x * sf_, (40-poly[i].y) * sf_), sf::Color::Black),
            sf::Vertex(sf::Vector2f(poly[nxt].x * sf_, (40-poly[nxt].y) * sf_), sf::Color::Black),
        };
        window->draw(edge, 2, sf::Lines);

        // Draw a polygon vertex
        double radius = 5;
        sf::CircleShape vertex(radius);
        vertex.setPosition(poly[i].x * sf_ - radius, (40-poly[i].y) * sf_ - radius);
        vertex.setFillColor(sf::Color::Black);
        window->draw(vertex);

        // Draw info box
        sf::RectangleShape info_box(sf::Vector2f(size_*sf_, info_box_height_));
        info_box.setPosition(0, size_*sf_);
        info_box.setFillColor(sf::Color(200, 200, 200));
        window->draw(info_box);

        // Draw text inside
        sf::Font font;
        font.loadFromFile("assets/Courier.ttf");
        sf::Text text;
        text.setFont(font);
        text.setCharacterSize(24);
        text.setColor(sf::Color::Black);
        text.setStyle(sf::Text::Bold);
        text.setString(info);
        text.setPosition(sf::Vector2f(0, size_*sf_));
        window->draw(text);
    }
    window->display();
}

void Window::Wait() {
    sf::Event event;
    while (window->isOpen()) {
        while (window->pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window->close();
            }
        }
    }
}

void Window::SaveToPhoto(const std::string& filename) {
    sf::Vector2u windowSize = window->getSize();
    sf::Texture texture;
    texture.create(windowSize.x, windowSize.y);
    texture.update(*window);
    sf::Image screenshot = texture.copyToImage();
    screenshot.saveToFile(filename);
}
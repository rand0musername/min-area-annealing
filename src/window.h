#pragma once
#include "geometry.h"
#include <vector>
#include <SFML/Graphics.hpp>

class Window {
public:
    Window(int scaling_factor);
    void Draw(const std::vector<geometry::Point>& poly, const std::string& info);
    void Wait();
    void SaveToPhoto(const std::string& filename);

private:
    // Max poly coordinate in test cases
    const int size_ = 40;
    const int info_box_height_ = 140;  

    int sf_;
    std::unique_ptr<sf::RenderWindow> window;
};
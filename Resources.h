#pragma once

#include <SFML/Graphics.hpp>
#include <unordered_map>
//хранилище текстур
class Resources
{
public:
	static std::unordered_map<std::string, sf::Texture> textures;
};


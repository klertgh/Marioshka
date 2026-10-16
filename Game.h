#pragma once
#include <SFML/Graphics.hpp>
#include "Renderer.h"
#include "Object.h"
#include "Camera.h"
#include "Mario.h"
// подключение глобальных переменных определённых в других частях программы
extern Camera camera;  
extern bool paused;

extern std::unique_ptr<Mario> mario;

void Begin(const sf::Window& window);
void Update(float deltaTime);
void Render(Renderer& renderer);
void RenderUI(Renderer& renderer);

void DeleteObject(Object* object);

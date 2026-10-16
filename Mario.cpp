#include "Mario.h"
#include "Object.h"
#include "Resources.h"

#include <box2d.h>
#include <iostream>
#include "Game.h"
#include "Enemy.h"

constexpr float M_PI = 22.0f / 7.0f;
const float movementSpeed = 7.0f;
const float jumpVelocity = 9.0f;


void Mario::Begin()
{
	runAnimation = Animation(0.45f,
		{
			AnimFrame(0.30f, Resources::textures["run3.png"]),
			AnimFrame(0.15f, Resources::textures["run2.png"]),
			AnimFrame(0.00f, Resources::textures["run1.png"]),
		});

	fixtureData.type = FixtureDataType::Mario;
	fixtureData.listener = this;
	fixtureData.mario = this;

	b2BodyDef bodyDef{};
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(position.x, position.y);
	bodyDef.fixedRotation = true; // фиксация поворота марио
	body = Physics::world->CreateBody(&bodyDef);

	b2FixtureDef fixtureDef{};;
	fixtureDef.userData.pointer = (uintptr_t)&fixtureData;
	fixtureDef.density = 1.0f; // плотность
	fixtureDef.friction = 0.0f; // трение

	//для плавного перемещения(без застревания)
	b2CircleShape circleShape{};
	circleShape.m_radius = 0.5f;
	circleShape.m_p.Set(0.0f, -0.5f);
	fixtureDef.shape = &circleShape;
	body->CreateFixture(&fixtureDef);
	circleShape.m_p.Set(0.0f, 0.5f);
	body->CreateFixture(&fixtureDef);
	//для физ. столкновения
	b2PolygonShape polygonShape{};
	polygonShape.SetAsBox(0.5f, 0.5f);
	fixtureDef.shape = &polygonShape;
	body->CreateFixture(&fixtureDef);
	//сенсор для проверки касания с землей
	polygonShape.SetAsBox(0.4f, 0.2f, b2Vec2(0.0f, 1.0f), 0.0f);
	fixtureDef.isSensor = true;
	groundFixture = body->CreateFixture(&fixtureDef);

}
//управление и движение марио
void Mario::Update(float deltaTime)
{
	float move = movementSpeed;

	runAnimation.Update(deltaTime);
	// вектор скорости(по x и y)
	b2Vec2 velocity = body->GetLinearVelocity();
	velocity.x = 0.0f;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		velocity.x += move;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
		velocity.x -= move;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && (isGrounded))
		velocity.y = -jumpVelocity;

	textureToDraw = runAnimation.GetTexture();

	// поворот марио
	if (velocity.x < -0.02f)
		facingLeft = true;
	else if (velocity.x > 0.02f)
		facingLeft = false;
	else
		textureToDraw = Resources::textures["stop.png"];

	if (!isGrounded)
		textureToDraw = Resources::textures["jump.png"];

	body->SetLinearVelocity(velocity); // обновление скорости

	position = sf::Vector2f(body->GetPosition().x, body->GetPosition().y);
	angle = body->GetAngle() * (180.0f / M_PI); // угол

}

void Mario::Draw(Renderer& renderer)
{
	renderer.Draw(textureToDraw, position, 
		sf::Vector2f(facingLeft ? -1.0f : 1.0f, 2.0f), angle); // если поворот марио влево ->
	                                                           // переворачиваем текстуру влево(-1.0 по X) и т.п.
}

void Mario::OnBeginContact(b2Fixture* self, b2Fixture* other)
{
	// получение информации о др объекте
	FixtureData* data = (FixtureData*)other->GetUserData().pointer;

	if (!data)
		return;
	//проверка на столкновение с землей (для прыжка)
	if (groundFixture == self && data->type == FixtureDataType::MapTile)
		isGrounded++;
	//проверка столкновения с монетой
	else if (data->type == FixtureDataType::Object && data->object->tag == "coin") {
		
		DeleteObject(data->object);
		std::cout << "coins = " << ++coins << "\n";
	}
	//проверка столкновения с врагом
	else if (data->type == FixtureDataType::Object && data->object->tag == "enemy") {
		//столкновение сверху
		if (groundFixture == self) {

			Enemy* enemy = dynamic_cast<Enemy*>(data->object);
			if (enemy)
				enemy->Die();
		} //иначе марио умирает
		else {
			isDead = true;
		}

	}
}

void Mario::OnEndContact(b2Fixture* self, b2Fixture* other)
{
	FixtureData* data = (FixtureData*)other->GetUserData().pointer;

	if (!data)
		return;

	if (groundFixture == self && data->type == FixtureDataType::MapTile && isGrounded > 0)
		isGrounded--;
}

size_t Mario::GetCoins()
{
	return coins;
}

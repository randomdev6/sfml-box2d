#include <SFML/Graphics.hpp>
#include <SFML/Window/Event.hpp>
#include <box2d/box2d.h>
#include <memory>
#include <iostream>
#include <cstring>
#include <vector>
#include "Util.h"

// ------------------------------------ Physics ------------------------------------ //

struct Physics
{
    Physics() : m_world(std::make_unique<b2World>(m_gravity))
    {
    }
    ~Physics(){}

    void update()
    {
        m_world->Step(timeStep, velocityIterations, positionIterations);
    }

    b2World* getWorld() { return m_world.get(); }

    float timeStep = 1.0f / 30.0f;
    int32 velocityIterations = 6;
    int32 positionIterations = 2;

    b2Vec2 m_gravity{ 0.0f, 5.f };
    std::unique_ptr<b2World> m_world;
};


// ------------------------------------ Box ------------------------------------ //

struct Box
{
    Box(b2World* worldPtr)
        : world(worldPtr),
          position(500.f, 100.f), dimension(40.f, 40.f)
    {
        init();
    }

    Box(b2World* worldPtr, sf::Vector2f& pos)
        : world(worldPtr),
          position(pos), dimension(40.f, 40.f)
    {
        init();
    }

    Box(const Box& other)
    {
        this->position = other.position;
        this->dimension = other.dimension;
        this->box = other.box;
        this->world = other.world;
        this->body = other.body;
    }

    ~Box() {}

    void init() {
        // sfml
        box.setOrigin(dimension.x / 2, dimension.y / 2);
        box.setPosition(position);
        box.setSize(dimension);
        box.setFillColor(randomColor());

        // create body definition
        b2BodyDef bdef;
        bdef.type = b2BodyType::b2_dynamicBody;
        bdef.position.Set(toMeter(position.x), toMeter(position.y));

        // create body in the world
        body = world->CreateBody(&bdef);
        
        // create shape
        b2PolygonShape shape;
        shape.SetAsBox(toMeter(dimension.x) / 2, toMeter(dimension.y) / 2);

        // fixture definition
        b2FixtureDef fdef;
        fdef.shape = &shape;

        // fixture properties
        fdef.density = 0.002f;
        fdef.friction = 0.3f;
        fdef.restitution = 0.5f;

        // add fixture to body
        body->CreateFixture(&fdef);
    }

    void updateControls()
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            position.x -= vel;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            position.x += vel;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            position.y -= vel;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            position.y += vel;
        }

        //body->GetContactList();
        b2Vec2 pos(toMeter(position.x), toMeter(position.y));
        body->SetTransform(pos, body->GetAngle());
    }

    void update()
    {
        position.x = toPixel(body->GetPosition().x);
        position.y = toPixel(body->GetPosition().y);
        this->box.setPosition(position);

        updateControls();
    }

    void render(sf::RenderWindow& window) {
        window.draw(box);
    }

    float vel = 5.f;
    sf::Vector2f position;
    sf::Vector2f dimension;
    sf::RectangleShape box;
    b2World* world;
    b2Body* body;
};

// ------------------------------------ Ground ------------------------------------ //

struct Ground
{
    Ground(b2World* worldPtr)
        : ground(new sf::RectangleShape()), world(worldPtr),
          position(500.f, 700.f), dimension(500.f, 30.f)
    {
        init();
    }

    void init() {
        // sfml
        ground->setOrigin(dimension.x / 2, dimension.y / 2);
        ground->setPosition(position);
        ground->setSize(dimension);
        ground->setFillColor(sf::Color::White);

        // create body definition
        b2BodyDef bdef;
        bdef.type = b2BodyType::b2_staticBody;
        bdef.position.Set(toMeter(position.x), toMeter(position.y));

        // create body in the world
        b2Body* body = world->CreateBody(&bdef);
        
        // create shape
        shape = new b2PolygonShape();
        shape->SetAsBox(toMeter(dimension.x) / 2, toMeter(dimension.y) / 2);

        // fixture definition
        fdef = new b2FixtureDef();
        fdef->shape = shape;

        // add fixture to body
        body->CreateFixture(fdef);
    }

    ~Ground() {
        delete ground;
        delete shape;
        delete fdef;
        ground = nullptr;
        shape = nullptr;
        fdef = nullptr;
    }

    void render(sf::RenderWindow& window) { window.draw(*ground); }

    sf::Vector2f dimension;
    sf::Vector2f position;
    sf::RectangleShape* ground;
    b2World* world;
    // b2Body* body;
    b2PolygonShape* shape;
    b2FixtureDef* fdef;
};


// ------------------------------------ Main ------------------------------------ //


int main()
{
    sf::RenderWindow window(sf::VideoMode(1000, 800), "sfml-box2d", sf::Style::Default);
    window.setFramerateLimit(60);

    Physics physics;
    Box box(physics.getWorld());
    Ground ground(physics.getWorld());

    while (window.isOpen())
    {
        if (true)
        {
            sf::Event event;
            while (window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
                    window.close();
            }

            // script update
            box.update();

            // update physics
            physics.update();

            window.clear();
            ground.render(window);
            box.render(window);
            window.display();
        }
    }
}

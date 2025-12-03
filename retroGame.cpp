#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <cmath>
#include <optional>
#include <string>

// --- Constants ---
const float GRAVITY = 0.6f;
const float MOVE_SPEED = 0.6f;
const float MAX_SPEED = 5.0f;
const float JUMP_FORCE = -13.0f;
const float FRICTION = 0.85f;

// --- Structs ---
struct Token {
    sf::CircleShape shape;
    bool collected = false;

    Token(float x, float y) {
        shape.setRadius(10.f);
        shape.setFillColor(sf::Color::Yellow);
        shape.setOutlineThickness(2);
        shape.setOutlineColor(sf::Color(212, 175, 55)); // Gold
        shape.setPosition({x, y});
    }
};

struct Platform {
    sf::RectangleShape shape;

    Platform(float x, float y, float w, float h) {
        shape.setPosition({x, y});
        shape.setSize({w, h});
        shape.setFillColor(sf::Color(107, 140, 66)); // Grass Green
        shape.setOutlineThickness(2);
        shape.setOutlineColor(sf::Color(64, 84, 40));
    }
};

struct Player {
    sf::RectangleShape shape;
    sf::Vector2f velocity;
    bool isGrounded = false;

    Player() {
        shape.setSize({30.f, 30.f});
        shape.setFillColor(sf::Color::Red);
        shape.setPosition({50.f, 400.f});
        velocity = {0.f, 0.f};
    }

    void update() {
        // Apply Gravity
        velocity.y += GRAVITY;

        // Apply Friction (X-axis only)
        velocity.x *= FRICTION;

        // Move
        shape.move(velocity);

        // Screen Boundaries
        sf::Vector2f pos = shape.getPosition();
        sf::Vector2f size = shape.getSize();

        if (pos.x < 0)
            shape.setPosition({0, pos.y});
        if (pos.x + size.x > 800)
            shape.setPosition({800 - size.x, pos.y});

        // Reset if fell off screen
        if (pos.y > 600) {
            shape.setPosition({50, 400});
            velocity = {0, 0};
        }
    }
};

// --- Collision Logic ---
void checkCollision(Player& p, const Platform& plat) {
    sf::FloatRect playerBounds = p.shape.getGlobalBounds();
    sf::FloatRect wallBounds = plat.shape.getGlobalBounds();

    // SFML 3.0: findIntersection returns std::optional
    // We check if it has a value (true) or is nullopt (false)
    if (playerBounds.findIntersection(wallBounds)) {

        // Calculate overlap manually
        float overlapLeft = (playerBounds.position.x + playerBounds.size.x) - wallBounds.position.x;
        float overlapRight = (wallBounds.position.x + wallBounds.size.x) - playerBounds.position.x;
        float overlapTop = (playerBounds.position.y + playerBounds.size.y) - wallBounds.position.y;
        float overlapBottom = (wallBounds.position.y + wallBounds.size.y) - playerBounds.position.y;

        float minOverlapX = std::min(overlapLeft, overlapRight);
        float minOverlapY = std::min(overlapTop, overlapBottom);

        if (minOverlapX < minOverlapY) {
            if (overlapLeft < overlapRight) {
                p.shape.move({-overlapLeft, 0});
                p.velocity.x = 0;
            } else {
                p.shape.move({overlapRight, 0});
                p.velocity.x = 0;
            }
        } else {
            if (overlapTop < overlapBottom) {
                p.shape.move({0, -overlapTop});
                p.velocity.y = 0;
                p.isGrounded = true;
            } else {
                p.shape.move({0, overlapBottom});
                p.velocity.y = 0;
            }
        }
    }
}

int main() {
    // SFML 3.0: VideoMode expects unsigned ints (u)
    sf::RenderWindow window(sf::VideoMode({800u, 600u}), "Retro Platformer C++");
    window.setFramerateLimit(60);

    // --- Font Handling ---
    sf::Font font;
    bool hasFont = false;

    // FIX 1: Use openFromFile instead of loadFromFile
    if (font.openFromFile("arial.ttf")) {
        hasFont = true;
    } else {
        std::cerr << "Failed to load font (arial.ttf). Score will not display." << std::endl;
    }

    sf::Text scoreText(font);
    if (hasFont) {
        scoreText.setCharacterSize(24);
        scoreText.setFillColor(sf::Color::White);
        scoreText.setPosition({20, 20});
    }

    // --- Game Objects ---
    Player player;
    int score = 0;

    std::vector<Platform> platforms;
    platforms.reserve(5);
    platforms.emplace_back(0.f, 550.f, 800.f, 50.f);    // Ground
    platforms.emplace_back(200.f, 450.f, 100.f, 20.f);
    platforms.emplace_back(400.f, 350.f, 100.f, 20.f);
    platforms.emplace_back(600.f, 250.f, 100.f, 20.f);
    platforms.emplace_back(100.f, 200.f, 80.f, 20.f);

    std::vector<Token> tokens;
    tokens.reserve(6);
    tokens.emplace_back(230.f, 410.f);
    tokens.emplace_back(440.f, 310.f);
    tokens.emplace_back(640.f, 210.f);
    tokens.emplace_back(130.f, 160.f);
    tokens.emplace_back(500.f, 510.f);
    tokens.emplace_back(700.f, 510.f);

    // --- Game Loop ---
    while (window.isOpen()) {
        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        // --- Input (SFML 3.0 Strict) ---
        // FIX 2: Removed "using sf::Keyboard" and used fully qualified names
        // FIX 3: Scoped Enum sf::Keyboard::Key::[KeyName]

        bool left = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) ||
                    sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A);

        bool right = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) ||
                     sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D);

        bool jump = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) ||
                    sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) ||
                    sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up);

        if (left) {
            if (player.velocity.x > -MAX_SPEED) player.velocity.x -= MOVE_SPEED;
        }
        if (right) {
            if (player.velocity.x < MAX_SPEED) player.velocity.x += MOVE_SPEED;
        }
        if (jump && player.isGrounded) {
            player.velocity.y = JUMP_FORCE;
            player.isGrounded = false;
        }

        // --- Physics ---
        player.update();
        player.isGrounded = false;

        for (auto& plat : platforms) {
            checkCollision(player, plat);
        }

        for (auto& t : tokens) {
            if (!t.collected) {
                // SFML 3 Optional Check
                if (player.shape.getGlobalBounds().findIntersection(t.shape.getGlobalBounds())) {
                    t.collected = true;
                    score += 10;
                    std::cout << "Score: " << score << std::endl;
                }
            }
        }

        if (hasFont) {
            scoreText.setString("Score: " + std::to_string(score));
        }

        // --- Drawing ---
        window.clear(sf::Color(92, 148, 252));

        for (const auto& plat : platforms) window.draw(plat.shape);
        for (const auto& t : tokens) if (!t.collected) window.draw(t.shape);

        window.draw(player.shape);

        if (hasFont) window.draw(scoreText);

        window.display();
    }

    return 0;
}
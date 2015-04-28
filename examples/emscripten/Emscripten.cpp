
////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <SFML/Audio.hpp>
#include <emscripten.h>
#include <stdexcept>
#include <cmath>
#include <ctime>
#include <cstdlib>


////////////////////////////////////////////////////////////
/// TCP socket test
///
////////////////////////////////////////////////////////////
void doSocket()
{
    static bool connectSent = false;
    static bool connected = false;
    static bool messageSent = false;
    static bool done = false;
    static sf::TcpSocket socket;
    const static std::string testString = "SFML Emscripten TCP socket test via WebSockets";

    if (!connectSent)
    {
        socket.connect("echo.websocket.org", 80, sf::seconds(10));
        connectSent = true;
    }

    if (!connected && socket.getRemoteAddress() != sf::IpAddress::None)
        connected = true;

    if (!messageSent && connected)
    {
        static std::size_t sent = 0;
        socket.send(testString.c_str() + sent, testString.size() - sent, sent);

        if (sent >= testString.size())
            messageSent = true;
    }

    if (!done && messageSent)
    {
        static std::string receivedData;
        char data[256];
        std::size_t size = 0;

        if ((socket.receive(data, 256, size) == sf::Socket::Done) && (size > 0))
            receivedData += std::string(data, size);

        if (receivedData.size() >= testString.size())
        {
            sf::err() << "Success: " << receivedData << std::endl;
            socket.disconnect();
        }

        if (socket.getRemoteAddress() == sf::IpAddress::None)
            done = true;
    }
}


////////////////////////////////////////////////////////////
/// \brief Game class
///
////////////////////////////////////////////////////////////
class Game
{
public:
    Game() :
    m_window(sf::VideoMode(800, 600, 32), ""),
    m_rightPaddleSpeed(0.f),
    m_ballAngle(0.f), // to be changed later
    m_isPlaying(false)
    {
        // Create the window of the application
        m_window.setVerticalSyncEnabled(true);

        std::srand(static_cast<unsigned int>(std::time(NULL)));

        // Define some constants
        const float pi = 3.14159f;
        const sf::Vector2f paddleSize(25, 100);
        const float ballRadius = 10.f;

        // Load the sounds used in the game
        if (!m_ballSoundBuffer.loadFromFile("resources/ball.wav"))
            throw std::runtime_error("Could not load resources/ball.wav");
        m_ballSound.setBuffer(m_ballSoundBuffer);

        // Create the left paddle
        m_leftPaddle.setSize(paddleSize - sf::Vector2f(3, 3));
        m_leftPaddle.setOutlineThickness(3);
        m_leftPaddle.setOutlineColor(sf::Color::Black);
        m_leftPaddle.setFillColor(sf::Color(100, 100, 200));
        m_leftPaddle.setOrigin(paddleSize / 2.f);

        // Create the right paddle
        m_rightPaddle.setSize(paddleSize - sf::Vector2f(3, 3));
        m_rightPaddle.setOutlineThickness(3);
        m_rightPaddle.setOutlineColor(sf::Color::Black);
        m_rightPaddle.setFillColor(sf::Color(200, 100, 100));
        m_rightPaddle.setOrigin(paddleSize / 2.f);

        // Create the ball
        m_ball.setRadius(ballRadius - 3);
        m_ball.setOutlineThickness(3);
        m_ball.setOutlineColor(sf::Color::Black);
        m_ball.setFillColor(sf::Color::White);
        m_ball.setOrigin(ballRadius / 2, ballRadius / 2);

        // Load the text font
        if (!m_font.loadFromFile("resources/sansation.ttf"))
            throw std::runtime_error("Could not load resources/sansation.ttf");

        // Initialize the pause message
        m_pauseMessage.setFont(m_font);
        m_pauseMessage.setCharacterSize(40);
        m_pauseMessage.setPosition(170.f, 150.f);
        m_pauseMessage.setColor(sf::Color::White);
        m_pauseMessage.setString("Welcome to SFML pong!\nPress space to start the game");
    }

    void loop()
    {
        const float pi = 3.14159f;
        const int gameWidth = 800;
        const int gameHeight = 600;
        const sf::Vector2f paddleSize(25, 100);
        const float ballRadius = 10.f;

        // Define the paddles properties
        const sf::Time AITime   = sf::seconds(0.1f);
        const float paddleSpeed = 400.f;
        const float ballSpeed   = 400.f;

        // Handle events
        sf::Event event;
        while (m_window.pollEvent(event))
        {
            // Window closed or escape key pressed: exit
            if ((event.type == sf::Event::Closed) ||
               ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape)))
            {
                emscripten_cancel_main_loop();
                break;
            }

            // Space key pressed: play
            if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Space))
            {
                if (!m_isPlaying)
                {
                    // (re)start the game
                    m_isPlaying = true;
                    m_clock.restart();

                    // Reset the position of the paddles and ball
                    m_leftPaddle.setPosition(10 + paddleSize.x / 2, gameHeight / 2);
                    m_rightPaddle.setPosition(gameWidth - 10 - paddleSize.x / 2, gameHeight / 2);
                    m_ball.setPosition(gameWidth / 2, gameHeight / 2);

                    // Reset the ball angle
                    do
                    {
                        // Make sure the ball initial angle is not too much vertical
                        m_ballAngle = (std::rand() % 360) * 2 * pi / 360;
                    }
                    while (std::abs(std::cos(m_ballAngle)) < 0.7f);
                }
            }
        }

        if (m_isPlaying)
        {
            float deltaTime = m_clock.restart().asSeconds();

            // Move the player's paddle
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) &&
               (m_leftPaddle.getPosition().y - paddleSize.y / 2 > 5.f))
            {
                m_leftPaddle.move(0.f, -paddleSpeed * deltaTime);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) &&
               (m_leftPaddle.getPosition().y + paddleSize.y / 2 < gameHeight - 5.f))
            {
                m_leftPaddle.move(0.f, paddleSpeed * deltaTime);
            }

            // Move the computer's paddle
            if (((m_rightPaddleSpeed < 0.f) && (m_rightPaddle.getPosition().y - paddleSize.y / 2 > 5.f)) ||
                ((m_rightPaddleSpeed > 0.f) && (m_rightPaddle.getPosition().y + paddleSize.y / 2 < gameHeight - 5.f)))
            {
                m_rightPaddle.move(0.f, m_rightPaddleSpeed * deltaTime);
            }

            // Update the computer's paddle direction according to the ball position
            if (m_AITimer.getElapsedTime() > AITime)
            {
                m_AITimer.restart();
                if (m_ball.getPosition().y + ballRadius > m_rightPaddle.getPosition().y + paddleSize.y / 2)
                    m_rightPaddleSpeed = paddleSpeed;
                else if (m_ball.getPosition().y - ballRadius < m_rightPaddle.getPosition().y - paddleSize.y / 2)
                    m_rightPaddleSpeed = -paddleSpeed;
                else
                    m_rightPaddleSpeed = 0.f;
            }

            // Move the ball
            float factor = ballSpeed * deltaTime;
            m_ball.move(std::cos(m_ballAngle) * factor, std::sin(m_ballAngle) * factor);

            // Check collisions between the ball and the screen
            if (m_ball.getPosition().x - ballRadius < 0.f)
            {
                m_isPlaying = false;
                m_pauseMessage.setString("You lost!\nPress space to restart or\nescape to exit");
            }
            if (m_ball.getPosition().x + ballRadius > gameWidth)
            {
                m_isPlaying = false;
                m_pauseMessage.setString("You won!\nPress space to restart or\nescape to exit");
            }
            if (m_ball.getPosition().y - ballRadius < 0.f)
            {
                m_ballSound.play();
                m_ballAngle = -m_ballAngle;
                m_ball.setPosition(m_ball.getPosition().x, ballRadius + 0.1f);
            }
            if (m_ball.getPosition().y + ballRadius > gameHeight)
            {
                m_ballSound.play();
                m_ballAngle = -m_ballAngle;
                m_ball.setPosition(m_ball.getPosition().x, gameHeight - ballRadius - 0.1f);
            }

            // Check the collisions between the ball and the paddles
            // Left Paddle
            if (m_ball.getPosition().x - ballRadius < m_leftPaddle.getPosition().x + paddleSize.x / 2 &&
                m_ball.getPosition().x - ballRadius > m_leftPaddle.getPosition().x &&
                m_ball.getPosition().y + ballRadius >= m_leftPaddle.getPosition().y - paddleSize.y / 2 &&
                m_ball.getPosition().y - ballRadius <= m_leftPaddle.getPosition().y + paddleSize.y / 2)
            {
                if (m_ball.getPosition().y > m_leftPaddle.getPosition().y)
                    m_ballAngle = pi - m_ballAngle + (std::rand() % 20) * pi / 180;
                else
                    m_ballAngle = pi - m_ballAngle - (std::rand() % 20) * pi / 180;

                m_ballSound.play();
                m_ball.setPosition(m_leftPaddle.getPosition().x + ballRadius + paddleSize.x / 2 + 0.1f, m_ball.getPosition().y);
            }

            // Right Paddle
            if (m_ball.getPosition().x + ballRadius > m_rightPaddle.getPosition().x - paddleSize.x / 2 &&
                m_ball.getPosition().x + ballRadius < m_rightPaddle.getPosition().x &&
                m_ball.getPosition().y + ballRadius >= m_rightPaddle.getPosition().y - paddleSize.y / 2 &&
                m_ball.getPosition().y - ballRadius <= m_rightPaddle.getPosition().y + paddleSize.y / 2)
            {
                if (m_ball.getPosition().y > m_rightPaddle.getPosition().y)
                    m_ballAngle = pi - m_ballAngle + (std::rand() % 20) * pi / 180;
                else
                    m_ballAngle = pi - m_ballAngle - (std::rand() % 20) * pi / 180;

                m_ballSound.play();
                m_ball.setPosition(m_rightPaddle.getPosition().x - ballRadius - paddleSize.x / 2 - 0.1f, m_ball.getPosition().y);
            }
        }

        // Clear the window
        m_window.clear(sf::Color(50, 200, 50));

        if (m_isPlaying)
        {
            // Draw the paddles and the ball
            m_window.draw(m_leftPaddle);
            m_window.draw(m_rightPaddle);
            m_window.draw(m_ball);
        }
        else
        {
            // Draw the pause message
            m_window.draw(m_pauseMessage);
        }

        // Display things on screen
        m_window.display();
    }

private:
    sf::RenderWindow m_window;

    sf::SoundBuffer m_ballSoundBuffer;
    sf::Sound m_ballSound;

    sf::RectangleShape m_leftPaddle;
    sf::RectangleShape m_rightPaddle;

    sf::CircleShape m_ball;

    sf::Font m_font;
    sf::Text m_pauseMessage;

    sf::Clock m_AITimer;
    float m_rightPaddleSpeed;
    float m_ballAngle;

    sf::Clock m_clock;
    bool m_isPlaying;
};

////////////////////////////////////////////////////////////
/// Main loop of application
///
/// \param param User data
///
////////////////////////////////////////////////////////////
void mainLoop(void* param)
{
    doSocket();

    Game& game = *reinterpret_cast<Game*>(param);

    game.loop();
}


////////////////////////////////////////////////////////////
/// Entry point of application
///
/// \return Application exit code
///
////////////////////////////////////////////////////////////
int main()
{
    Game game;

    // Main loop, this will never return
    emscripten_set_main_loop_arg(mainLoop, &game, 0, 1);

    return EXIT_SUCCESS;
}

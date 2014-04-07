
////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Graphics.hpp>
#include <cmath>
#include <fstream>
#include <sstream>
#include <vector>


////////////////////////////////////////////////////////////
// Obj model
////////////////////////////////////////////////////////////
class ObjModel : public sf::Model
{
private :

    struct FaceData
    {
        unsigned int position0, position1, position2;
        unsigned int textureCoordinate0, textureCoordinate1, textureCoordinate2;
        unsigned int normal0, normal1, normal2;
    };

    sf::Vertex makeVertex(const std::string& indices)
    {
        sf::Vertex vertex;
        std::istringstream indiceStream(indices);

        unsigned int positionIndex = 0;
        unsigned int textureCoordinateIndex = 0;
        unsigned int normalIndex = 0;
        char separator = 0;

        if (!(indiceStream >> positionIndex >> separator).good())
            return vertex;

        if (!(indiceStream >> textureCoordinateIndex >> separator).good())
            return vertex;

        if (!(indiceStream >> normalIndex).eof())
            return vertex;

        // .obj indices start at 1
        positionIndex -= 1;
        textureCoordinateIndex -= 1;
        normalIndex -= 1;

        if (positionIndex >= m_vertexPositions.size() ||
            textureCoordinateIndex >= m_vertexTextureCoordinates.size() ||
            normalIndex >= m_vertexNormals.size())
            return vertex;

        vertex.position = m_vertexPositions[positionIndex];
        vertex.texCoords = m_vertexTextureCoordinates[textureCoordinateIndex];
        vertex.normal = m_vertexNormals[normalIndex];

        // Not needed, but just for demonstration
        vertex.color = getColor();

        return vertex;
    }

public :

    bool loadFromFile(const std::string& filename)
    {
        // Open our model file
        std::ifstream teapotModelFile(filename.c_str());
        if (!teapotModelFile.is_open())
            return false;

        // Parse the model file line by line
        std::string line;
        std::istringstream lineStream;
        std::string token;
        while (true)
        {
            std::getline(teapotModelFile, line);

            // Break on error or failure to read (end of file)
            if (teapotModelFile.bad() || teapotModelFile.fail())
                break;

            lineStream.clear();
            lineStream.str(line);
            lineStream >> token;

            if (token == "v")
            {
                // Handle vertex positions
                sf::Vector3f position;
                lineStream >> position.x >> position.y >> position.z;
                m_vertexPositions.push_back(position);
            }
            else if (token == "vt")
            {
                // Handle vertex texture coordinates
                sf::Vector2f coordinate;
                lineStream >> coordinate.x >> coordinate.y;
                m_vertexTextureCoordinates.push_back(coordinate);
            }
            else if (token == "vn")
            {
                // Handle vertex normals
                sf::Vector3f normal;
                lineStream >> normal.x >> normal.y >> normal.z;
                m_vertexNormals.push_back(normal);
            }
            else if (token == "f")
            {
                // Handle faces
                std::string vertexString0, vertexString1, vertexString2;

                lineStream >> vertexString0 >> vertexString1 >> vertexString2;

                sf::Vertex vertex0(makeVertex(vertexString0));
                sf::Vertex vertex1(makeVertex(vertexString1));
                sf::Vertex vertex2(makeVertex(vertexString2));

                addVertex(vertex0);
                addVertex(vertex1);
                addVertex(vertex2);

                unsigned int index = getVertexCount();

                addFace(index - 3, index - 2, index - 1);
            }
        }

        // Update the underlying polyhedron geometry
        update();

        return true;
    }

private :

    std::vector<sf::Vector3f> m_vertexPositions;
    std::vector<sf::Vector2f> m_vertexTextureCoordinates;
    std::vector<sf::Vector3f> m_vertexNormals;
    std::vector<FaceData>     m_faceData;
};


////////////////////////////////////////////////////////////
/// Entry point of application
///
/// \return Application exit code
///
////////////////////////////////////////////////////////////
int main()
{
    // Request a 32-bits depth buffer when creating the window
    sf::ContextSettings contextSettings;
    contextSettings.depthBits = 32;

    // Create the main window
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML 3D graphics", sf::Style::Default, contextSettings);
    window.setVerticalSyncEnabled(true);

    float aspectRatio = static_cast<float>(window.getSize().x) / static_cast<float>(window.getSize().y);
    sf::Vector3f downscaleFactor(1 / static_cast<float>(window.getSize().x) * aspectRatio, -1 / static_cast<float>(window.getSize().y), 1);

    // Set up our 3D camera with a field of view of 90 degrees
    // 1000 units space between the clipping planes
    // and scale it according to the screen aspect ratio
    sf::Camera camera(90.f, 0.001f, 1000.f);
    camera.scale(1 / aspectRatio, 1, 1);
    camera.setPosition(0, 0, 10);

    // Set the camera as the window's active view
    window.setView(camera);

    // Create a sprite for the background
    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("resources/background.jpg"))
        return EXIT_FAILURE;
    sf::Sprite background(backgroundTexture);
    background.setOrigin(backgroundTexture.getSize().x / 2, backgroundTexture.getSize().y / 2);
    background.setPosition(0, 0, -100);
    background.setScale(downscaleFactor * 100.f);

    // Create some text to draw on top of our OpenGL object
    sf::Font font;
    if (!font.loadFromFile("resources/sansation.ttf"))
        return EXIT_FAILURE;
    sf::Text text("SFML / 3D demo", font);
    text.setColor(sf::Color(255, 255, 255, 170));
    text.setOrigin(text.getGlobalBounds().width / 2, text.getGlobalBounds().height / 2);
    text.setPosition(0, -30, -100);
    text.setScale(downscaleFactor * 100.f);

    sf::Text info("W, A, S, D, Space, Shift, Mouse to move\nEsc to exit", font);
    info.setColor(sf::Color(255, 255, 255, 170));
    info.setPosition(10, 0, 0);

    // Create a cube to demonstrate transform and lighting effects
    sf::Cuboid cube(sf::Vector3f(5, 5, 5));
    cube.setColor(sf::Color::Red);
    cube.setPosition(20, 0, -50);

    // Create a sphere to demonstrate transform and lighting effects
    sf::SphericalPolyhedron sphere(5);
    sphere.setColor(sf::Color::Cyan);
    sphere.setPosition(-20, 0, -50);

    // Create a sphere to mark our light position
    sf::SphericalPolyhedron lightSphere(2, 1);
    lightSphere.setColor(sf::Color::Yellow);

    // Create a billboard
    sf::Texture billboardTexture;
    if (!billboardTexture.loadFromFile("resources/texture.jpg"))
        return EXIT_FAILURE;
    sf::Billboard billboard(billboardTexture);
    billboard.setOrigin(billboardTexture.getSize().x / 2, billboardTexture.getSize().y / 2);
    billboard.setPosition(0, -10, -50);
    billboard.setScale(downscaleFactor * 20.f);
    billboard.setCamera(camera);

    // Create a teapot
    ObjModel teapot;
    if (!teapot.loadFromFile("resources/teapot.obj"))
        return EXIT_FAILURE;
    teapot.setColor(sf::Color::Green);
    teapot.setPosition(0, 10, -50);
    teapot.setScale(0.5f, 0.5f, 0.5f);

    // Create a clock for measuring the time elapsed
    sf::Clock clock;
    float elapsedSeconds = 0;

    // Create a light to illuminate our scene
    sf::Light light;
    light.setColor(sf::Color::White);
    light.setAmbientIntensity(0.1f);
    light.setDiffuseIntensity(1.0f);
    light.setLinearAttenuation(0.002f);
    light.setQuadraticAttenuation(0.0005f);
    light.enable();
    sf::Light::enableLighting();

    // Enable depth testing so we can draw 3D objects in any order
    window.enableDepthTest(true);

    // Keep the mouse cursor hidden at the center of the window
    sf::Mouse::setPosition(sf::Vector2i(window.getSize()) / 2, window);
    window.setMouseCursorVisible(false);

    // Variables that keep track of our virtual camera orientation
    float yaw = 3.141592654f / 2.f;
    float pitch = 0.f;
    sf::Vector3f direction;
    sf::Vector3f rightVector;

    // Start game loop
    while (window.isOpen())
    {
        float delta = clock.restart().asSeconds();
        elapsedSeconds += delta;

        int deltaX = 0;
        int deltaY = 0;

        // Process events
        sf::Event event;
        while (window.pollEvent(event))
        {
            // Close window : exit
            if (event.type == sf::Event::Closed)
                window.close();

            // Escape key : exit
            if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape))
                window.close();

            // Mouse move : rotate camera
            if (event.type == sf::Event::MouseMoved)
            {
                deltaX = event.mouseMove.x - window.getSize().x / 2;
                deltaY = event.mouseMove.y - window.getSize().y / 2;
            }
        }

        // Keep the mouse cursor within the window
        sf::Mouse::setPosition(sf::Vector2i(window.getSize()) / 2, window);

        // Update our virtual camera orientation/position based on user input
        yaw   -= deltaX / 5.f * delta;
        pitch -= deltaY / 5.f * delta;

        direction.x = std::cos(yaw) * std::cos(pitch);
        direction.y = std::sin(pitch);
        direction.z = -std::sin(yaw) * std::cos(pitch);

        rightVector.x = -direction.z;
        rightVector.y = 0.f;
        rightVector.z = direction.x;

        float rightVectorNorm = std::sqrt(rightVector.x * rightVector.x +
                                          rightVector.y * rightVector.y +
                                          rightVector.z * rightVector.z);
        rightVector /= rightVectorNorm;

        camera.setDirection(direction);

        // W key pressed : move forward
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
            camera.move(direction * 50.f * delta);

        // A key pressed : strafe left
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
            camera.move(rightVector * -50.f * delta);

        // S key pressed : move backward
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            camera.move(direction * -50.f * delta);

        // D key pressed : strafe right
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
            camera.move(rightVector * 50.f * delta);

        // Space bar pressed : move upwards
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
            camera.move(0, 50 * delta, 0);

        // Shift key pressed : move downwards
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
            camera.move(0, -50 * delta, 0);

        // Inform the window to update its view with the new camera data
        window.setView(camera);

        // Clear the window
        window.clear();

        cube.rotate(50 * delta, sf::Vector3f(0.5f, 0.9f, 0.2f));
        lightSphere.rotate(180 * delta, sf::Vector3f(0.7f, 0.2f, 0.4f));
        teapot.rotate(40 * delta, sf::Vector3f(0.0f, 1.0f, 0.0f));

        // Make the light source orbit around the scene
        sf::Vector3f newOrbitPosition(50 * std::cos(elapsedSeconds / 6),
                                      30 * std::cos(elapsedSeconds / 6),
                                      20 * std::sin(elapsedSeconds / 6));
        light.setPosition(sf::Vector3f(0, 0, -50) + newOrbitPosition);

        // Set the sphere to the same position as the light source
        lightSphere.setPosition(light.getPosition());

        // Draw the background
        window.draw(background);

        // Disable lighting for the text and the light sphere
        sf::Light::disableLighting();

        // Disable depth testing for sf::Text because it requires blending
        window.enableDepthTest(false);
        window.draw(text);
        window.enableDepthTest(true);

        // Draw the sphere representing the light position
        window.draw(lightSphere);

        // Enable lighting again
        sf::Light::enableLighting();

        // Draw the cube, sphere and billboard
        window.draw(cube);
        window.draw(sphere);
        window.draw(billboard);
        window.draw(teapot);

        // Disable lighting and reset to 2D view to draw information
        sf::Light::disableLighting();
        window.setView(window.getDefaultView());

        // Draw informational text
        window.draw(info);

        // Reset view to our camera and enable lighting again
        window.setView(camera);
        sf::Light::enableLighting();

        // Finally, display the rendered frame on screen
        window.display();
    }

    return EXIT_SUCCESS;
}


////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Graphics/Light.hpp>
#include <SFML/Graphics/GLCheck.hpp>
#include <SFML/System/Mutex.hpp>
#include <SFML/System/Lock.hpp>
#include <SFML/System/Err.hpp>
#include <cmath>


namespace sf
{

std::vector<bool> Light::m_usedIds;
Mutex Light::m_usedIdsMutex;

////////////////////////////////////////////////////////////
Light::Light() :
m_light               (-1),
m_position            (0, 0, 0),
m_directional         (false),
m_ambientColor        (Color::Black),
m_diffuseColor        (Color::White),
m_specularColor       (Color::White),
m_constantAttenuation (1.0f),
m_linearAttenuation   (0.0f),
m_quadraticAttenuation(0.0f),
m_enabled             (false)
{
    getId();

    if (m_light < 0)
        return;

    GLfloat position[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glCheck(glLightfv(GL_LIGHT0 + m_light, GL_POSITION, position));

    GLfloat ambientColor[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glCheck(glLightfv(GL_LIGHT0 + m_light, GL_AMBIENT, ambientColor));

    GLfloat diffuseColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glCheck(glLightfv(GL_LIGHT0 + m_light, GL_DIFFUSE, diffuseColor));

    GLfloat specularColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glCheck(glLightfv(GL_LIGHT0 + m_light, GL_SPECULAR, specularColor));

    glCheck(glLightf(GL_LIGHT0 + m_light, GL_CONSTANT_ATTENUATION, 1.0f));
    glCheck(glLightf(GL_LIGHT0 + m_light, GL_LINEAR_ATTENUATION, 0.0f));
    glCheck(glLightf(GL_LIGHT0 + m_light, GL_QUADRATIC_ATTENUATION, 0.0f));

    glCheck(glDisable(GL_LIGHT0 + m_light));
}


////////////////////////////////////////////////////////////
Light::Light(const Light& copy) :
m_light               (-1),
m_position            (copy.m_position),
m_directional         (copy.m_directional),
m_ambientColor        (copy.m_ambientColor),
m_diffuseColor        (copy.m_diffuseColor),
m_specularColor       (copy.m_specularColor),
m_constantAttenuation (copy.m_constantAttenuation),
m_linearAttenuation   (copy.m_linearAttenuation),
m_quadraticAttenuation(copy.m_quadraticAttenuation),
m_enabled             (false)
{
    getId();

    if (m_light < 0)
        return;

    // If this is a directional light source, normalize the direction vector
    if (m_directional)
    {
        float norm = std::sqrt(m_position.x * m_position.x + m_position.y * m_position.y + m_position.z * m_position.z);

        m_position /= norm;
    }

    GLfloat position[] = {m_position.x, m_position.y, m_position.z, m_directional ? 0.0f : 1.0f};
    glCheck(glLightfv(GL_LIGHT0 + m_light, GL_POSITION, position));

    GLfloat ambientColor[] = {static_cast<float>(m_ambientColor.r) / 255.f,
                              static_cast<float>(m_ambientColor.g) / 255.f,
                              static_cast<float>(m_ambientColor.b) / 255.f,
                              static_cast<float>(m_ambientColor.a) / 255.f};
    glCheck(glLightfv(GL_LIGHT0 + m_light, GL_AMBIENT, ambientColor));

    GLfloat diffuseColor[] = {static_cast<float>(m_diffuseColor.r) / 255.f,
                              static_cast<float>(m_diffuseColor.g) / 255.f,
                              static_cast<float>(m_diffuseColor.b) / 255.f,
                              static_cast<float>(m_diffuseColor.a) / 255.f};
    glCheck(glLightfv(GL_LIGHT0 + m_light, GL_DIFFUSE, diffuseColor));

    GLfloat specularColor[] = {static_cast<float>(m_specularColor.r) / 255.f,
                               static_cast<float>(m_specularColor.g) / 255.f,
                               static_cast<float>(m_specularColor.b) / 255.f,
                               static_cast<float>(m_specularColor.a) / 255.f};
    glCheck(glLightfv(GL_LIGHT0 + m_light, GL_SPECULAR, specularColor));

    glCheck(glLightf(GL_LIGHT0 + m_light, GL_CONSTANT_ATTENUATION, m_constantAttenuation));
    glCheck(glLightf(GL_LIGHT0 + m_light, GL_LINEAR_ATTENUATION, m_linearAttenuation));
    glCheck(glLightf(GL_LIGHT0 + m_light, GL_QUADRATIC_ATTENUATION, m_quadraticAttenuation));

    glCheck(glDisable(GL_LIGHT0 + m_light));
}


////////////////////////////////////////////////////////////
Light::~Light()
{
    if (m_light >= 0)
    {
        Lock lock(m_usedIdsMutex);
        m_usedIds[m_light] = false;
    }
}


////////////////////////////////////////////////////////////
void Light::setDirectional(bool directional)
{
    m_directional = directional;

    if (m_light < 0)
        return;

    // If this becomes a directional light source, normalize the direction vector
    if (m_directional)
    {
        float norm = std::sqrt(m_position.x * m_position.x + m_position.y * m_position.y + m_position.z * m_position.z);

        m_position /= norm;
    }

    GLfloat position[] = {m_position.x, m_position.y, m_position.z, m_directional ? 0.0f : 1.0f};
    glCheck(glLightfv(GL_LIGHT0 + m_light, GL_POSITION, position));
}


////////////////////////////////////////////////////////////
void Light::setPosition(float x, float y, float z)
{
    setPosition(Vector3f(x, y, z));
}


////////////////////////////////////////////////////////////
void Light::setPosition(const Vector3f& position)
{
    m_position = position;

    if (m_light < 0)
        return;

    // If this is a directional light source, normalize the direction vector
    if (m_directional)
    {
        float norm = std::sqrt(m_position.x * m_position.x + m_position.y * m_position.y + m_position.z * m_position.z);

        m_position /= norm;
    }

    GLfloat glPosition[] = {m_position.x, m_position.y, m_position.z, m_directional ? 0.0f : 1.0f};
    glCheck(glLightfv(GL_LIGHT0 + m_light, GL_POSITION, glPosition));
}


////////////////////////////////////////////////////////////
const Vector3f& Light::getPosition() const
{
    return m_position;
}


////////////////////////////////////////////////////////////
void Light::setDirection(float x, float y, float z)
{
    setPosition(Vector3f(x, y, z));
}


////////////////////////////////////////////////////////////
void Light::setDirection(const Vector3f& position)
{
    setPosition(position);
}


////////////////////////////////////////////////////////////
const Vector3f& Light::getDirection() const
{
    return m_position;
}


////////////////////////////////////////////////////////////
void Light::setAmbientColor(const Color& color)
{
    m_ambientColor = color;

    if (m_light < 0)
        return;

    GLfloat ambientColor[] = {static_cast<float>(m_ambientColor.r) / 255.f,
                              static_cast<float>(m_ambientColor.g) / 255.f,
                              static_cast<float>(m_ambientColor.b) / 255.f,
                              static_cast<float>(m_ambientColor.a) / 255.f};
    glCheck(glLightfv(GL_LIGHT0 + m_light, GL_AMBIENT, ambientColor));
}


////////////////////////////////////////////////////////////
const Color& Light::getAmbientColor() const
{
    return m_ambientColor;
}


////////////////////////////////////////////////////////////
void Light::setDiffuseColor(const Color& color)
{
    m_diffuseColor = color;

    if (m_light < 0)
        return;

    GLfloat diffuseColor[] = {static_cast<float>(m_diffuseColor.r) / 255.f,
                              static_cast<float>(m_diffuseColor.g) / 255.f,
                              static_cast<float>(m_diffuseColor.b) / 255.f,
                              static_cast<float>(m_diffuseColor.a) / 255.f};
    glCheck(glLightfv(GL_LIGHT0 + m_light, GL_DIFFUSE, diffuseColor));
}


////////////////////////////////////////////////////////////
const Color& Light::getDiffuseColor() const
{
    return m_diffuseColor;
}


////////////////////////////////////////////////////////////
void Light::setSpecularColor(const Color& color)
{
    m_specularColor = color;

    if (m_light < 0)
        return;

    GLfloat specularColor[] = {static_cast<float>(m_specularColor.r) / 255.f,
                               static_cast<float>(m_specularColor.g) / 255.f,
                               static_cast<float>(m_specularColor.b) / 255.f,
                               static_cast<float>(m_specularColor.a) / 255.f};
    glCheck(glLightfv(GL_LIGHT0 + m_light, GL_SPECULAR, specularColor));
}


////////////////////////////////////////////////////////////
const Color& Light::getSpecularColor() const
{
    return m_specularColor;
}


////////////////////////////////////////////////////////////
void Light::setConstantAttenuation(float attenuation)
{
    m_constantAttenuation = attenuation;

    if (m_light < 0)
        return;

    glCheck(glLightf(GL_LIGHT0 + m_light, GL_CONSTANT_ATTENUATION, m_constantAttenuation));
}


////////////////////////////////////////////////////////////
float Light::getConstantAttenuation() const
{
    return m_constantAttenuation;
}


////////////////////////////////////////////////////////////
void Light::setLinearAttenuation(float attenuation)
{
    m_linearAttenuation = attenuation;

    if (m_light < 0)
        return;

    glCheck(glLightf(GL_LIGHT0 + m_light, GL_LINEAR_ATTENUATION, m_linearAttenuation));
}


////////////////////////////////////////////////////////////
float Light::getLinearAttenuation() const
{
    return m_linearAttenuation;
}


////////////////////////////////////////////////////////////
void Light::setQuadraticAttenuation(float attenuation)
{
    m_quadraticAttenuation = attenuation;

    if (m_light < 0)
        return;

    glCheck(glLightf(GL_LIGHT0 + m_light, GL_QUADRATIC_ATTENUATION, m_quadraticAttenuation));
}


////////////////////////////////////////////////////////////
float Light::getQuadraticAttenuation() const
{
    return m_quadraticAttenuation;
}


////////////////////////////////////////////////////////////
void Light::move(float offsetX, float offsetY, float offsetZ)
{
    move(Vector3f(offsetX, offsetY, offsetZ));
}


////////////////////////////////////////////////////////////
void Light::move(const Vector3f& offset)
{
    setPosition(m_position + offset);
}


////////////////////////////////////////////////////////////
bool Light::isDirectional() const
{
    return m_directional;
}


////////////////////////////////////////////////////////////
void Light::enable()
{
    if (m_light < 0)
        return;

    glCheck(glEnable(GL_LIGHT0 + m_light));
}


////////////////////////////////////////////////////////////
void Light::disable()
{
    if (m_light < 0)
        return;

    glCheck(glDisable(GL_LIGHT0 + m_light));
}


////////////////////////////////////////////////////////////
unsigned int Light::getMaximumLights()
{
    ensureGlContext();

    GLint max_lights = 0;
    glCheck(glGetIntegerv(GL_MAX_LIGHTS, &max_lights));

    return static_cast<unsigned int>(max_lights);
}


////////////////////////////////////////////////////////////
void Light::enableLighting()
{
    ensureGlContext();

    glCheck(glEnable(GL_LIGHTING));
}


////////////////////////////////////////////////////////////
void Light::disableLighting()
{
    ensureGlContext();

    glCheck(glDisable(GL_LIGHTING));
}


////////////////////////////////////////////////////////////
Light& Light::operator =(const Light& right)
{
    Light temp(right);

    std::swap(m_light,                temp.m_light);
    std::swap(m_position,             temp.m_position);
    std::swap(m_directional,          temp.m_directional);
    std::swap(m_ambientColor,         temp.m_ambientColor);
    std::swap(m_diffuseColor,         temp.m_diffuseColor);
    std::swap(m_specularColor,        temp.m_specularColor);
    std::swap(m_constantAttenuation,  temp.m_constantAttenuation);
    std::swap(m_linearAttenuation,    temp.m_linearAttenuation);
    std::swap(m_quadraticAttenuation, temp.m_quadraticAttenuation);
    std::swap(m_enabled,              temp.m_enabled);

    return *this;
}


////////////////////////////////////////////////////////////
void Light::getId()
{
    Lock lock(m_usedIdsMutex);

    if (m_usedIds.empty())
        m_usedIds.resize(getMaximumLights(), false);

    for (int i = 0; i < m_usedIds.size(); ++i)
    {
        if (!m_usedIds[i])
        {
            m_light = i;
            m_usedIds[i] = true;
            return;
        }
    }

#ifdef SFML_DEBUG
    // Inform the user that they created too many lights
    // for the fixed function pipeline to handle
    err() << "Not enough OpenGL lights to support creating "
          << "more sf::Light objects."
          << std::endl;
#endif
}

} // namespace sf

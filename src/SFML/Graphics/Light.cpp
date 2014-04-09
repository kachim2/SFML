
////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Graphics/Light.hpp>
#include <SFML/Graphics/GLCheck.hpp>
#include <SFML/System/Mutex.hpp>
#include <SFML/System/Lock.hpp>
#include <SFML/System/Err.hpp>
#include <cmath>
#include <sstream>


namespace sf
{

Mutex Light::m_lightMutex;
std::vector<bool> Light::m_usedIds;
std::set<const Light*> Light::m_enabledLights;
bool Light::m_lightingEnabled = false;

////////////////////////////////////////////////////////////
Light::Light() :
m_light               (-1),
m_position            (0, 0, 0),
m_directional         (false),
m_color               (Color::White),
m_ambientIntensity    (0.f),
m_diffuseIntensity    (1.f),
m_specularIntensity   (1.f),
m_constantAttenuation (1.f),
m_linearAttenuation   (0.f),
m_quadraticAttenuation(0.f),
m_enabled             (false)
{
    getId();

    if (m_light < 0)
        return;

    if (hasShaderLighting())
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
m_color               (copy.m_color),
m_ambientIntensity    (copy.m_ambientIntensity),
m_diffuseIntensity    (copy.m_diffuseIntensity),
m_specularIntensity   (copy.m_specularIntensity),
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

    if (hasShaderLighting())
        return;

    GLfloat position[] = {m_position.x, m_position.y, m_position.z, m_directional ? 0.0f : 1.0f};
    glCheck(glLightfv(GL_LIGHT0 + m_light, GL_POSITION, position));

    GLfloat ambientColor[] = {static_cast<float>(m_color.r) / 255.f * m_ambientIntensity,
                              static_cast<float>(m_color.g) / 255.f * m_ambientIntensity,
                              static_cast<float>(m_color.b) / 255.f * m_ambientIntensity,
                              static_cast<float>(m_color.a) / 255.f * m_ambientIntensity};
    glCheck(glLightfv(GL_LIGHT0 + m_light, GL_AMBIENT, ambientColor));

    GLfloat diffuseColor[] = {static_cast<float>(m_color.r) / 255.f * m_diffuseIntensity,
                              static_cast<float>(m_color.g) / 255.f * m_diffuseIntensity,
                              static_cast<float>(m_color.b) / 255.f * m_diffuseIntensity,
                              static_cast<float>(m_color.a) / 255.f * m_diffuseIntensity};
    glCheck(glLightfv(GL_LIGHT0 + m_light, GL_DIFFUSE, diffuseColor));

    GLfloat specularColor[] = {static_cast<float>(m_color.r) / 255.f * m_specularIntensity,
                               static_cast<float>(m_color.g) / 255.f * m_specularIntensity,
                               static_cast<float>(m_color.b) / 255.f * m_specularIntensity,
                               static_cast<float>(m_color.a) / 255.f * m_specularIntensity};
    glCheck(glLightfv(GL_LIGHT0 + m_light, GL_SPECULAR, specularColor));

    glCheck(glLightf(GL_LIGHT0 + m_light, GL_CONSTANT_ATTENUATION, m_constantAttenuation));
    glCheck(glLightf(GL_LIGHT0 + m_light, GL_LINEAR_ATTENUATION, m_linearAttenuation));
    glCheck(glLightf(GL_LIGHT0 + m_light, GL_QUADRATIC_ATTENUATION, m_quadraticAttenuation));

    glCheck(glDisable(GL_LIGHT0 + m_light));
}


////////////////////////////////////////////////////////////
Light::~Light()
{
    disable();

    if (m_light >= 0)
    {
        Lock lock(m_lightMutex);
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

    if (hasShaderLighting())
        return;

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

    if (hasShaderLighting())
        return;

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
void Light::setColor(const Color& color)
{
    m_color = color;

    if (m_light < 0)
        return;

    if (hasShaderLighting())
        return;

    GLfloat ambientColor[] = {static_cast<float>(m_color.r) / 255.f * m_ambientIntensity,
                              static_cast<float>(m_color.g) / 255.f * m_ambientIntensity,
                              static_cast<float>(m_color.b) / 255.f * m_ambientIntensity,
                              static_cast<float>(m_color.a) / 255.f * m_ambientIntensity};
    glCheck(glLightfv(GL_LIGHT0 + m_light, GL_AMBIENT, ambientColor));

    GLfloat diffuseColor[] = {static_cast<float>(m_color.r) / 255.f * m_diffuseIntensity,
                              static_cast<float>(m_color.g) / 255.f * m_diffuseIntensity,
                              static_cast<float>(m_color.b) / 255.f * m_diffuseIntensity,
                              static_cast<float>(m_color.a) / 255.f * m_diffuseIntensity};
    glCheck(glLightfv(GL_LIGHT0 + m_light, GL_DIFFUSE, diffuseColor));

    GLfloat specularColor[] = {static_cast<float>(m_color.r) / 255.f * m_specularIntensity,
                               static_cast<float>(m_color.g) / 255.f * m_specularIntensity,
                               static_cast<float>(m_color.b) / 255.f * m_specularIntensity,
                               static_cast<float>(m_color.a) / 255.f * m_specularIntensity};
    glCheck(glLightfv(GL_LIGHT0 + m_light, GL_SPECULAR, specularColor));
}


////////////////////////////////////////////////////////////
const Color& Light::getColor() const
{
    return m_color;
}


////////////////////////////////////////////////////////////
void Light::setAmbientIntensity(float intensity)
{
    m_ambientIntensity = intensity;

    if (m_light < 0)
        return;

    if (hasShaderLighting())
        return;

    GLfloat ambientColor[] = {static_cast<float>(m_color.r) / 255.f * m_ambientIntensity,
                              static_cast<float>(m_color.g) / 255.f * m_ambientIntensity,
                              static_cast<float>(m_color.b) / 255.f * m_ambientIntensity,
                              static_cast<float>(m_color.a) / 255.f * m_ambientIntensity};
    glCheck(glLightfv(GL_LIGHT0 + m_light, GL_AMBIENT, ambientColor));
}


////////////////////////////////////////////////////////////
float Light::getAmbientIntensity() const
{
    return m_ambientIntensity;
}


////////////////////////////////////////////////////////////
void Light::setDiffuseIntensity(float intensity)
{
    m_diffuseIntensity = intensity;

    if (m_light < 0)
        return;

    if (hasShaderLighting())
        return;

    GLfloat diffuseColor[] = {static_cast<float>(m_color.r) / 255.f * m_diffuseIntensity,
                              static_cast<float>(m_color.g) / 255.f * m_diffuseIntensity,
                              static_cast<float>(m_color.b) / 255.f * m_diffuseIntensity,
                              static_cast<float>(m_color.a) / 255.f * m_diffuseIntensity};
    glCheck(glLightfv(GL_LIGHT0 + m_light, GL_DIFFUSE, diffuseColor));
}


////////////////////////////////////////////////////////////
float Light::getDiffuseIntensity() const
{
    return m_diffuseIntensity;
}


////////////////////////////////////////////////////////////
void Light::setSpecularIntensity(float intensity)
{
    m_specularIntensity = intensity;

    if (m_light < 0)
        return;

    if (hasShaderLighting())
        return;

    GLfloat specularColor[] = {static_cast<float>(m_color.r) / 255.f * m_specularIntensity,
                               static_cast<float>(m_color.g) / 255.f * m_specularIntensity,
                               static_cast<float>(m_color.b) / 255.f * m_specularIntensity,
                               static_cast<float>(m_color.a) / 255.f * m_specularIntensity};
    glCheck(glLightfv(GL_LIGHT0 + m_light, GL_SPECULAR, specularColor));
}


////////////////////////////////////////////////////////////
float Light::getSpecularIntensity() const
{
    return m_specularIntensity;
}


////////////////////////////////////////////////////////////
void Light::setConstantAttenuation(float attenuation)
{
    m_constantAttenuation = attenuation;

    if (m_light < 0)
        return;

    if (hasShaderLighting())
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

    if (hasShaderLighting())
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

    if (hasShaderLighting())
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

    Lock lock(m_lightMutex);
    m_enabledLights.insert(this);

    if (hasShaderLighting())
        return;

    glCheck(glEnable(GL_LIGHT0 + m_light));
}


////////////////////////////////////////////////////////////
void Light::disable()
{
    if (m_light < 0)
        return;

    Lock lock(m_lightMutex);
    m_enabledLights.erase(this);

    if (hasShaderLighting())
        return;

    glCheck(glDisable(GL_LIGHT0 + m_light));
}


////////////////////////////////////////////////////////////
unsigned int Light::getMaximumLights()
{
    ensureGlContext();

    if (hasShaderLighting())
    {
        // hasShaderLighting() guarantees this will be a sane value
        return (Shader::getMaximumUniformComponents() - 256) / 128;
    }

    GLint maxLights = 0;
    glCheck(glGetIntegerv(GL_MAX_LIGHTS, &maxLights));

    return static_cast<unsigned int>(maxLights);
}


////////////////////////////////////////////////////////////
void Light::enableLighting()
{
    ensureGlContext();

    Lock lock(m_lightMutex);
    m_lightingEnabled = true;

    if (hasShaderLighting())
        return;

    glCheck(glEnable(GL_LIGHTING));
}


////////////////////////////////////////////////////////////
void Light::disableLighting()
{
    ensureGlContext();

    Lock lock(m_lightMutex);
    m_lightingEnabled = false;

    if (hasShaderLighting())
        return;

    glCheck(glDisable(GL_LIGHTING));
}


////////////////////////////////////////////////////////////
bool Light::isLightingEnabled()
{
    Lock lock(m_lightMutex);
    return m_lightingEnabled;
}


////////////////////////////////////////////////////////////
bool Light::hasShaderLighting()
{
    Lock lock(m_lightMutex);

    static bool checked = false;
    static bool shaderLightingSupported = false;
    if (!checked)
    {
        checked = true;

        double versionNumber = 0.0;
        std::istringstream versionStringStream(Shader::getSupportedVersion());
        versionStringStream >> versionNumber;

// Disable non-legacy pipeline if requested
#if defined(SFML_LEGACY_GL)
        versionNumber = 0.0;
#endif

        // This will only succeed if the supported version is not GLSL ES
        if (versionNumber > 1.29)
        {
            unsigned int maxUniformComponents = Shader::getMaximumUniformComponents();

            GLint maxLegacyLights = 0;
            glCheck(glGetIntegerv(GL_MAX_LIGHTS, &maxLegacyLights));

            unsigned int requiredUniformComponents = maxLegacyLights * 128 + 256;

            if (maxUniformComponents >= requiredUniformComponents)
                shaderLightingSupported = true;
        }
    }

    return shaderLightingSupported;
}


////////////////////////////////////////////////////////////
Light& Light::operator =(const Light& right)
{
    Light temp(right);

    std::swap(m_light,                temp.m_light);
    std::swap(m_position,             temp.m_position);
    std::swap(m_directional,          temp.m_directional);
    std::swap(m_color,                temp.m_color);
    std::swap(m_ambientIntensity,     temp.m_ambientIntensity);
    std::swap(m_diffuseIntensity,     temp.m_diffuseIntensity);
    std::swap(m_specularIntensity,    temp.m_specularIntensity);
    std::swap(m_constantAttenuation,  temp.m_constantAttenuation);
    std::swap(m_linearAttenuation,    temp.m_linearAttenuation);
    std::swap(m_quadraticAttenuation, temp.m_quadraticAttenuation);
    std::swap(m_enabled,              temp.m_enabled);

    return *this;
}


////////////////////////////////////////////////////////////
void Light::getId()
{
    Lock lock(m_lightMutex);

    if (m_usedIds.empty())
        m_usedIds.resize(getMaximumLights(), false);

    for (std::size_t i = 0; i < m_usedIds.size(); ++i)
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


////////////////////////////////////////////////////////////
void Light::addToShader(const Shader& shader) const
{
    if (m_shaderElement.empty())
    {
        std::ostringstream shaderElement;
        shaderElement << "sf_Lights[" << m_light << "]";
        m_shaderElement = shaderElement.str();
    }

    shader.setParameter(m_shaderElement + ".color", m_color);
    shader.setParameter(m_shaderElement + ".ambientIntensity", m_ambientIntensity);
    shader.setParameter(m_shaderElement + ".diffuseIntensity", m_diffuseIntensity);
    shader.setParameter(m_shaderElement + ".specularIntensity", m_specularIntensity);
    shader.setParameter(m_shaderElement + ".positionDirection", m_position.x, m_position.y, m_position.z, m_directional ? 0.f : 1.f);
    shader.setParameter(m_shaderElement + ".constantAttenuation", m_constantAttenuation);
    shader.setParameter(m_shaderElement + ".linearAttenuation", m_linearAttenuation);
    shader.setParameter(m_shaderElement + ".quadraticAttenuation", m_quadraticAttenuation);
}


////////////////////////////////////////////////////////////
const std::set<const Light*>& Light::getEnabledLights()
{
    return m_enabledLights;
}

} // namespace sf

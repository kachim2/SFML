////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2014 Laurent Gomila (laurent.gom@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Graphics/Shader.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/VertexBuffer.hpp>
#include <SFML/Graphics/GLCheck.hpp>
#include <SFML/System/InputStream.hpp>
#include <SFML/System/Mutex.hpp>
#include <SFML/System/Lock.hpp>
#include <SFML/System/Err.hpp>
#include <fstream>
#include <vector>


namespace
{
    // Thread-safe unique identifier generator,
    // is used for id
    sf::Uint64 getUniqueId()
    {
        static sf::Uint64 id = 1; // start at 1, zero is "no program"
        static sf::Mutex mutex;

        sf::Lock lock(mutex);
        return id++;
    }
}


namespace
{
    // Retrieve the maximum number of texture units available
    GLint getMaxTextureUnits()
    {
        GLint maxUnits;
        glCheck(glGetIntegerv(GL_MAX_TEXTURE_COORDS_ARB, &maxUnits));
        return maxUnits;
    }

    // Read the contents of a file into an array of char
    bool getFileContents(const std::string& filename, std::vector<char>& buffer)
    {
        std::ifstream file(filename.c_str(), std::ios_base::binary);
        if (file)
        {
            file.seekg(0, std::ios_base::end);
            std::streamsize size = file.tellg();
            if (size > 0)
            {
                file.seekg(0, std::ios_base::beg);
                buffer.resize(static_cast<std::size_t>(size));
                file.read(&buffer[0], size);
            }
            buffer.push_back('\0');
            return true;
        }
        else
        {
            return false;
        }
    }

    // Read the contents of a stream into an array of char
    bool getStreamContents(sf::InputStream& stream, std::vector<char>& buffer)
    {
        bool success = true;
        sf::Int64 size = stream.getSize();
        if (size > 0)
        {
            buffer.resize(static_cast<std::size_t>(size));
            stream.seek(0);
            sf::Int64 read = stream.read(&buffer[0], size);
            success = (read == size);
        }
        buffer.push_back('\0');
        return success;
    }
}


namespace sf
{
////////////////////////////////////////////////////////////
Shader::CurrentTextureType Shader::CurrentTexture;


////////////////////////////////////////////////////////////
Shader::Shader() :
m_shaderProgram (0),
m_currentTexture(-1),
m_textures      (),
m_params        (),
m_attributes    (),
m_blockBindings (),
m_warnMissing   (true),
m_id            (0),
m_parameterBlock(false),
m_blockProgram  (0)
{
}


////////////////////////////////////////////////////////////
Shader::~Shader()
{
    ensureGlContext();

    // Destroy effect program
    if (m_shaderProgram)
        glCheck(glDeleteObjectARB(m_shaderProgram));
}


////////////////////////////////////////////////////////////
bool Shader::loadFromFile(const std::string& filename, Type type)
{
    // Read the file
    std::vector<char> shader;
    if (!getFileContents(filename, shader))
    {
        err() << "Failed to open shader file \"" << filename << "\"" << std::endl;
        return false;
    }

    // Compile the shader program
    if (type == Vertex)
        return compile(&shader[0], NULL, NULL);
    else if (type == Fragment)
        return compile(NULL, &shader[0], NULL);
    else
        return compile(NULL, NULL, &shader[0]);
}


////////////////////////////////////////////////////////////
bool Shader::loadFromFile(const std::string& vertexShaderFilename, const std::string& fragmentShaderFilename, const std::string& geometryShaderFilename)
{
    // Read the vertex shader file
    std::vector<char> vertexShader;
    if (!getFileContents(vertexShaderFilename, vertexShader))
    {
        err() << "Failed to open vertex shader file \"" << vertexShaderFilename << "\"" << std::endl;
        return false;
    }

    // Read the fragment shader file
    std::vector<char> fragmentShader;
    if (!getFileContents(fragmentShaderFilename, fragmentShader))
    {
        err() << "Failed to open fragment shader file \"" << fragmentShaderFilename << "\"" << std::endl;
        return false;
    }

    // Read the fragment shader file
    std::vector<char> geometryShader;
    if (!geometryShaderFilename.empty() && !getFileContents(geometryShaderFilename, geometryShader))
    {
        err() << "Failed to open geometry shader file \"" << geometryShaderFilename << "\"" << std::endl;
        return false;
    }

    // Compile the shader program
    return compile(&vertexShader[0], &fragmentShader[0], geometryShaderFilename.empty() ? NULL : &geometryShader[0]);
}


////////////////////////////////////////////////////////////
bool Shader::loadFromMemory(const std::string& shader, Type type)
{
    // Compile the shader program
    if (type == Vertex)
        return compile(shader.c_str(), NULL, NULL);
    else if (type == Fragment)
        return compile(NULL, shader.c_str(), NULL);
    else
        return compile(NULL, NULL, shader.c_str());
}


////////////////////////////////////////////////////////////
bool Shader::loadFromMemory(const std::string& vertexShader, const std::string& fragmentShader, const std::string& geometryShader)
{
    // Compile the shader program
    return compile(vertexShader.c_str(), fragmentShader.c_str(), geometryShader.empty() ? NULL : geometryShader.c_str());
}


////////////////////////////////////////////////////////////
bool Shader::loadFromStream(InputStream& stream, Type type)
{
    // Read the shader code from the stream
    std::vector<char> shader;
    if (!getStreamContents(stream, shader))
    {
        err() << "Failed to read shader from stream" << std::endl;
        return false;
    }

    // Compile the shader program
    if (type == Vertex)
        return compile(&shader[0], NULL, NULL);
    else if (type == Fragment)
        return compile(NULL, &shader[0], NULL);
    else
        return compile(NULL, NULL, &shader[0]);
}


////////////////////////////////////////////////////////////
bool Shader::loadFromStream(InputStream& vertexShaderStream, InputStream& fragmentShaderStream)
{
    // Read the vertex shader code from the stream
    std::vector<char> vertexShader;
    if (!getStreamContents(vertexShaderStream, vertexShader))
    {
        err() << "Failed to read vertex shader from stream" << std::endl;
        return false;
    }

    // Read the fragment shader code from the stream
    std::vector<char> fragmentShader;
    if (!getStreamContents(fragmentShaderStream, fragmentShader))
    {
        err() << "Failed to read fragment shader from stream" << std::endl;
        return false;
    }

    // Compile the shader program
    return compile(&vertexShader[0], &fragmentShader[0], NULL);
}


////////////////////////////////////////////////////////////
bool Shader::loadFromStream(InputStream& vertexShaderStream, InputStream& fragmentShaderStream, InputStream& geometryShaderStream)
{
    // Read the vertex shader code from the stream
    std::vector<char> vertexShader;
    if (!getStreamContents(vertexShaderStream, vertexShader))
    {
        err() << "Failed to read vertex shader from stream" << std::endl;
        return false;
    }

    // Read the fragment shader code from the stream
    std::vector<char> fragmentShader;
    if (!getStreamContents(fragmentShaderStream, fragmentShader))
    {
        err() << "Failed to read fragment shader from stream" << std::endl;
        return false;
    }

    // Read the geometry shader code from the stream
    std::vector<char> geometryShader;
    if (!getStreamContents(geometryShaderStream, geometryShader))
    {
        err() << "Failed to read geometry shader from stream" << std::endl;
        return false;
    }

    // Compile the shader program
    return compile(&vertexShader[0], &fragmentShader[0], &geometryShader[0]);
}


////////////////////////////////////////////////////////////
void Shader::setParameter(const std::string& name, int x) const
{
    if (m_shaderProgram)
    {
        ensureGlContext();

        GLhandleARB program = 0;
        if (!m_parameterBlock)
        {
            // Enable program
            program = glGetHandleARB(GL_PROGRAM_OBJECT_ARB);
            if (program != m_shaderProgram)
                glCheck(glUseProgramObjectARB(m_shaderProgram));
        }

        // Get parameter location and assign it new values
        GLint location = getParamLocation(name);
        if (location != -1)
            glCheck(glUniform1iARB(location, x));

        if (!m_parameterBlock)
        {
            // Disable program
            if (program != m_shaderProgram)
                glCheck(glUseProgramObjectARB(program));
        }
    }
}


////////////////////////////////////////////////////////////
void Shader::setParameter(const std::string& name, int x, int y) const
{
    if (m_shaderProgram)
    {
        ensureGlContext();

        GLhandleARB program = 0;
        if (!m_parameterBlock)
        {
            // Enable program
            program = glGetHandleARB(GL_PROGRAM_OBJECT_ARB);
            if (program != m_shaderProgram)
                glCheck(glUseProgramObjectARB(m_shaderProgram));
        }

        // Get parameter location and assign it new values
        GLint location = getParamLocation(name);
        if (location != -1)
            glCheck(glUniform2iARB(location, x, y));

        if (!m_parameterBlock)
        {
            // Disable program
            if (program != m_shaderProgram)
                glCheck(glUseProgramObjectARB(program));
        }
    }
}


////////////////////////////////////////////////////////////
void Shader::setParameter(const std::string& name, int x, int y, int z) const
{
    if (m_shaderProgram)
    {
        ensureGlContext();

        GLhandleARB program = 0;
        if (!m_parameterBlock)
        {
            // Enable program
            program = glGetHandleARB(GL_PROGRAM_OBJECT_ARB);
            if (program != m_shaderProgram)
                glCheck(glUseProgramObjectARB(m_shaderProgram));
        }

        // Get parameter location and assign it new values
        GLint location = getParamLocation(name);
        if (location != -1)
            glCheck(glUniform3iARB(location, x, y, z));

        if (!m_parameterBlock)
        {
            // Disable program
            if (program != m_shaderProgram)
                glCheck(glUseProgramObjectARB(program));
        }
    }
}


////////////////////////////////////////////////////////////
void Shader::setParameter(const std::string& name, int x, int y, int z, int w) const
{
    if (m_shaderProgram)
    {
        ensureGlContext();

        GLhandleARB program = 0;
        if (!m_parameterBlock)
        {
            // Enable program
            program = glGetHandleARB(GL_PROGRAM_OBJECT_ARB);
            if (program != m_shaderProgram)
                glCheck(glUseProgramObjectARB(m_shaderProgram));
        }

        // Get parameter location and assign it new values
        GLint location = getParamLocation(name);
        if (location != -1)
            glCheck(glUniform4iARB(location, x, y, z, w));

        if (!m_parameterBlock)
        {
            // Disable program
            if (program != m_shaderProgram)
                glCheck(glUseProgramObjectARB(program));
        }
    }
}


////////////////////////////////////////////////////////////
void Shader::setParameter(const std::string& name, const Vector2i& v) const
{
    setParameter(name, v.x, v.y);
}


////////////////////////////////////////////////////////////
void Shader::setParameter(const std::string& name, const Vector3i& v) const
{
    setParameter(name, v.x, v.y, v.z);
}


////////////////////////////////////////////////////////////
void Shader::setParameter(const std::string& name, float x) const
{
    if (m_shaderProgram)
    {
        ensureGlContext();

        GLhandleARB program = 0;
        if (!m_parameterBlock)
        {
            // Enable program
            program = glGetHandleARB(GL_PROGRAM_OBJECT_ARB);
            if (program != m_shaderProgram)
                glCheck(glUseProgramObjectARB(m_shaderProgram));
        }

        // Get parameter location and assign it new values
        GLint location = getParamLocation(name);
        if (location != -1)
            glCheck(glUniform1fARB(location, x));

        if (!m_parameterBlock)
        {
            // Disable program
            if (program != m_shaderProgram)
                glCheck(glUseProgramObjectARB(program));
        }
    }
}


////////////////////////////////////////////////////////////
void Shader::setParameter(const std::string& name, float x, float y) const
{
    if (m_shaderProgram)
    {
        ensureGlContext();

        GLhandleARB program = 0;
        if (!m_parameterBlock)
        {
            // Enable program
            program = glGetHandleARB(GL_PROGRAM_OBJECT_ARB);
            if (program != m_shaderProgram)
                glCheck(glUseProgramObjectARB(m_shaderProgram));
        }

        // Get parameter location and assign it new values
        GLint location = getParamLocation(name);
        if (location != -1)
            glCheck(glUniform2fARB(location, x, y));

        if (!m_parameterBlock)
        {
            // Disable program
            if (program != m_shaderProgram)
                glCheck(glUseProgramObjectARB(program));
        }
    }
}


////////////////////////////////////////////////////////////
void Shader::setParameter(const std::string& name, float x, float y, float z) const
{
    if (m_shaderProgram)
    {
        ensureGlContext();

        GLhandleARB program = 0;
        if (!m_parameterBlock)
        {
            // Enable program
            program = glGetHandleARB(GL_PROGRAM_OBJECT_ARB);
            if (program != m_shaderProgram)
                glCheck(glUseProgramObjectARB(m_shaderProgram));
        }

        // Get parameter location and assign it new values
        GLint location = getParamLocation(name);
        if (location != -1)
            glCheck(glUniform3fARB(location, x, y, z));

        if (!m_parameterBlock)
        {
            // Disable program
            if (program != m_shaderProgram)
                glCheck(glUseProgramObjectARB(program));
        }
    }
}


////////////////////////////////////////////////////////////
void Shader::setParameter(const std::string& name, float x, float y, float z, float w) const
{
    if (m_shaderProgram)
    {
        ensureGlContext();

        GLhandleARB program = 0;
        if (!m_parameterBlock)
        {
            // Enable program
            program = glGetHandleARB(GL_PROGRAM_OBJECT_ARB);
            if (program != m_shaderProgram)
                glCheck(glUseProgramObjectARB(m_shaderProgram));
        }

        // Get parameter location and assign it new values
        GLint location = getParamLocation(name);
        if (location != -1)
            glCheck(glUniform4fARB(location, x, y, z, w));

        if (!m_parameterBlock)
        {
            // Disable program
            if (program != m_shaderProgram)
                glCheck(glUseProgramObjectARB(program));
        }
    }
}


////////////////////////////////////////////////////////////
void Shader::setParameter(const std::string& name, const Vector2f& v) const
{
    setParameter(name, v.x, v.y);
}


////////////////////////////////////////////////////////////
void Shader::setParameter(const std::string& name, const Vector3f& v) const
{
    setParameter(name, v.x, v.y, v.z);
}


////////////////////////////////////////////////////////////
void Shader::setParameter(const std::string& name, const Color& color) const
{
    setParameter(name, color.r / 255.f, color.g / 255.f, color.b / 255.f, color.a / 255.f);
}


////////////////////////////////////////////////////////////
void Shader::setParameter(const std::string& name, const sf::Transform& transform) const
{
    if (m_shaderProgram)
    {
        ensureGlContext();

        GLhandleARB program = 0;
        if (!m_parameterBlock)
        {
            // Enable program
            program = glGetHandleARB(GL_PROGRAM_OBJECT_ARB);
            if (program != m_shaderProgram)
                glCheck(glUseProgramObjectARB(m_shaderProgram));
        }

        // Get parameter location and assign it new values
        GLint location = getParamLocation(name);
        if (location != -1)
            glCheck(glUniformMatrix4fvARB(location, 1, GL_FALSE, transform.getMatrix()));

        if (!m_parameterBlock)
        {
            // Disable program
            if (program != m_shaderProgram)
                glCheck(glUseProgramObjectARB(program));
        }
    }
}


////////////////////////////////////////////////////////////
void Shader::setParameter(const std::string& name, const Texture& texture) const
{
    if (m_shaderProgram)
    {
        ensureGlContext();

        // Find the location of the variable in the shader
        int location = getParamLocation(name);
        if (location != -1)
        {
            // Store the location -> texture mapping
            TextureTable::iterator it = m_textures.find(location);
            if (it == m_textures.end())
            {
                // New entry, make sure there are enough texture units
                static const GLint maxUnits = getMaxTextureUnits();
                if (m_textures.size() + 1 >= static_cast<std::size_t>(maxUnits))
                {
                    err() << "Impossible to use texture \"" << name << "\" for shader: all available texture units are used" << std::endl;
                    return;
                }

                m_textures[location] = &texture;
            }
            else
            {
                // Location already used, just replace the texture
                it->second = &texture;
            }
        }
    }
}


////////////////////////////////////////////////////////////
void Shader::setParameter(const std::string& name, CurrentTextureType) const
{
    if (m_shaderProgram)
    {
        ensureGlContext();

        // Find the location of the variable in the shader
        m_currentTexture = getParamLocation(name);
    }
}


////////////////////////////////////////////////////////////
void Shader::setBlock(const std::string& name, const VertexBuffer& buffer) const
{
    if (!isUniformBufferAvailable())
        return;

    if (m_shaderProgram)
    {
        ensureGlContext();

        GLhandleARB program = 0;
        if (!m_parameterBlock)
        {
            // Enable program
            program = glGetHandleARB(GL_PROGRAM_OBJECT_ARB);
            if (program != m_shaderProgram)
                glCheck(glUseProgramObjectARB(m_shaderProgram));
        }

        VertexBuffer::bind(&buffer, GL_UNIFORM_BUFFER);

        BufferTable::const_iterator it = m_boundBuffers.find(name);
        if ((it == m_boundBuffers.end()) || (it->second != buffer.m_cacheId))
        {
            // Get block binding and bind the buffer
            GLint binding = getBlockBinding(name);
            if (binding != -1)
            {
                glCheck(glBindBufferBase(GL_UNIFORM_BUFFER, binding, buffer.getBufferObjectName()));
                m_boundBuffers[name] = buffer.m_cacheId;
            }
        }

        VertexBuffer::bind(NULL, GL_UNIFORM_BUFFER);

        if (!m_parameterBlock)
        {
            // Disable program
            if (program != m_shaderProgram)
                glCheck(glUseProgramObjectARB(program));
        }
    }
}


////////////////////////////////////////////////////////////
int Shader::getVertexAttributeLocation(const std::string& name) const
{
    // Check the cache
    LocationTable::const_iterator it = m_attributes.find(name);
    if (it != m_attributes.end())
    {
        // Already in cache, return it
        return it->second;
    }
    else
    {
        // Not in cache, request the location from OpenGL
        int location = glGetAttribLocationARB(m_shaderProgram, name.c_str());
        if (location == -1)
        {
            // Error: location not found
            if (m_warnMissing)
                err() << "Vertex attribute \"" << name << "\" not found in shader" << std::endl;
        }

        m_attributes.insert(std::make_pair(name, location));

        return location;
    }
}


////////////////////////////////////////////////////////////
bool Shader::warnMissing(bool warn) const
{
    bool previousWarn = m_warnMissing;
    m_warnMissing = warn;
    return previousWarn;
}


////////////////////////////////////////////////////////////
void Shader::beginParameterBlock() const
{
    m_parameterBlock = true;

    m_blockProgram = static_cast<unsigned int>(glGetHandleARB(GL_PROGRAM_OBJECT_ARB));

    if (m_blockProgram != m_shaderProgram)
        glCheck(glUseProgramObjectARB(m_shaderProgram));
}


////////////////////////////////////////////////////////////
void Shader::endParameterBlock() const
{
    m_parameterBlock = false;

    if (m_blockProgram != m_shaderProgram)
        glCheck(glUseProgramObjectARB(m_blockProgram));
}


////////////////////////////////////////////////////////////
unsigned int Shader::getProgramObject() const
{
    return m_shaderProgram;
}


////////////////////////////////////////////////////////////
void Shader::bind(const Shader* shader)
{
    ensureGlContext();

    if (shader && shader->m_shaderProgram)
    {
        // Enable the program
        glCheck(glUseProgramObjectARB(shader->m_shaderProgram));

        // Bind the textures
        shader->bindTextures();

        // Bind the current texture
        if (shader->m_currentTexture != -1)
            glCheck(glUniform1iARB(shader->m_currentTexture, 0));
    }
    else
    {
        // Bind no shader
        glCheck(glUseProgramObjectARB(0));
    }
}


////////////////////////////////////////////////////////////
bool Shader::isAvailable()
{
    ensureGlContext();

    // Make sure that GLEW is initialized
    priv::ensureGlewInit();

    return GLEW_ARB_shading_language_100 &&
           GLEW_ARB_shader_objects       &&
           GLEW_ARB_vertex_shader        &&
           GLEW_ARB_fragment_shader;
}


////////////////////////////////////////////////////////////
bool Shader::isGeometryShaderAvailable()
{
    return isAvailable() && GLEW_VERSION_3_2;
}


////////////////////////////////////////////////////////////
bool Shader::isUniformBufferAvailable()
{
    static bool checked = false;
    static bool uniformBufferSupported = false;

    if (!checked)
    {
        checked = true;

        uniformBufferSupported = isAvailable() && VertexBuffer::isAvailable() && GLEW_ARB_uniform_buffer_object;
    }

    return uniformBufferSupported;
}


////////////////////////////////////////////////////////////
std::string Shader::getSupportedVersion()
{
    static bool checked = false;
    static std::string supportedVersion;

    if (!checked)
    {
        checked = true;

        if (isAvailable())
        {
            const GLubyte* version = NULL;
            glCheck(version = glGetString(GL_SHADING_LANGUAGE_VERSION_ARB));

            if (!version)
                return supportedVersion;

            std::string versionString(reinterpret_cast<const char*>(version));

            // Get rid of OpenGL ES GLSL declaration
            // (but keep a remaining ES prefix so one can check for it)
            if (versionString.find("OpenGL ES GLSL ") != std::string::npos)
                versionString = versionString.substr(15);

            supportedVersion = versionString;
        }
    }

    return supportedVersion;
}


////////////////////////////////////////////////////////////
unsigned int Shader::getMaximumUniformComponents()
{
    if (!isAvailable())
        return 0;

    GLint maxVertexUniformComponents = 0;
    glCheck(glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS_ARB, &maxVertexUniformComponents));
    return maxVertexUniformComponents;
}


////////////////////////////////////////////////////////////
bool Shader::compile(const char* vertexShaderCode, const char* fragmentShaderCode, const char* geometryShaderCode)
{
    ensureGlContext();

    // First make sure that we can use shaders
    if (!isAvailable())
    {
        err() << "Failed to create a shader: your system doesn't support shaders "
              << "(you should test Shader::isAvailable() before trying to use the Shader class)" << std::endl;
        return false;
    }

    // Destroy the shader if it was already created
    if (m_shaderProgram)
        glCheck(glDeleteObjectARB(m_shaderProgram));

    // Reset the internal state
    m_currentTexture = -1;
    m_textures.clear();
    m_params.clear();
    m_attributes.clear();
    m_blockBindings.clear();
    m_boundBuffers.clear();

    // Create the program
    m_shaderProgram = glCreateProgramObjectARB();

    // Create the vertex shader if needed
    if (vertexShaderCode)
    {
        // Create and compile the shader
        GLhandleARB vertexShader = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
        glCheck(glShaderSourceARB(vertexShader, 1, &vertexShaderCode, NULL));
        glCheck(glCompileShaderARB(vertexShader));

        // Check the compile log
        GLint success;
        glCheck(glGetObjectParameterivARB(vertexShader, GL_OBJECT_COMPILE_STATUS_ARB, &success));
        if (success == GL_FALSE)
        {
            char log[1024];
            glCheck(glGetInfoLogARB(vertexShader, sizeof(log), 0, log));
            err() << "Failed to compile vertex shader:" << std::endl
                  << log << std::endl;
            glCheck(glDeleteObjectARB(vertexShader));
            glCheck(glDeleteObjectARB(m_shaderProgram));
            m_shaderProgram = 0;
            return false;
        }

        // Attach the shader to the program, and delete it (not needed anymore)
        glCheck(glAttachObjectARB(m_shaderProgram, vertexShader));
        glCheck(glDeleteObjectARB(vertexShader));
    }

    // Create the fragment shader if needed
    if (fragmentShaderCode)
    {
        // Create and compile the shader
        GLhandleARB fragmentShader = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
        glCheck(glShaderSourceARB(fragmentShader, 1, &fragmentShaderCode, NULL));
        glCheck(glCompileShaderARB(fragmentShader));

        // Check the compile log
        GLint success;
        glCheck(glGetObjectParameterivARB(fragmentShader, GL_OBJECT_COMPILE_STATUS_ARB, &success));
        if (success == GL_FALSE)
        {
            char log[1024];
            glCheck(glGetInfoLogARB(fragmentShader, sizeof(log), 0, log));
            err() << "Failed to compile fragment shader:" << std::endl
                  << log << std::endl;
            glCheck(glDeleteObjectARB(fragmentShader));
            glCheck(glDeleteObjectARB(m_shaderProgram));
            m_shaderProgram = 0;
            return false;
        }

        // Attach the shader to the program, and delete it (not needed anymore)
        glCheck(glAttachObjectARB(m_shaderProgram, fragmentShader));
        glCheck(glDeleteObjectARB(fragmentShader));
    }

    // Create the geometry shader if needed
    if (geometryShaderCode)
    {
        // Create and compile the shader
        GLhandleARB geometryShader = glCreateShaderObjectARB(GL_GEOMETRY_SHADER);
        glCheck(glShaderSourceARB(geometryShader, 1, &geometryShaderCode, NULL));
        glCheck(glCompileShaderARB(geometryShader));

        // Check the compile log
        GLint success;
        glCheck(glGetObjectParameterivARB(geometryShader, GL_OBJECT_COMPILE_STATUS_ARB, &success));
        if (success == GL_FALSE)
        {
            char log[1024];
            glCheck(glGetInfoLogARB(geometryShader, sizeof(log), 0, log));
            err() << "Failed to compile geometry shader:" << std::endl
                  << log << std::endl;
            glCheck(glDeleteObjectARB(geometryShader));
            glCheck(glDeleteObjectARB(m_shaderProgram));
            m_shaderProgram = 0;
            return false;
        }

        // Attach the shader to the program, and delete it (not needed anymore)
        glCheck(glAttachObjectARB(m_shaderProgram, geometryShader));
        glCheck(glDeleteObjectARB(geometryShader));
    }

    // Link the program
    glCheck(glLinkProgramARB(m_shaderProgram));

    // Check the link log
    GLint success;
    glCheck(glGetObjectParameterivARB(m_shaderProgram, GL_OBJECT_LINK_STATUS_ARB, &success));
    if (success == GL_FALSE)
    {
        char log[1024];
        glCheck(glGetInfoLogARB(m_shaderProgram, sizeof(log), 0, log));
        err() << "Failed to link shader:" << std::endl
              << log << std::endl;
        glCheck(glDeleteObjectARB(m_shaderProgram));
        m_shaderProgram = 0;
        return false;
    }

    // Force an OpenGL flush, so that the shader will appear updated
    // in all contexts immediately (solves problems in multi-threaded apps)
    glCheck(glFlush());

    m_id = getUniqueId();

    return true;
}


////////////////////////////////////////////////////////////
void Shader::bindTextures() const
{
    TextureTable::const_iterator it = m_textures.begin();
    for (std::size_t i = 0; i < m_textures.size(); ++i)
    {
        GLint index = static_cast<GLsizei>(i + 1);
        glCheck(glUniform1iARB(it->first, index));
        glCheck(glActiveTextureARB(GL_TEXTURE0_ARB + index));
        Texture::bind(it->second);
        ++it;
    }

    // Make sure that the texture unit which is left active is the number 0
    if (!m_textures.empty())
        glCheck(glActiveTextureARB(GL_TEXTURE0_ARB));
}


////////////////////////////////////////////////////////////
int Shader::getParamLocation(const std::string& name) const
{
    // Check the cache
    LocationTable::const_iterator it = m_params.find(name);
    if (it != m_params.end())
    {
        // Already in cache, return it
        return it->second;
    }
    else
    {
        // Not in cache, request the location from OpenGL
        int location = glGetUniformLocationARB(m_shaderProgram, name.c_str());
        if (location == -1)
        {
            // Error: location not found
            if (m_warnMissing)
                err() << "Uniform \"" << name << "\" not found in shader" << std::endl;
        }

        m_params.insert(std::make_pair(name, location));

        return location;
    }
}


////////////////////////////////////////////////////////////
int Shader::getBlockBinding(const std::string& name) const
{
    // Check the cache
    LocationTable::const_iterator it = m_blockBindings.find(name);
    if (it != m_blockBindings.end())
    {
        // Already in cache, return it
        return it->second;
    }
    else
    {
        // Not in cache, request the block index
        // from OpenGL and create a new binding
        int binding = -1;

        // Check if we can create a new binding
        static int maxBindings = -1;
        if (maxBindings < 0)
            glCheck(glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &maxBindings));

        if (static_cast<int>(m_blockBindings.size()) >= (maxBindings - 1))
        {
            err() << "Cannot create uniform block binding, "
                     "out of bindings (Max: " << maxBindings << ")" << std::endl;
            return binding;
        }

        unsigned int index = 0;
        glCheck(index = glGetUniformBlockIndex(m_shaderProgram, name.c_str()));
        if (index != GL_INVALID_INDEX)
        {
            binding = static_cast<int>(m_blockBindings.size());
            glCheck(glUniformBlockBinding(m_shaderProgram, index, static_cast<unsigned int>(binding)));
        }
        else
        {
            // Error: index not found
            if (m_warnMissing)
                err() << "Uniform block \"" << name << "\" not found in shader" << std::endl;
        }

        m_blockBindings.insert(std::make_pair(name, binding));

        return binding;
    }
}

} // namespace sf

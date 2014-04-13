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

#ifndef SFML_SHADER_HPP
#define SFML_SHADER_HPP

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Graphics/Export.hpp>
#include <SFML/Graphics/Transform.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Window/GlResource.hpp>
#include <SFML/System/NonCopyable.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>
#include <map>
#include <string>


namespace sf
{
class InputStream;
class Texture;
class VertexBuffer;

////////////////////////////////////////////////////////////
/// \brief Shader class (vertex and fragment)
///
////////////////////////////////////////////////////////////
class SFML_GRAPHICS_API Shader : GlResource, NonCopyable
{
public :

    ////////////////////////////////////////////////////////////
    /// \brief Types of shaders
    ///
    ////////////////////////////////////////////////////////////
    enum Type
    {
        Vertex,   ///< Vertex shader
        Fragment, ///< Fragment (pixel) shader
        Geometry  ///< Geometry shader
    };

    ////////////////////////////////////////////////////////////
    /// \brief Special type that can be passed to setParameter,
    ///        and that represents the texture of the object being drawn
    ///
    /// \see setParameter(const std::string&, CurrentTextureType)
    ///
    ////////////////////////////////////////////////////////////
    struct CurrentTextureType {};

    ////////////////////////////////////////////////////////////
    /// \brief Represents the texture of the object being drawn
    ///
    /// \see setParameter(const std::string&, CurrentTextureType)
    ///
    ////////////////////////////////////////////////////////////
    static CurrentTextureType CurrentTexture;

public :

    ////////////////////////////////////////////////////////////
    /// \brief Default constructor
    ///
    /// This constructor creates an invalid shader.
    ///
    ////////////////////////////////////////////////////////////
    Shader();

    ////////////////////////////////////////////////////////////
    /// \brief Destructor
    ///
    ////////////////////////////////////////////////////////////
    ~Shader();

    ////////////////////////////////////////////////////////////
    /// \brief Load either the vertex, fragment or geometry shader from a file
    ///
    /// This function loads a single shader, either vertex,
    /// fragment or geometry, identified by the second argument.
    /// The source must be a text file containing a valid
    /// shader in GLSL language. GLSL is a C-like language
    /// dedicated to OpenGL shaders; you'll probably need to
    /// read a good documentation for it before writing your
    /// own shaders.
    ///
    /// \param filename Path of the vertex, fragment or geometry shader file to load
    /// \param type     Type of shader (vertex, fragment or geometry)
    ///
    /// \return True if loading succeeded, false if it failed
    ///
    /// \see loadFromMemory, loadFromStream
    ///
    ////////////////////////////////////////////////////////////
    bool loadFromFile(const std::string& filename, Type type);

    ////////////////////////////////////////////////////////////
    /// \brief Load both the vertex and fragment shaders and optionally a geometry shader from files
    ///
    /// This function loads both the vertex and the
    /// fragment shaders and optionally a geometry shader.
    /// If one of them fails to load, the shader is left
    /// empty (the valid shader is unloaded).
    /// The sources must be text files containing valid shaders
    /// in GLSL language. GLSL is a C-like language dedicated to
    /// OpenGL shaders; you'll probably need to read a good documentation
    /// for it before writing your own shaders.
    ///
    /// \param vertexShaderFilename   Path of the vertex shader file to load
    /// \param fragmentShaderFilename Path of the fragment shader file to load
    /// \param geometryShaderFilename Path of the geometry shader file to load (optional)
    ///
    /// \return True if loading succeeded, false if it failed
    ///
    /// \see loadFromMemory, loadFromStream
    ///
    ////////////////////////////////////////////////////////////
    bool loadFromFile(const std::string& vertexShaderFilename, const std::string& fragmentShaderFilename, const std::string& geometryShaderFilename = "");

    ////////////////////////////////////////////////////////////
    /// \brief Load either the vertex, fragment or geometry shader from a source code in memory
    ///
    /// This function loads a single shader, either vertex,
    /// fragment or geometry, identified by the second argument.
    /// The source code must be a valid shader in GLSL language.
    /// GLSL is a C-like language dedicated to OpenGL shaders;
    /// you'll probably need to read a good documentation for
    /// it before writing your own shaders.
    ///
    /// \param shader String containing the source code of the shader
    /// \param type   Type of shader (vertex, fragment or geometry)
    ///
    /// \return True if loading succeeded, false if it failed
    ///
    /// \see loadFromFile, loadFromStream
    ///
    ////////////////////////////////////////////////////////////
    bool loadFromMemory(const std::string& shader, Type type);

    ////////////////////////////////////////////////////////////
    /// \brief Load both the vertex and fragment shaders and optionally a geometry shader from source codes in memory
    ///
    /// This function loads both the vertex and the
    /// fragment shaders and optionally a geometry shader.
    /// If one of them fails to load, the shader is left
    /// empty (the valid shader is unloaded).
    /// The sources must be valid shaders in GLSL language. GLSL is
    /// a C-like language dedicated to OpenGL shaders; you'll
    /// probably need to read a good documentation for it before
    /// writing your own shaders.
    ///
    /// \param vertexShader   String containing the source code of the vertex shader
    /// \param fragmentShader String containing the source code of the fragment shader
    /// \param geometryShader String containing the source code of the geometry shader (optional)
    ///
    /// \return True if loading succeeded, false if it failed
    ///
    /// \see loadFromFile, loadFromStream
    ///
    ////////////////////////////////////////////////////////////
    bool loadFromMemory(const std::string& vertexShader, const std::string& fragmentShader, const std::string& geometryShader = "");

    ////////////////////////////////////////////////////////////
    /// \brief Load either the vertex, fragment or geometry shader from a custom stream
    ///
    /// This function loads a single shader, either vertex,
    /// fragment or geometry, identified by the second argument.
    /// The source code must be a valid shader in GLSL language.
    /// GLSL is a C-like language dedicated to OpenGL shaders;
    /// you'll probably need to read a good documentation for it
    /// before writing your own shaders.
    ///
    /// \param stream Source stream to read from
    /// \param type   Type of shader (vertex, fragment or geometry)
    ///
    /// \return True if loading succeeded, false if it failed
    ///
    /// \see loadFromFile, loadFromMemory
    ///
    ////////////////////////////////////////////////////////////
    bool loadFromStream(InputStream& stream, Type type);

    ////////////////////////////////////////////////////////////
    /// \brief Load both the vertex and fragment shaders from custom streams
    ///
    /// This function loads both the vertex and the fragment
    /// shaders. If one of them fails to load, the shader is left
    /// empty (the valid shader is unloaded).
    /// The source codes must be valid shaders in GLSL language.
    /// GLSL is a C-like language dedicated to OpenGL shaders;
    /// you'll probably need to read a good documentation for
    /// it before writing your own shaders.
    ///
    /// \param vertexShaderStream   Source stream to read the vertex shader from
    /// \param fragmentShaderStream Source stream to read the fragment shader from
    ///
    /// \return True if loading succeeded, false if it failed
    ///
    /// \see loadFromFile, loadFromMemory
    ///
    ////////////////////////////////////////////////////////////
    bool loadFromStream(InputStream& vertexShaderStream, InputStream& fragmentShaderStream);

    ////////////////////////////////////////////////////////////
    /// \brief Load the vertex, fragment and geometry shaders from custom streams
    ///
    /// This function loads the vertex, fragment and geometry
    /// shaders. If one of them fails to load, the shader is left
    /// empty (the valid shader is unloaded).
    /// The source codes must be valid shaders in GLSL language.
    /// GLSL is a C-like language dedicated to OpenGL shaders;
    /// you'll probably need to read a good documentation for
    /// it before writing your own shaders.
    ///
    /// \param vertexShaderStream   Source stream to read the vertex shader from
    /// \param fragmentShaderStream Source stream to read the fragment shader from
    /// \param geometryShaderStream Source stream to read the geometry shader from
    ///
    /// \return True if loading succeeded, false if it failed
    ///
    /// \see loadFromFile, loadFromMemory
    ///
    ////////////////////////////////////////////////////////////
    bool loadFromStream(InputStream& vertexShaderStream, InputStream& fragmentShaderStream, InputStream& geometryShaderStream);

    ////////////////////////////////////////////////////////////
    /// \brief Change a int parameter of the shader
    ///
    /// \a name is the name of the variable to change in the shader.
    /// The corresponding parameter in the shader must be a int
    /// (int GLSL type).
    ///
    /// Example:
    /// \code
    /// uniform int myparam; // this is the variable in the shader
    /// \endcode
    /// \code
    /// shader.setParameter("myparam", 5);
    /// \endcode
    ///
    /// \param name Name of the parameter in the shader
    /// \param x    Value to assign
    ///
    ////////////////////////////////////////////////////////////
    void setParameter(const std::string& name, int x) const;

    ////////////////////////////////////////////////////////////
    /// \brief Change a 2-components int vector parameter of the shader
    ///
    /// \a name is the name of the variable to change in the shader.
    /// The corresponding parameter in the shader must be a 2x1 int vector
    /// (ivec2 GLSL type).
    ///
    /// Example:
    /// \code
    /// uniform ivec2 myparam; // this is the variable in the shader
    /// \endcode
    /// \code
    /// shader.setParameter("myparam", 5, 6);
    /// \endcode
    ///
    /// \param name Name of the parameter in the shader
    /// \param x    First component of the value to assign
    /// \param y    Second component of the value to assign
    ///
    ////////////////////////////////////////////////////////////
    void setParameter(const std::string& name, int x, int y) const;

    ////////////////////////////////////////////////////////////
    /// \brief Change a 3-components int vector parameter of the shader
    ///
    /// \a name is the name of the variable to change in the shader.
    /// The corresponding parameter in the shader must be a 3x1 int vector
    /// (ivec3 GLSL type).
    ///
    /// Example:
    /// \code
    /// uniform ivec3 myparam; // this is the variable in the shader
    /// \endcode
    /// \code
    /// shader.setParameter("myparam", 5, 6, -8);
    /// \endcode
    ///
    /// \param name Name of the parameter in the shader
    /// \param x    First component of the value to assign
    /// \param y    Second component of the value to assign
    /// \param z    Third component of the value to assign
    ///
    ////////////////////////////////////////////////////////////
    void setParameter(const std::string& name, int x, int y, int z) const;

    ////////////////////////////////////////////////////////////
    /// \brief Change a 4-components int vector parameter of the shader
    ///
    /// \a name is the name of the variable to change in the shader.
    /// The corresponding parameter in the shader must be a 4x1 int vector
    /// (ivec4 GLSL type).
    ///
    /// Example:
    /// \code
    /// uniform ivec4 myparam; // this is the variable in the shader
    /// \endcode
    /// \code
    /// shader.setParameter("myparam", 5, 6, -8, 0);
    /// \endcode
    ///
    /// \param name Name of the parameter in the shader
    /// \param x    First component of the value to assign
    /// \param y    Second component of the value to assign
    /// \param z    Third component of the value to assign
    /// \param w    Fourth component of the value to assign
    ///
    ////////////////////////////////////////////////////////////
    void setParameter(const std::string& name, int x, int y, int z, int w) const;

    ////////////////////////////////////////////////////////////
    /// \brief Change a 2-components int vector parameter of the shader
    ///
    /// \a name is the name of the variable to change in the shader.
    /// The corresponding parameter in the shader must be a 2x1 int vector
    /// (ivec2 GLSL type).
    ///
    /// Example:
    /// \code
    /// uniform ivec2 myparam; // this is the variable in the shader
    /// \endcode
    /// \code
    /// shader.setParameter("myparam", sf::Vector2i(5, 6));
    /// \endcode
    ///
    /// \param name   Name of the parameter in the shader
    /// \param vector Vector to assign
    ///
    ////////////////////////////////////////////////////////////
    void setParameter(const std::string& name, const Vector2i& vector) const;

    ////////////////////////////////////////////////////////////
    /// \brief Change a 3-components int vector parameter of the shader
    ///
    /// \a name is the name of the variable to change in the shader.
    /// The corresponding parameter in the shader must be a 3x1 int vector
    /// (ivec3 GLSL type).
    ///
    /// Example:
    /// \code
    /// uniform ivec3 myparam; // this is the variable in the shader
    /// \endcode
    /// \code
    /// shader.setParameter("myparam", sf::Vector3f(5, 6, -8));
    /// \endcode
    ///
    /// \param name   Name of the parameter in the shader
    /// \param vector Vector to assign
    ///
    ////////////////////////////////////////////////////////////
    void setParameter(const std::string& name, const Vector3i& vector) const;

    ////////////////////////////////////////////////////////////
    /// \brief Change a float parameter of the shader
    ///
    /// \a name is the name of the variable to change in the shader.
    /// The corresponding parameter in the shader must be a float
    /// (float GLSL type).
    ///
    /// Example:
    /// \code
    /// uniform float myparam; // this is the variable in the shader
    /// \endcode
    /// \code
    /// shader.setParameter("myparam", 5.2f);
    /// \endcode
    ///
    /// \param name Name of the parameter in the shader
    /// \param x    Value to assign
    ///
    ////////////////////////////////////////////////////////////
    void setParameter(const std::string& name, float x) const;

    ////////////////////////////////////////////////////////////
    /// \brief Change a 2-components vector parameter of the shader
    ///
    /// \a name is the name of the variable to change in the shader.
    /// The corresponding parameter in the shader must be a 2x1 vector
    /// (vec2 GLSL type).
    ///
    /// Example:
    /// \code
    /// uniform vec2 myparam; // this is the variable in the shader
    /// \endcode
    /// \code
    /// shader.setParameter("myparam", 5.2f, 6.0f);
    /// \endcode
    ///
    /// \param name Name of the parameter in the shader
    /// \param x    First component of the value to assign
    /// \param y    Second component of the value to assign
    ///
    ////////////////////////////////////////////////////////////
    void setParameter(const std::string& name, float x, float y) const;

    ////////////////////////////////////////////////////////////
    /// \brief Change a 3-components vector parameter of the shader
    ///
    /// \a name is the name of the variable to change in the shader.
    /// The corresponding parameter in the shader must be a 3x1 vector
    /// (vec3 GLSL type).
    ///
    /// Example:
    /// \code
    /// uniform vec3 myparam; // this is the variable in the shader
    /// \endcode
    /// \code
    /// shader.setParameter("myparam", 5.2f, 6.0f, -8.1f);
    /// \endcode
    ///
    /// \param name Name of the parameter in the shader
    /// \param x    First component of the value to assign
    /// \param y    Second component of the value to assign
    /// \param z    Third component of the value to assign
    ///
    ////////////////////////////////////////////////////////////
    void setParameter(const std::string& name, float x, float y, float z) const;

    ////////////////////////////////////////////////////////////
    /// \brief Change a 4-components vector parameter of the shader
    ///
    /// \a name is the name of the variable to change in the shader.
    /// The corresponding parameter in the shader must be a 4x1 vector
    /// (vec4 GLSL type).
    ///
    /// Example:
    /// \code
    /// uniform vec4 myparam; // this is the variable in the shader
    /// \endcode
    /// \code
    /// shader.setParameter("myparam", 5.2f, 6.0f, -8.1f, 0.4f);
    /// \endcode
    ///
    /// \param name Name of the parameter in the shader
    /// \param x    First component of the value to assign
    /// \param y    Second component of the value to assign
    /// \param z    Third component of the value to assign
    /// \param w    Fourth component of the value to assign
    ///
    ////////////////////////////////////////////////////////////
    void setParameter(const std::string& name, float x, float y, float z, float w) const;

    ////////////////////////////////////////////////////////////
    /// \brief Change a 2-components vector parameter of the shader
    ///
    /// \a name is the name of the variable to change in the shader.
    /// The corresponding parameter in the shader must be a 2x1 vector
    /// (vec2 GLSL type).
    ///
    /// Example:
    /// \code
    /// uniform vec2 myparam; // this is the variable in the shader
    /// \endcode
    /// \code
    /// shader.setParameter("myparam", sf::Vector2f(5.2f, 6.0f));
    /// \endcode
    ///
    /// \param name   Name of the parameter in the shader
    /// \param vector Vector to assign
    ///
    ////////////////////////////////////////////////////////////
    void setParameter(const std::string& name, const Vector2f& vector) const;

    ////////////////////////////////////////////////////////////
    /// \brief Change a 3-components vector parameter of the shader
    ///
    /// \a name is the name of the variable to change in the shader.
    /// The corresponding parameter in the shader must be a 3x1 vector
    /// (vec3 GLSL type).
    ///
    /// Example:
    /// \code
    /// uniform vec3 myparam; // this is the variable in the shader
    /// \endcode
    /// \code
    /// shader.setParameter("myparam", sf::Vector3f(5.2f, 6.0f, -8.1f));
    /// \endcode
    ///
    /// \param name   Name of the parameter in the shader
    /// \param vector Vector to assign
    ///
    ////////////////////////////////////////////////////////////
    void setParameter(const std::string& name, const Vector3f& vector) const;

    ////////////////////////////////////////////////////////////
    /// \brief Change a color parameter of the shader
    ///
    /// \a name is the name of the variable to change in the shader.
    /// The corresponding parameter in the shader must be a 4x1 vector
    /// (vec4 GLSL type).
    ///
    /// It is important to note that the components of the color are
    /// normalized before being passed to the shader. Therefore,
    /// they are converted from range [0 .. 255] to range [0 .. 1].
    /// For example, a sf::Color(255, 125, 0, 255) will be transformed
    /// to a vec4(1.0, 0.5, 0.0, 1.0) in the shader.
    ///
    /// Example:
    /// \code
    /// uniform vec4 color; // this is the variable in the shader
    /// \endcode
    /// \code
    /// shader.setParameter("color", sf::Color(255, 128, 0, 255));
    /// \endcode
    ///
    /// \param name  Name of the parameter in the shader
    /// \param color Color to assign
    ///
    ////////////////////////////////////////////////////////////
    void setParameter(const std::string& name, const Color& color) const;

    ////////////////////////////////////////////////////////////
    /// \brief Change a matrix parameter of the shader
    ///
    /// \a name is the name of the variable to change in the shader.
    /// The corresponding parameter in the shader must be a 4x4 matrix
    /// (mat4 GLSL type).
    ///
    /// Example:
    /// \code
    /// uniform mat4 matrix; // this is the variable in the shader
    /// \endcode
    /// \code
    /// sf::Transform transform;
    /// transform.translate(5, 10);
    /// shader.setParameter("matrix", transform);
    /// \endcode
    ///
    /// \param name      Name of the parameter in the shader
    /// \param transform Transform to assign
    ///
    ////////////////////////////////////////////////////////////
    void setParameter(const std::string& name, const sf::Transform& transform) const;

    ////////////////////////////////////////////////////////////
    /// \brief Change a texture parameter of the shader
    ///
    /// \a name is the name of the variable to change in the shader.
    /// The corresponding parameter in the shader must be a 2D texture
    /// (sampler2D GLSL type).
    ///
    /// Example:
    /// \code
    /// uniform sampler2D the_texture; // this is the variable in the shader
    /// \endcode
    /// \code
    /// sf::Texture texture;
    /// ...
    /// shader.setParameter("the_texture", texture);
    /// \endcode
    /// It is important to note that \a texture must remain alive as long
    /// as the shader uses it, no copy is made internally.
    ///
    /// To use the texture of the object being draw, which cannot be
    /// known in advance, you can pass the special value
    /// sf::Shader::CurrentTexture:
    /// \code
    /// shader.setParameter("the_texture", sf::Shader::CurrentTexture).
    /// \endcode
    ///
    /// \param name    Name of the texture in the shader
    /// \param texture Texture to assign
    ///
    ////////////////////////////////////////////////////////////
    void setParameter(const std::string& name, const Texture& texture) const;

    ////////////////////////////////////////////////////////////
    /// \brief Change a texture parameter of the shader
    ///
    /// This overload maps a shader texture variable to the
    /// texture of the object being drawn, which cannot be
    /// known in advance. The second argument must be
    /// sf::Shader::CurrentTexture.
    /// The corresponding parameter in the shader must be a 2D texture
    /// (sampler2D GLSL type).
    ///
    /// Example:
    /// \code
    /// uniform sampler2D current; // this is the variable in the shader
    /// \endcode
    /// \code
    /// shader.setParameter("current", sf::Shader::CurrentTexture);
    /// \endcode
    ///
    /// \param name Name of the texture in the shader
    ///
    ////////////////////////////////////////////////////////////
    void setParameter(const std::string& name, CurrentTextureType) const;

    ////////////////////////////////////////////////////////////
    /// \brief Bind a VertexBuffer to a uniform block in the shader
    ///
    /// This method maps a VertexBuffer to a uniform block in
    /// the shader. It is assumed the block has layout (std140)
    /// and only occupies a single binding.
    ///
    /// Example:
    /// \code
    /// uniform layout (std140) someBlock // this is the block in the shader
    /// {
    ///     vec3 param1; // Aligned to vec4 because of std140
    ///     vec4 param2;
    /// }; // Make sure block only occupies a single binding (no arrays of blocks)
    /// ...
    /// vec3 someVariable = param1;
    /// vec2 someOtherVariable = param2.xy;
    /// \endcode
    /// \code
    /// sf::VertexBuffer buffer;
    /// ...
    /// // Set buffer data...
    /// ...
    /// shader.setBlock("someBlock", buffer); // Upload
    /// ...
    /// // Set buffer data again...
    /// ...
    /// shader.setBlock("someBlock", buffer); // Upload
    /// \endcode
    ///
    /// \param name   Name of the uniform block in the shader
    /// \param buffer Buffer to bind to the uniform block
    ///
    ////////////////////////////////////////////////////////////
    void setBlock(const std::string& name, const VertexBuffer& buffer) const;

    ////////////////////////////////////////////////////////////
    /// \brief Get the location ID of a shader vertex attribute
    ///
    /// \param name Name of the vertex attribute to search
    ///
    /// \return Location ID of the vertex attribute, or -1 if not found
    ///
    ////////////////////////////////////////////////////////////
    int getVertexAttributeLocation(const std::string& name) const;

    ////////////////////////////////////////////////////////////
    /// \brief Set/Get whether the shader warns about missing variables
    ///
    /// When setting certain parameters is optional, you can use
    /// this method to disable warnings about them not being set
    /// in the shader.
    ///
    /// This will also return the previous warning setting for
    /// easy resetting back to the previous state when required.
    ///
    /// \param warn true to enable warnings about missing variables, false to disable
    ///
    /// \return true if warnings were previously enabled, false if not
    ///
    ////////////////////////////////////////////////////////////
    bool warnMissing(bool warn) const;

    ////////////////////////////////////////////////////////////
    /// \brief Begin setting a parameter block
    ///
    /// When setting a lot of variables at a time on the
    /// same shader, performance can be increased by batching
    /// them together into a parameter block. That way, the
    /// shader does not have to be activated and deactivated
    /// every time a parameter is set which will increase
    /// performance by reducing the number of OpenGL calls.
    ///
    /// When done setting a parameter block, don't forget
    /// to call endParameterBlock.
    ///
    /// \code
    /// sf::Shader shader;
    /// ...
    /// shader.beginParameterBlock();
    /// shader.setParameter("color1", sf::Color(255, 128, 0,   255));
    /// shader.setParameter("color2", sf::Color(255, 255, 128, 255));
    /// shader.setParameter("color3", sf::Color(0,   128, 255, 255));
    /// shader.setParameter("color4", sf::Color(0,   255, 0,   255));
    /// shader.endParameterBlock();
    /// ...
    /// \endcode
    ///
    /// \see endParameterBlock
    ///
    ////////////////////////////////////////////////////////////
    void beginParameterBlock() const;

    ////////////////////////////////////////////////////////////
    /// \brief End setting a parameter block
    ///
    /// \see beginParameterBlock
    ///
    ////////////////////////////////////////////////////////////
    void endParameterBlock() const;

    ////////////////////////////////////////////////////////////
    /// \brief Get the underlying program object identifier
    ///
    /// This returns the identifier of the underlying
    /// OpenGL program object managed by this Shader.
    ///
    /// \return The underlying program object
    ///
    ////////////////////////////////////////////////////////////
    unsigned int getProgramObject() const;

    ////////////////////////////////////////////////////////////
    /// \brief Bind a shader for rendering
    ///
    /// This function is not part of the graphics API, it mustn't be
    /// used when drawing SFML entities. It must be used only if you
    /// mix sf::Shader with OpenGL code.
    ///
    /// \code
    /// sf::Shader s1, s2;
    /// ...
    /// sf::Shader::bind(&s1);
    /// // draw OpenGL stuff that use s1...
    /// sf::Shader::bind(&s2);
    /// // draw OpenGL stuff that use s2...
    /// sf::Shader::bind(NULL);
    /// // draw OpenGL stuff that use no shader...
    /// \endcode
    ///
    /// \param shader Shader to bind, can be null to use no shader
    ///
    ////////////////////////////////////////////////////////////
    static void bind(const Shader* shader);

    ////////////////////////////////////////////////////////////
    /// \brief Tell whether or not the system supports shaders
    ///
    /// This function should always be called before using
    /// the shader features. If it returns false, then
    /// any attempt to use sf::Shader will fail.
    ///
    /// \return True if shaders are supported, false otherwise
    ///
    /// \see getSupportedVersion, isGeometryShaderAvailable, isUniformBufferAvailable
    ///
    ////////////////////////////////////////////////////////////
    static bool isAvailable();

    ////////////////////////////////////////////////////////////
    /// \brief Tell whether or not the system supports geometry shaders
    ///
    /// This function should always be called before trying
    /// to load a geometry shader. If it returns false, then
    /// any attempt to load a geometry shader will fail.
    ///
    /// This will check for \e core support of geometry shaders,
    /// \e not ARB or EXT support. As such, it can only return
    /// true if OpenGL 3.2 or later is supported. This does
    /// \e not mean you need to have a version 3.2 or later
    /// context to use geometry shaders. 3.2 merely has to
    /// be \e supported by the driver/hardware.
    ///
    /// The non-core functionality isn't exposed through the
    /// Shader API. All required information such as input
    /// primitive, output primitive and vertex count need
    /// to be specified using the layout() GLSL syntax so that
    /// a Shader object, when compiled, is self-contained.
    ///
    /// \return True if geometry shaders are supported, false otherwise
    ///
    /// \see isAvailable, getSupportedVersion
    ///
    ////////////////////////////////////////////////////////////
    static bool isGeometryShaderAvailable();

    ////////////////////////////////////////////////////////////
    /// \brief Tell whether or not the system supports uniform buffers
    ///
    /// Uniform buffers are simple buffer objects, such as those
    /// used in VertexBuffer. Uniform buffers however, store
    /// shader uniform data instead of vertex data. They can
    /// be used to upload large amounts of uniform data
    /// with a single API call and that data can be shared
    /// among multiple shaders as well.
    ///
    /// Because uniform buffers rely on shaders and buffer
    /// objects being available, this will only return true
    /// if Shader::isAvailable() and VertexBuffer::isAvailable()
    /// both return true and uniform buffer support itself
    /// is present.
    ///
    /// \return True if uniform buffers are supported, false otherwise
    ///
    /// \see isAvailable
    ///
    ////////////////////////////////////////////////////////////
    static bool isUniformBufferAvailable();

    ////////////////////////////////////////////////////////////
    /// \brief Get the string identifying the supported GLSL version
    ///
    /// In the desktop implementation (not ES), the string returned
    /// is guaranteed to begin with the version number. In the ES
    /// implementation, the returned string is prefixed with "ES ".
    ///
    /// \return std::string containing the supported GLSL version or an empty string if unsupported
    ///
    /// \see isAvailable
    ///
    ////////////////////////////////////////////////////////////
    static std::string getSupportedVersion();

    ////////////////////////////////////////////////////////////
    /// \brief Get the maximum number of uniform components supported
    ///
    /// A uniform component is typically a single 32-bit data type
    /// such as float or int. Vectors of those data types take up
    /// the corresponding multiple of space, i.e. vec4 takes up
    /// 4 floats worth of space and counts as 4 components. To
    /// guarantee alignment, vec3 might also be packed to take up
    /// the same amount of space as a vec4.
    ///
    /// \return Maximum number of uniform components supported
    ///
    ////////////////////////////////////////////////////////////
    static unsigned int getMaximumUniformComponents();

private :

    friend class RenderTarget;

    ////////////////////////////////////////////////////////////
    /// \brief Compile the shader(s) and create the program
    ///
    /// If one of the arguments is NULL, the corresponding shader
    /// is not created.
    ///
    /// \param vertexShaderCode   Source code of the vertex shader
    /// \param fragmentShaderCode Source code of the fragment shader
    /// \param geometryShaderCode Source code of the geometry shader
    ///
    /// \return True on success, false if any error happened
    ///
    ////////////////////////////////////////////////////////////
    bool compile(const char* vertexShaderCode, const char* fragmentShaderCode, const char* geometryShaderCode);

    ////////////////////////////////////////////////////////////
    /// \brief Bind all the textures used by the shader
    ///
    /// This function each texture to a different unit, and
    /// updates the corresponding variables in the shader accordingly.
    ///
    ////////////////////////////////////////////////////////////
    void bindTextures() const;

    ////////////////////////////////////////////////////////////
    /// \brief Get the location ID of a shader parameter
    ///
    /// \param name Name of the parameter to search
    ///
    /// \return Location ID of the parameter, or -1 if not found
    ///
    ////////////////////////////////////////////////////////////
    int getParamLocation(const std::string& name) const;

    ////////////////////////////////////////////////////////////
    /// \brief Get the binding ID of a shader uniform block
    ///
    /// \param name Name of the uniform block to search
    ///
    /// \return Binding ID of the uniform block, or -1 if not found
    ///
    ////////////////////////////////////////////////////////////
    int getBlockBinding(const std::string& name) const;

    ////////////////////////////////////////////////////////////
    // Types
    ////////////////////////////////////////////////////////////
    typedef std::map<int, const Texture*> TextureTable;
    typedef std::map<std::string, int> LocationTable;
    typedef std::map<std::string, unsigned int> BufferTable;

    ////////////////////////////////////////////////////////////
    // Member data
    ////////////////////////////////////////////////////////////
    unsigned int          m_shaderProgram;  ///< OpenGL identifier for the program
    mutable int           m_currentTexture; ///< Location of the current texture in the shader
    mutable TextureTable  m_textures;       ///< Texture variables in the shader, mapped to their location
    mutable LocationTable m_params;         ///< Parameters location cache
    mutable LocationTable m_attributes;     ///< Attributes location cache
    mutable LocationTable m_blockBindings;  ///< Block binding cache
    mutable BufferTable   m_boundBuffers;   ///< Buffers bound to this shader
    mutable bool          m_warnMissing;    ///< Whether to warn the user that variables could not be found.
    Uint64                m_id;             ///< Unique number that identifies the compiled and linked program
    mutable bool          m_parameterBlock; ///< Whether we are in a parameter block
    mutable unsigned int  m_blockProgram;   ///< The program to restore after a parameter block
};

} // namespace sf


#endif // SFML_SHADER_HPP


////////////////////////////////////////////////////////////
/// \class sf::Shader
/// \ingroup graphics
///
/// Shaders are programs written using a specific language,
/// executed directly by the graphics card and allowing
/// to apply real-time operations to the rendered entities.
///
/// There are two kinds of shaders:
/// \li Vertex shaders, that process vertices
/// \li Fragment (pixel) shaders, that process pixels
///
/// A sf::Shader can be composed of either a vertex shader
/// alone, a fragment shader alone, or both combined
/// (see the variants of the load functions). When rendering
/// using the non-legacy OpenGL pipeline, both are required
/// to ensure proper behaviour.
///
/// Shaders are written in GLSL, which is a C-like
/// language dedicated to OpenGL shaders. You'll probably
/// need to learn its basics before writing your own shaders
/// for SFML.
///
/// Like any C/C++ program, a shader has its own variables
/// that you can set from your C++ application. sf::Shader
/// handles 6 different types of variables:
/// \li ints
/// \li floats
/// \li vectors (2, 3 or 4 components)
/// \li colors
/// \li textures
/// \li transforms (matrices)
///
/// The value of the variables can be changed at any time
/// with the various overloads of the setParameter function:
/// \code
/// shader.setParameter("offset", 3);
/// shader.setParameter("offset", 2.f);
/// shader.setParameter("point", 0.5f, 0.8f, 0.3f);
/// shader.setParameter("color", sf::Color(128, 50, 255));
/// shader.setParameter("matrix", transform); // transform is a sf::Transform
/// shader.setParameter("overlay", texture); // texture is a sf::Texture
/// shader.setParameter("texture", sf::Shader::CurrentTexture);
/// \endcode
///
/// When rendering using the legacy pipeline, the special
/// Shader::CurrentTexture argument maps the given texture
/// variable to the current texture of the object being
/// drawn (which cannot be known in advance).
///
/// When SFML selects to use the non-legacy pipeline
/// implementation to render, shaders can make use of special
/// uniforms and attributes which are provided by the SFML
/// drawable implementation. Many of them are direct replacements
/// for deprecated/removed built-in shader variables.
///
/// Here is a list of the built-in SFML shader uniforms:
/// \li uniform mat4 sf_ModelMatrix, the current model matrix
/// \li uniform mat4 sf_ViewMatrix, the current view matrix
/// \li uniform mat4 sf_ProjectionMatrix, the current projection matrix
/// \li uniform mat4 sf_NormalMatrix, the current normal matrix
/// \li uniform mat4 sf_TextureMatrix, the current texture matrix
/// \li uniform int sf_TextureEnabled, set to 1 when texturing is requested, 0 otherwise
/// \li uniform sampler2D sf_Texture0, the bound 2D texture at the time of rendering
/// \li uniform vec3 sf_ViewerPosition, the position of the view/camera in world space
/// \li uniform int sf_LightingEnabled, whether lighting is enabled, will be 1 when enabled and 0 when disabled
/// \li uniform int sf_LightCount, the number of lights currently enabled
/// \li uniform Light sf_Lights[], uniform array of lights (values only set up to sf_Lights[sf_LightCount - 1])
///
/// The light structure:
/// \code
/// struct Light\n"
/// {
///     vec4 ambientColor;
///     vec4 diffuseColor;
///     vec4 specularColor;
///     vec4 positionDirection;
///     vec4 attenuation;
/// };
/// \endcode
///
/// Whether a light is positional or directional can be
/// determined through the w coordinate of positionDirection.
/// If w is 1.0, the light is positional, if w is 0.0 it is
/// directional.
///
/// The attenuation vec4 is applied as follows:
/// \li x is constant attenuation
/// \li y is linear attenuation
/// \li z is quadratic attenuation
/// \li w is unused
///
/// Here is a list of the built-in SFML shader vertex attributes:
/// \li in vec3 sf_Vertex, the position of the current vertex
/// \li in vec4 sf_Color, the color of the current vertex
/// \li in vec2 sf_MultiTexCoord0, the texture coordinate of the current vertex (normalize with sf_TextureMatrix)
/// \li in vec3 sf_Normal, the normal of the current vertex
///
/// The default shaders that are provided when rendering
/// using the non-legacy pipeline takes care of per-pixel
/// lighting and texturing for you. If you want to write
/// your own shaders, don't forget that you have to reimplement
/// these yourself if you require the corresponding functionality.
///
/// To apply a shader to a drawable, you must pass it as an
/// additional parameter to the Draw function:
/// \code
/// window.draw(sprite, &shader);
/// \endcode
///
/// ... which is in fact just a shortcut for this:
/// \code
/// sf::RenderStates states;
/// states.shader = &shader;
/// window.draw(sprite, states);
/// \endcode
///
/// In the code above we pass a pointer to the shader, because it may
/// be null (which means "no shader").
///
/// Shaders can be used on any drawable, but some combinations are
/// not interesting. For example, using a vertex shader on a sf::Sprite
/// is limited because there are only 4 vertices, the sprite would
/// have to be subdivided in order to apply wave effects.
/// Another bad example is a fragment shader with sf::Text: the texture
/// of the text is not the actual text that you see on screen, it is
/// a big texture containing all the characters of the font in an
/// arbitrary order; thus, texture lookups on pixels other than the
/// current one may not give you the expected result.
///
/// Shaders can also be used to apply global post-effects to the
/// current contents of the target (like the old sf::PostFx class
/// in SFML 1). This can be done in two different ways:
/// \li draw everything to a sf::RenderTexture, then draw it to
///     the main target using the shader
/// \li draw everything directly to the main target, then use
///     sf::Texture::update(Window&) to copy its contents to a texture
///     and draw it to the main target using the shader
///
/// The first technique is more optimized because it doesn't involve
/// retrieving the target's pixels to system memory, but the
/// second one doesn't impact the rendering process and can be
/// easily inserted anywhere without impacting all the code.
///
/// Like sf::Texture that can be used as a raw OpenGL texture,
/// sf::Shader can also be used directly as a raw shader for
/// custom OpenGL geometry.
/// \code
/// sf::Shader::bind(&shader);
/// ... render OpenGL geometry ...
/// sf::Shader::bind(NULL);
/// \endcode
///
////////////////////////////////////////////////////////////


#ifndef SFML_CONVEXPOLYHEDRON_HPP
#define SFML_CONVEXPOLYHEDRON_HPP

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Graphics/Export.hpp>
#include <SFML/Graphics/Polyhedron.hpp>
#include <vector>


namespace sf
{
////////////////////////////////////////////////////////////
/// \brief Specialized polyhedron representing a convex polyhedron
///
////////////////////////////////////////////////////////////
class SFML_GRAPHICS_API ConvexPolyhedron : public Polyhedron
{
public :

    ////////////////////////////////////////////////////////////
    /// \brief Default constructor
    ///
    /// \param faceCount Number of faces of the polyhedron
    ///
    ////////////////////////////////////////////////////////////
    explicit ConvexPolyhedron(unsigned int faceCount = 0);

    ////////////////////////////////////////////////////////////
    /// \brief Set the number of faces of the polyhedron
    ///
    /// \param count New number of faces of the polyhedron
    ///
    /// \see getFaceCount
    ///
    ////////////////////////////////////////////////////////////
    void setFaceCount(unsigned int count);

    ////////////////////////////////////////////////////////////
    /// \brief Get the number of faces of the polyhedron
    ///
    /// \return Number of faces of the polyhedron
    ///
    /// \see setFaceCount
    ///
    ////////////////////////////////////////////////////////////
    virtual unsigned int getFaceCount() const;

    ////////////////////////////////////////////////////////////
    /// \brief Set the vertices of a face
    ///
    /// Don't forget that the face vertices must be specified
    /// with counter-clockwise winding or else it will be culled!
    /// setFaceCount must be called first in order to set the total
    /// number of faces. The result is undefined if \a index is out
    /// of the valid range.
    ///
    /// \param index Index of the face to change, in range [0 .. getFaceCount() - 1]
    /// \param v0    First vertex of the face
    /// \param v1    Second vertex of the face
    /// \param v2    Third vertex of the face
    ///
    /// \see getFace
    ///
    ////////////////////////////////////////////////////////////
    void setFace(unsigned int index, const Vertex& v0, const Vertex& v1, const Vertex& v2);

    ////////////////////////////////////////////////////////////
    /// \brief Get a face
    ///
    /// The result is undefined if \a index is out of the valid range.
    ///
    /// \param index Index of the face to get, in range [0 .. getFaceCount() - 1]
    ///
    /// \return Index-th Face of the polyhedron
    ///
    /// \see setFace
    ///
    ////////////////////////////////////////////////////////////
    virtual Face getFace(unsigned int index) const;

private :

    ////////////////////////////////////////////////////////////
    // Member data
    ////////////////////////////////////////////////////////////
    std::vector<Vertex> m_vertices; ///< Vertices composing the convex polyhedron
};

} // namespace sf


#endif // SFML_CONVEXPOLYHEDRON_HPP


////////////////////////////////////////////////////////////
/// \class sf::ConvexPolyhedron
/// \ingroup graphics
///
/// This class inherits all the functions of sf::Transformable
/// (position, rotation, scale, bounds, ...) as well as the
/// functions of sf::Polyhedron (color, texture, ...).
///
/// It is important to keep in mind that while specifying faces,
/// faces with clockwise winding (vertices specified in clockwise
/// order from the perspective of the viewer) are culled by default.
/// If you want a face to be facing the "outside" of the polyhedron,
/// specify its vertices in counter-clockwise order.
///
/// If you want to light your scene, you will either need to
/// specify the vertex normal data yourself, or if you want to
/// automatically generate per-face normals you can call
/// generateNormals() after you are done specifying the faces.
///
/// Usage example:
/// \code
/// sf::ConvexPolyhedron polyhedron;
/// polyhedron.setFaceCount(4);
/// polyhedron.setFace(0, sf::Vector3f(0, -10, -10), sf::Vector3f(10, -10, 10), sf::Vector3f(-10, -10, 10));
/// polyhedron.setFace(1, sf::Vector3f(10, -10, 10), sf::Vector3f(0, -10, -10), sf::Vector3f(0, 10, 0));
/// polyhedron.setFace(2, sf::Vector3f(-10, -10, 10), sf::Vector3f(10, -10, 10), sf::Vector3f(0, 10, 0));
/// polyhedron.setFace(3, sf::Vector3f(0, -10, -10), sf::Vector3f(-10, -10, 10), sf::Vector3f(0, 10, 0));
/// polyhedron.generateNormals();
/// polyhedron.setColor(sf::Color::Red);
/// polyhedron.setPosition(10, 20, 30);
/// ...
/// window.draw(polyhedron);
/// \endcode
///
/// \see sf::Polyhedron, sf::Cuboid, sf::SphericalPolyhedron
///
////////////////////////////////////////////////////////////

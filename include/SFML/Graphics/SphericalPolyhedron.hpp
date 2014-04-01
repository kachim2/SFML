
#ifndef SFML_SPHERICALPOLYHEDRON_HPP
#define SFML_SPHERICALPOLYHEDRON_HPP

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Graphics/Export.hpp>
#include <SFML/Graphics/Polyhedron.hpp>
#include <vector>


namespace sf
{
////////////////////////////////////////////////////////////
/// \brief Specialized polyhedron representing a spherical polyhedron
///
////////////////////////////////////////////////////////////
class SFML_GRAPHICS_API SphericalPolyhedron : public Polyhedron
{
public :

    ////////////////////////////////////////////////////////////
    /// \brief Default constructor
    ///
    /// Creates a spherical polyhedron positioned at (0, 0, 0)
    /// with radius 0 and 5 subdivisions.
    ///
    /// \param radius       Radius of the spherical polyhedron
    /// \param subdivisions Number of times to subdivide faces of the base geometry
    ///
    ////////////////////////////////////////////////////////////
    explicit SphericalPolyhedron(float radius = 0, unsigned int subdivisions = 5);

    ////////////////////////////////////////////////////////////
    /// \brief Set the radius of the spherical polyhedron
    ///
    /// \param radius New radius of the spherical polyhedron
    ///
    /// \see getRadius
    ///
    ////////////////////////////////////////////////////////////
    void setRadius(float radius);

    ////////////////////////////////////////////////////////////
    /// \brief Get the radius of the spherical polyhedron
    ///
    /// \return Radius of the spherical polyhedron
    ///
    /// \see setRadius
    ///
    ////////////////////////////////////////////////////////////
    float getRadius() const;

    ////////////////////////////////////////////////////////////
    /// \brief Set the number of times the base polyhedron is subdivided
    ///
    /// \param subdivisions Number of times the base polyhedron is subdivided
    ///
    /// \see getSubdivisions
    ///
    ////////////////////////////////////////////////////////////
    void setSubdivisions(unsigned int subdivisions);

    ////////////////////////////////////////////////////////////
    /// \brief Get the number of times the base polyhedron is subdivided
    ///
    /// \return The number of times the base polyhedron is subdivided
    ///
    /// \see setSubdivisions
    ///
    ////////////////////////////////////////////////////////////
    unsigned int getSubdivisions() const;

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
    /// \brief Get a face of the polyhedron
    ///
    /// The result is undefined if \a index is out of the valid range.
    ///
    /// \param index Index of the face to get, in range [0 .. getFaceCount() - 1]
    ///
    /// \return Index-th face of the polyhedron
    ///
    ////////////////////////////////////////////////////////////
    virtual Face getFace(unsigned int index) const;

private :

    ////////////////////////////////////////////////////////////
    /// \brief Construct the geometry from the base polyhedron and given subdivisions
    ///
    ////////////////////////////////////////////////////////////
    void construct() const;

    ////////////////////////////////////////////////////////////
    /// \brief Subdivide a face of the current geometry
    ///
    /// \param a First corner position
    /// \param b Second corner position
    /// \param c Third corner position
    /// \param s Current subdivision
    ///
    ////////////////////////////////////////////////////////////
    void subdivide(const Vector3f& a, const Vector3f& b, const Vector3f& c, unsigned int s) const;

    ////////////////////////////////////////////////////////////
    // Member data
    ////////////////////////////////////////////////////////////
    float        m_radius;       ///< Radius of the spherical polyhedron
    unsigned int m_subdivisions; ///< Radius of the spherical polyhedron

    mutable std::vector<Vertex> m_geometry; ///< Constructed geometry
};

} // namespace sf


#endif // SFML_SPHERICALPOLYHEDRON_HPP


////////////////////////////////////////////////////////////
/// \class sf::SphericalPolyhedron
/// \ingroup graphics
///
/// This class inherits all the functions of sf::Transformable
/// (position, rotation, scale, bounds, ...) as well as the
/// functions of sf::Polyhedron (color, texture, ...).
///
/// Usage example:
/// \code
/// sf::SphericalPolyhedron sphere;
/// sphere.setRadius(150);
/// sphere.setColor(sf::Color::Red);
/// sphere.setPosition(10, 20, 30);
/// ...
/// window.draw(sphere);
/// \endcode
///
/// Since the graphics card can't draw perfect spheres, we have to
/// fake them through tessellation of a base icosahedron. The
/// "subdivisions" property of sf::SphericalPolyhedron defines how many
/// subdivisions to perform on the faces of the base primitive,
/// and therefore defines the quality of the sphere.
///
/// \see sf::Polyhedron, sf::Cuboid, sf::ConvexPolyhedron
///
////////////////////////////////////////////////////////////

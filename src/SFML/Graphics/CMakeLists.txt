
set(INCROOT ${PROJECT_SOURCE_DIR}/include/SFML/Graphics)
set(SRCROOT ${PROJECT_SOURCE_DIR}/src/SFML/Graphics)

# all source files
set(SRC
    ${SRCROOT}/BlendMode.cpp
    ${INCROOT}/BlendMode.hpp
    ${SRCROOT}/Color.cpp
    ${INCROOT}/Color.hpp
    ${INCROOT}/Export.hpp
    ${SRCROOT}/Font.cpp
    ${INCROOT}/Font.hpp
    ${INCROOT}/Glyph.hpp
    ${SRCROOT}/GLCheck.cpp
    ${SRCROOT}/GLCheck.hpp
    ${SRCROOT}/GLExtensions.hpp
    ${SRCROOT}/GLExtensions.cpp
    ${SRCROOT}/Image.cpp
    ${INCROOT}/Image.hpp
    ${SRCROOT}/ImageLoader.cpp
    ${SRCROOT}/ImageLoader.hpp
    ${INCROOT}/PrimitiveType.hpp
    ${INCROOT}/Rect.hpp
    ${INCROOT}/Rect.inl
    ${SRCROOT}/RenderStates.cpp
    ${INCROOT}/RenderStates.hpp
    ${SRCROOT}/RenderTexture.cpp
    ${INCROOT}/RenderTexture.hpp
    ${SRCROOT}/RenderTarget.cpp
    ${INCROOT}/RenderTarget.hpp
    ${SRCROOT}/RenderWindow.cpp
    ${INCROOT}/RenderWindow.hpp
    ${SRCROOT}/Shader.cpp
    ${INCROOT}/Shader.hpp
    ${SRCROOT}/Texture.cpp
    ${INCROOT}/Texture.hpp
    ${SRCROOT}/TextureSaver.cpp
    ${SRCROOT}/TextureSaver.hpp
    ${SRCROOT}/Transform.cpp
    ${INCROOT}/Transform.hpp
    ${SRCROOT}/Transformable.cpp
    ${INCROOT}/Transformable.hpp
    ${SRCROOT}/View.cpp
    ${INCROOT}/View.hpp
    ${SRCROOT}/Vertex.cpp
    ${INCROOT}/Vertex.hpp
)
if(NOT SFML_OPENGL_ES)
    list(APPEND SRC ${SRCROOT}/GLLoader.cpp)
    list(APPEND SRC ${SRCROOT}/GLLoader.hpp)
endif()
source_group("" FILES ${SRC})

# drawables sources
set(DRAWABLES_SRC
    ${INCROOT}/Drawable.hpp
    ${SRCROOT}/Shape.cpp
    ${INCROOT}/Shape.hpp
    ${SRCROOT}/CircleShape.cpp
    ${INCROOT}/CircleShape.hpp
    ${SRCROOT}/RectangleShape.cpp
    ${INCROOT}/RectangleShape.hpp
    ${SRCROOT}/ConvexShape.cpp
    ${INCROOT}/ConvexShape.hpp
    ${SRCROOT}/Sprite.cpp
    ${INCROOT}/Sprite.hpp
    ${SRCROOT}/Text.cpp
    ${INCROOT}/Text.hpp
    ${SRCROOT}/VertexArray.cpp
    ${INCROOT}/VertexArray.hpp
)
source_group("drawables" FILES ${DRAWABLES_SRC})

# render-texture sources
set(RENDER_TEXTURE_SRC
    ${SRCROOT}/RenderTextureImpl.cpp
    ${SRCROOT}/RenderTextureImpl.hpp
    ${SRCROOT}/RenderTextureImplFBO.cpp
    ${SRCROOT}/RenderTextureImplFBO.hpp
    ${SRCROOT}/RenderTextureImplDefault.cpp
    ${SRCROOT}/RenderTextureImplDefault.hpp
)
source_group("render texture" FILES ${RENDER_TEXTURE_SRC})

# render-target sources
set(RENDER_TARGET_SRC
    ${SRCROOT}/RenderTargetImpl.cpp
    ${SRCROOT}/RenderTargetImpl.hpp
    ${SRCROOT}/RenderTargetImplVBO.cpp
    ${SRCROOT}/RenderTargetImplVBO.hpp
)

if(NOT SFML_OS_EMSCRIPTEN)
    set(RENDER_TARGET_SRC
        ${RENDER_TARGET_SRC}
        ${SRCROOT}/RenderTargetImplDefault.cpp
        ${SRCROOT}/RenderTargetImplDefault.hpp
    )
endif()
source_group("render target" FILES ${RENDER_TARGET_SRC})

# stb_image sources
include_directories("${PROJECT_SOURCE_DIR}/extlibs/headers/stb_image")

# let CMake know about our additional graphics libraries paths
if(SFML_OS_WINDOWS)
    set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "${PROJECT_SOURCE_DIR}/extlibs/headers/jpeg")
    set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "${PROJECT_SOURCE_DIR}/extlibs/headers/freetype2")
elseif(SFML_OS_MACOSX)
    set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "${PROJECT_SOURCE_DIR}/extlibs/headers/jpeg")
    set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "${PROJECT_SOURCE_DIR}/extlibs/headers/freetype2")
    set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "${PROJECT_SOURCE_DIR}/extlibs/libs-osx/Frameworks")
elseif(SFML_OS_IOS)
    set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "${PROJECT_SOURCE_DIR}/extlibs/headers/jpeg")
    set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "${PROJECT_SOURCE_DIR}/extlibs/headers/freetype2")
elseif(SFML_OS_ANDROID)
    set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "${PROJECT_SOURCE_DIR}/extlibs/headers/jpeg")
    set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "${PROJECT_SOURCE_DIR}/extlibs/headers/freetype2")
endif()

# find external libraries
if(NOT SFML_OPENGL_ES)
    find_package(OpenGL REQUIRED)
    if(SFML_OS_LINUX)
        find_package(X11 REQUIRED)
    endif()
    include_directories(${FREETYPE_INCLUDE_DIRS} ${JPEG_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR})
endif()
if(SFML_OPENGL_ES AND SFML_OS_LINUX)
    find_package(EGL REQUIRED)
    find_package(GLES REQUIRED)
    include_directories(${EGL_INCLUDE_DIR} ${GLES_INCLUDE_DIR})
endif()
if(SFML_OS_ANDROID)
    find_host_package(JPEG REQUIRED)
    find_host_package(Freetype REQUIRED)
elseif(SFML_OS_EMSCRIPTEN)
    set(JPEG_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/extlibs/headers/jpeg")
    set(FREETYPE_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/extlibs/headers/freetype2")

    file(GLOB FREETYPE_PRIVATE_HEADERS "${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libfreetype/include/internal/*.h")

    set(EXTLIBS_SRC
        ${FREETYPE_PRIVATE_HEADERS}
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libfreetype/autofit/autofit.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libfreetype/base/ftadvanc.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libfreetype/base/ftbbox.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libfreetype/base/ftbdf.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libfreetype/base/ftbitmap.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libfreetype/base/ftcalc.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libfreetype/base/ftcid.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libfreetype/base/ftdbgmem.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libfreetype/base/ftdebug.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libfreetype/base/ftfstype.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libfreetype/base/ftgasp.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libfreetype/base/ftgloadr.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libfreetype/base/ftglyph.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libfreetype/base/ftgxval.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libfreetype/base/ftinit.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libfreetype/base/ftlcdfil.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libfreetype/base/ftmm.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libfreetype/base/ftobjs.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libfreetype/base/ftotval.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libfreetype/base/ftoutln.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libfreetype/base/ftpatent.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libfreetype/base/ftpfr.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libfreetype/base/ftrfork.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libfreetype/base/ftsnames.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libfreetype/base/ftstream.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libfreetype/base/ftstroke.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libfreetype/base/ftsynth.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libfreetype/base/ftsystem.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libfreetype/base/fttrigon.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libfreetype/base/fttype1.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libfreetype/base/ftutil.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libfreetype/base/ftwinfnt.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libfreetype/base/ftxf86.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libfreetype/bdf/bdf.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libfreetype/bzip2/ftbzip2.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libfreetype/cache/ftcache.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libfreetype/cff/cff.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libfreetype/cid/type1cid.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libfreetype/gzip/ftgzip.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libfreetype/lzw/ftlzw.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libfreetype/pcf/pcf.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libfreetype/pfr/pfr.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libfreetype/psaux/psaux.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libfreetype/pshinter/pshinter.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libfreetype/psnames/psmodule.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libfreetype/raster/raster.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libfreetype/sfnt/sfnt.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libfreetype/smooth/smooth.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libfreetype/truetype/truetype.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libfreetype/type1/type1.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libfreetype/type42/type42.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libfreetype/winfonts/winfnt.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libjpeg/jaricom.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libjpeg/jcapimin.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libjpeg/jcapistd.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libjpeg/jcarith.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libjpeg/jccoefct.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libjpeg/jccolor.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libjpeg/jcdctmgr.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libjpeg/jchuff.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libjpeg/jcinit.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libjpeg/jcmainct.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libjpeg/jcmarker.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libjpeg/jcmaster.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libjpeg/jcomapi.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libjpeg/jcparam.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libjpeg/jcprepct.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libjpeg/jcsample.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libjpeg/jctrans.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libjpeg/jdapimin.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libjpeg/jdapistd.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libjpeg/jdarith.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libjpeg/jdatadst.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libjpeg/jdatasrc.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libjpeg/jdcoefct.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libjpeg/jdcolor.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libjpeg/jddctmgr.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libjpeg/jdhuff.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libjpeg/jdinput.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libjpeg/jdmainct.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libjpeg/jdmarker.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libjpeg/jdmaster.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libjpeg/jdmerge.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libjpeg/jdpostct.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libjpeg/jdsample.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libjpeg/jdtrans.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libjpeg/jerror.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libjpeg/jfdctflt.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libjpeg/jfdctfst.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libjpeg/jfdctint.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libjpeg/jidctflt.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libjpeg/jidctfst.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libjpeg/jidctint.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libjpeg/jmemmgr.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libjpeg/jmemnobs.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libjpeg/jquant1.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libjpeg/jquant2.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libjpeg/jutils.c
    )
    source_group("extlibs" FILES ${EXTLIBS_SRC})

    include_directories("${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libfreetype/truetype")
    include_directories("${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libfreetype/sfnt")
    include_directories("${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libfreetype/autofit")
    include_directories("${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libfreetype/smooth")
    include_directories("${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libfreetype/raster")
    include_directories("${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libfreetype/psaux")
    include_directories("${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libfreetype/psnames")
    include_directories("${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libfreetype/include")
    include_directories("${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libjpeg")

    add_definitions(-DFT2_BUILD_LIBRARY)

else()
    find_package(JPEG REQUIRED)
    find_package(Freetype REQUIRED)
endif()
include_directories(${FREETYPE_INCLUDE_DIRS} ${JPEG_INCLUDE_DIR})

# build the list of external libraries to link
if(NOT SFML_OPENGL_ES)
    list(APPEND GRAPHICS_EXT_LIBS ${OPENGL_gl_LIBRARY})
    if(SFML_OS_LINUX)
        list(APPEND GRAPHICS_EXT_LIBS ${X11_LIBRARIES})
    endif()
endif()
if(SFML_OPENGL_ES AND SFML_OS_LINUX)
    list(APPEND GRAPHICS_EXT_LIBS ${EGL_LIBRARY} ${GLES_LIBRARY})
endif()
if(SFML_OS_IOS)
    list(APPEND GRAPHICS_EXT_LIBS "-framework OpenGLES")
elseif(SFML_OS_ANDROID)
    list(APPEND GRAPHICS_EXT_LIBS z)
endif()
list(APPEND GRAPHICS_EXT_LIBS ${FREETYPE_LIBRARY} ${JPEG_LIBRARY})

# add preprocessor symbols
add_definitions(-DSTBI_FAILURE_USERMSG)

# ImageLoader.cpp must be compiled with the -fno-strict-aliasing
# when gcc is used; otherwise saving PNGs may crash in stb_image_write
if(SFML_COMPILER_GCC)
    set_source_files_properties(${SRCROOT}/ImageLoader.cpp PROPERTIES COMPILE_FLAGS -fno-strict-aliasing)
endif()

# define the sfml-graphics target
sfml_add_library(sfml-graphics
                 SOURCES ${SRC} ${DRAWABLES_SRC} ${RENDER_TEXTURE_SRC} ${RENDER_TARGET_SRC} ${STB_SRC} ${EXTLIBS_SRC}
                 DEPENDS sfml-window sfml-system
                 EXTERNAL_LIBS ${GRAPHICS_EXT_LIBS})

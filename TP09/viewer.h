#ifndef VIEWER_H
#define VIEWER_H

// GLEW lib: needs to be included first!!
#include <GL/glew.h>

// OpenGL library
#include <GL/gl.h>

// OpenGL Utility library
#include <GL/glu.h>

// OpenGL Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <QGLFormat>
#include <QGLWidget>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QTimer>
#include <stack>

#include "camera.h"
#include "shader.h"
#include "grid.h"

class Viewer : public QGLWidget {
 public:
  Viewer(const QGLFormat &format=QGLFormat::defaultFormat());
  ~Viewer();

 protected :
  virtual void paintGL();
  virtual void initializeGL();
  virtual void resizeGL(int width,int height);
  virtual void keyPressEvent(QKeyEvent *ke);
  virtual void mousePressEvent(QMouseEvent *me);
  virtual void mouseMoveEvent(QMouseEvent *me);

 private:
  void createVAO();
  void deleteVAO();
  void drawVAO();

  void createShaders();
  void enableShader(unsigned int shader=0);
  void enablePerlinShader();
  void enableNormalShader();
  void enableThirdShader();
  void enableForthShader();
  void disableShader();

  void drawObject(const glm::vec3 &pos,const glm::vec3 &col);

  void createFBO();
  void deleteFBO();
  void initFBO();

  void createTextures();
  void loadTexture(GLuint id,const char *filename);
  void deleteTextures();

  QTimer        *_timer;    // timer that controls the animation
  unsigned int   _currentshader; // current shader index

  Camera *_cam;    // the camera
  Grid *_grid;
  glm::vec3 _light; // light direction
  bool      _mode;  // camera motion or light motion

  std::vector<std::string> _vertexFilenames;   // all vertex filenames
  std::vector<std::string> _fragmentFilenames; // all fragment filenames
  std::vector<Shader *>    _shaders;           // all the shaders

  //GLuint _vao;
  GLuint _buffers[1];
  GLuint _vaoTerrain;
  GLuint _vaoQuad;
  GLuint _terrain[2];
  GLuint _quad;

  GLuint _snowTextu;
  GLuint _montagneTextu;
  GLuint _grassTextu;

  // render texture ids
  GLuint _noisePerlinId;
  GLuint _normalId;
  GLuint _rendDepthId;
  GLuint _rendColorId;
  // fbo id
  GLuint _fbo;
  GLuint _fbo2;
};

#endif // VIEWER_H

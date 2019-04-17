#include "viewer.h"

#include <math.h>
#include <iostream>
#include <QTime>

using namespace std;


Viewer::Viewer(const QGLFormat &format)
  : QGLWidget(format),
    _grid(new Grid()),
    _timer(new QTimer(this)),
    _currentshader(0),
    _light(glm::vec3(0,0,1)),
    _mode(false) {

  setlocale(LC_ALL,"C");

  // create a camera (automatically modify model/view matrices according to user interactions)
  _cam  = new Camera(15,glm::vec3(0.0f, 0.7f, 0.5f));
  _timer->setInterval(10);
  connect(_timer,SIGNAL(timeout()),this,SLOT(updateGL()));
}

Viewer::~Viewer() {
  delete _timer;
  delete _cam;
  delete _grid;
  for(unsigned int i=0;i<_shaders.size();++i) {
    delete _shaders[i];
  }

  deleteVAO();
  deleteFBO();
  deleteTextures();
}

void Viewer::createVAO() {
  //the variable _grid should be an instance of Grid

  const GLfloat quadData[] = {
    -1.0f,-1.0f,0.0f, 1.0f,-1.0f,0.0f, -1.0f,1.0f,0.0f, -1.0f,1.0f,0.0f, 1.0f,-1.0f,0.0f, 1.0f,1.0f,0.0f };

  glGenBuffers(2,_terrain);//generate two buffers objects names
  glGenBuffers(1,&_quad);//generate one buffer object name
  glGenVertexArrays(1,&_vaoTerrain); //generate one vertex array object names
  glGenVertexArrays(1,&_vaoQuad); //generate one vertex array object names

  // create the VBO associated with the grid (the terrain)
  glBindVertexArray(_vaoTerrain);//bind a vertex array object
  glBindBuffer(GL_ARRAY_BUFFER,_terrain[0]); // bind _terrain[0] to Vertex attributes (vertices)
  glBufferData(GL_ARRAY_BUFFER,_grid->nbVertices()*3*sizeof(float),_grid->vertices(),GL_STATIC_DRAW);//creates and initializes a buffer object's data store
  glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void *)0);//define an array of generic vertex attribute data
  glEnableVertexAttribArray(0);//Enable a generic vertex attribute array
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,_terrain[1]); // bind _terrain[1] to Vertex attributes (indices)
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,_grid->nbFaces()*3*sizeof(int),_grid->faces(),GL_STATIC_DRAW);//creates and initializes a buffer object's data store

  // create the VBO associated with the screen quad
  glBindVertexArray(_vaoQuad);//bind a vertex array object
  glBindBuffer(GL_ARRAY_BUFFER,_quad); // bind _quad to Vertex attributes (vertices)
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadData),quadData,GL_STATIC_DRAW);//creates and initializes a buffer object's data store
  glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void *)0);//define an array of generic vertex attribute data
  glEnableVertexAttribArray(0);//Disable a generic vertex attribute array
}

void Viewer::deleteVAO() {
  glDeleteBuffers(2,_terrain);
  glDeleteBuffers(1,&_quad);
  glDeleteVertexArrays(1,&_vaoTerrain);
  glDeleteVertexArrays(1,&_vaoQuad);
}

void Viewer::drawVAO() {
  // Draw the 2 triangles !
  glBindVertexArray(_vaoQuad);
  glDrawArrays(GL_TRIANGLES,0,6);
  glBindVertexArray(0);//disable vertex array
}

void Viewer::createShaders() {

  // Add your own shader files here
  _vertexFilenames.push_back("shaders/noise.vert");
  _fragmentFilenames.push_back("shaders/noise.frag");

    // Add your own shader files here
  _vertexFilenames.push_back("shaders/normal.vert");
  _fragmentFilenames.push_back("shaders/normal.frag");

      // Add your own shader files here
  _vertexFilenames.push_back("shaders/displace.vert");
  _fragmentFilenames.push_back("shaders/displace.frag");

    // *** Phong shader TODO***
  //_vertexFilenames.push_back("shaders/phong.vert");
  //_fragmentFilenames.push_back("shaders/phong.frag");
}

void Viewer::enablePerlinShader() {
  glBindFramebuffer(GL_FRAMEBUFFER,_fbo);
  // current shader ID
  GLuint id = _shaders[0]->id();

  // activate the current shader
  glUseProgram(id);

  glClear(GL_COLOR_BUFFER_BIT);

  // actually draw the scene
  drawVAO();

  glActiveTexture(GL_TEXTURE0);//select active texture unit
  glBindTexture(GL_TEXTURE_2D,_noisePerlinId);//bind a named texture to a texturing target (bound to GL_TEXTURE_2D becomes two-dimensional texture)
  glUniform1i(glGetUniformLocation(id,"heightmap"),0);

  glBindFramebuffer(GL_FRAMEBUFFER,0);
}

void Viewer::enableNormalShader() {
  glBindFramebuffer(GL_FRAMEBUFFER,_fbo);
  // current shader ID
  GLuint id = _shaders[1]->id();

  // activate the current shader
  glUseProgram(id);

  GLenum bufferlist [] = {GL_COLOR_ATTACHMENT1};
  glDrawBuffers(1,bufferlist);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  drawVAO();

  glBindFramebuffer(GL_FRAMEBUFFER,0);
}


void Viewer::enableThirdShader() {
  // current shader ID
  GLuint id = _shaders[2]->id();

  // activate the current shader
  glUseProgram(id);

    // send the model-view matrix
  glUniformMatrix4fv(glGetUniformLocation(id,"mdvMat"),1,GL_FALSE,&(_cam->mdvMatrix()[0][0]));

  // send the projection matrix
  glUniformMatrix4fv(glGetUniformLocation(id,"projMat"),1,GL_FALSE,&(_cam->projMatrix()[0][0]));


  glActiveTexture(GL_TEXTURE1);//select active texture unit
  glBindTexture(GL_TEXTURE_2D,_normalId);//bind a named texture to a texturing target (bound to GL_TEXTURE_2D becomes two-dimensional texture)
  glUniform1i(glGetUniformLocation(id,"normalmap"),1);

  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D,_snowTextu);
  glUniform1i(glGetUniformLocation(id, "snowTextu"), 2);

  glActiveTexture(GL_TEXTURE3);
  glBindTexture(GL_TEXTURE_2D,_montagneTextu);
  glUniform1i(glGetUniformLocation(id, "montagneTextu"), 3);

    glActiveTexture(GL_TEXTURE4);
  glBindTexture(GL_TEXTURE_2D,_grassTextu);
  glUniform1i(glGetUniformLocation(id, "grassTextu"), 4);

  glBindVertexArray(_vaoTerrain);
  glDrawElements(GL_TRIANGLES,3*_grid->nbFaces(),GL_UNSIGNED_INT,(void *)0);

  // send the normal matrix (inverse( transpose( top-left 3x3(MDV)))  TODO
  glUniformMatrix3fv(glGetUniformLocation(id,"normalMat"),1,GL_FALSE,&(_cam->normalMatrix()[0][0]));

  // send a light direction (defined in camera space) TODO
  glUniform3fv(glGetUniformLocation(id,"light"),1,&(_light[0]));

}


void Viewer::disableShader() {
  // desactivate all shaders
  glUseProgram(0);
}

void Viewer::paintGL() {

  glViewport(0,0,800,800);

  // tell the GPU to use this specified shader and send custom variables (matrices and others)
  enablePerlinShader();

  enableNormalShader();

  // clear the color and depth buffers
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  enableThirdShader();


  // tell the GPU to stop using this shader
  disableShader();
}

void Viewer::resizeGL(int width,int height) {
  _cam->initialize(width,height,false);
  glViewport(0,0,width,height);
  updateGL();
}

void Viewer::createFBO() {
  // Ids needed for the FBO and associated textures
  glGenFramebuffers(1,&_fbo);
  glGenTextures(1,&_noisePerlinId);
  glGenTextures(1,&_normalId);
}

void Viewer::initFBO() {

  // create the texture for rendering normals
  glBindTexture(GL_TEXTURE_2D,_noisePerlinId);
  glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA32F,800,800,0,GL_RGBA,GL_FLOAT,NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


  glBindTexture(GL_TEXTURE_2D,_normalId);
  glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA32F,800,800,0,GL_RGBA,GL_FLOAT,NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // attach textures to framebuffer object
  glBindFramebuffer(GL_FRAMEBUFFER,_fbo);

  glBindTexture(GL_TEXTURE_2D,_noisePerlinId);
  glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,_noisePerlinId,0);

  glBindTexture(GL_TEXTURE_2D,_normalId);
  glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT1,GL_TEXTURE_2D,_normalId,0);

  glBindFramebuffer(GL_FRAMEBUFFER,0);
}

void Viewer::deleteFBO() {
  // delete all FBO Ids
  glDeleteFramebuffers(1,&_fbo);
  glDeleteTextures(1,&_noisePerlinId);
  glDeleteTextures(1,&_normalId);
}

void Viewer::loadTexture(GLuint id,const char *filename) {
    // load image
    QImage image = QGLWidget::convertToGLFormat(QImage(filename));

    // activate texture
    glBindTexture(GL_TEXTURE_2D,id);

    // set texture parameters
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_MIRRORED_REPEAT);

    // store texture in the GPU
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,image.width(),image.height(),0,
           GL_RGBA,GL_UNSIGNED_BYTE,(const GLvoid *)image.bits());

    // generate mipmaps
    glGenerateMipmap(GL_TEXTURE_2D);
}

void Viewer::createTextures(){


    glGenTextures(1,&_snowTextu);
    loadTexture(_snowTextu, "textures/snow.jpg");

    glGenTextures(1,&_montagneTextu);
    loadTexture(_montagneTextu, "textures/mountains-texture.png");


    glGenTextures(1,&_grassTextu);
    loadTexture(_grassTextu, "textures/grass.jpg");
}

void Viewer::deleteTextures(){
    glDeleteTextures(1,&_montagneTextu);
}

void Viewer::mousePressEvent(QMouseEvent *me) {
  const glm::vec2 p((float)me->x(),(float)(height()-me->y()));

  if(me->button()==Qt::LeftButton) {
    _cam->initRotation(p);
    _mode = false;
  } else if(me->button()==Qt::MidButton) {
    _cam->initMoveZ(p);
    _mode = false;
  } else if(me->button()==Qt::RightButton) {
    _light[0] = (p[0]-(float)(width()/2))/((float)(width()/2));
    _light[1] = (p[1]-(float)(height()/2))/((float)(height()/2));
    _light[2] = 1.0f-std::max(fabs(_light[0]),fabs(_light[1]));
    _light = glm::normalize(_light);
    _mode = true;
  }

  updateGL();
}

void Viewer::mouseMoveEvent(QMouseEvent *me) {
  const glm::vec2 p((float)me->x(),(float)(height()-me->y()));

  if(_mode) {
    // light mode
    _light[0] = (p[0]-(float)(width()/2))/((float)(width()/2));
    _light[1] = (p[1]-(float)(height()/2))/((float)(height()/2));
    _light[2] = 1.0f-std::max(fabs(_light[0]),fabs(_light[1]));
    _light = glm::normalize(_light);
  } else {
    // camera mode
    _cam->move(p);
  }

  updateGL();
}


void Viewer::keyPressEvent(QKeyEvent *ke) {

  // key a: play/stop animation
  if(ke->key()==Qt::Key_A) {
    if(_timer->isActive())
      _timer->stop();
    else
      _timer->start();
  }

  // key i: init camera
  if(ke->key()==Qt::Key_I) {
    _cam->initialize(width(),height(),true);
  }

  // key f: compute FPS
  if(ke->key()==Qt::Key_F) {
    int elapsed;
    QTime timer;
    timer.start();
    unsigned int nb = 500;
    for(unsigned int i=0;i<nb;++i) {
      paintGL();
    }
    elapsed = timer.elapsed();
    double t = (double)nb/((double)elapsed);
    cout << "FPS : " << t*1000.0 << endl;
  }

  // key r: reload shaders
  if(ke->key()==Qt::Key_R) {
    for(unsigned int i=0;i<_vertexFilenames.size();++i) {
      _shaders[i]->reload(_vertexFilenames[i].c_str(),_fragmentFilenames[i].c_str());
    }
  }

  // space: next shader
  if(ke->key()==Qt::Key_Space) {
    _currentshader = (_currentshader+1)%_shaders.size();
  }

  updateGL();
}

void Viewer::initializeGL() {
  // make this window the current one
  makeCurrent();

  // init and chack glew
  glewExperimental = GL_TRUE;

  if(glewInit()!=GLEW_OK) {
    cerr << "Warning: glewInit failed!" << endl;
  }


  // init OpenGL settings
  glClearColor(0.0,0.0,0.0,1.0);
  glEnable(GL_DEPTH_TEST);
  glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
  glViewport(0,0,width(),height());

  // initialize camera
  _cam->initialize(width(),height(),true);

  // load shader files
  createShaders();

  // init and load all shader files
  for(unsigned int i=0;i<_vertexFilenames.size();++i) {
    _shaders.push_back(new Shader());
    _shaders[i]->load(_vertexFilenames[i].c_str(),_fragmentFilenames[i].c_str());
  }

  // VAO creation
  createVAO();
  createTextures();

  // create/init FBO
  createFBO();
  initFBO();

  // starts the timer
  _timer->start();
}


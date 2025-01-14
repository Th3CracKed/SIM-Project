#include <qapplication.h>
#include <QString>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "viewer.h"

using namespace std;

int main(int argc,char** argv) {
  QApplication application(argc,argv);

  QGLFormat fmt;
  fmt.setVersion(3,3);
  fmt.setProfile(QGLFormat::CoreProfile);
  fmt.setSampleBuffers(true);
  
  Viewer viewer(fmt);

  viewer.setWindowTitle("Projet");
  viewer.show();
  
  return application.exec();
}

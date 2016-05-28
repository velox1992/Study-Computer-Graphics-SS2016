#include "MyGLWidget.h"


//---------------------------------------------------------------------------------------------------------
MyGLWidget::MyGLWidget()
{

}

//---------------------------------------------------------------------------------------------------------

MyGLWidget::MyGLWidget(QWidget *parent)
    : QGLWidget(parent)
{

    connect(&timer, SIGNAL(timeout()),this,SLOT(updateGL()));
    timer.start(40);
}

//---------------------------------------------------------------------------------------------------------

void MyGLWidget::fetchArData() {

    arDataPtr->mutex.lock();
    cameraMatrix = arDataPtr->cameraMatrix ;
    modelView_matrix = arDataPtr->modelView_matrix ;
    drawAR = arDataPtr->drawAR ;
    tex = arDataPtr->tex ;
    detectorInitialized =  arDataPtr->detectorInitialized ;
    arDataPtr->mutex.unlock();
}

//---------------------------------------------------------------------------------------------------------

void MyGLWidget::initializeGL()
{
    // Enable GL textures
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

    //glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );

    setFixedHeight(480);
    setFixedWidth(640);

    //glEnable(GL_CULL_FACE);   // Draw Front or Back?

    glClearColor(1.0, 0.0, 0.0, 0.0) ;
    glClearDepth(1.0) ;
    glDepthFunc(GL_LESS) ;
    glEnable(GL_DEPTH_TEST) ;
    glShadeModel(GL_SMOOTH) ;

    glEnable(GL_TEXTURE_2D); // Enable texturing
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glDisable(GL_TEXTURE_2D);

}


//---------------------------------------------------------------------------------------------------------

void MyGLWidget::resizeGL(int width, int height)
{
       // Set viewport to cover the whole window
       glViewport(0, 0, 640, 480);
}



//---------------------------------------------------------------------------------------------------------

void MyGLWidget::paintGL()
{

    fetchArData();

    if ( detectorInitialized ) {

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity() ;
        glOrtho(-4, 4, -3, 3, 0.1, 100);

      // -- Verarbeitung des OpenCV-Bildes
        glEnable(GL_TEXTURE_2D); // Enable texturing
        tex = QGLWidget::convertToGLFormat(tex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex.width(), tex.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, tex.bits());
        glDisable(GL_TEXTURE_2D);

      // -- OpenCV-Bild (Background) zeichnen

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glEnable(GL_TEXTURE_2D);

        glPushMatrix();
        glTranslatef(0.0,0.0,-99) ;

        glBegin(GL_QUADS);
            glTexCoord2f(0,0); glVertex3f(-4, -3, -0.0);
            glTexCoord2f(1,0); glVertex3f( 4, -3, -0.0);
            glTexCoord2f(1,1); glVertex3f( 4,  3, -0.0);
            glTexCoord2f(0,1); glVertex3f(-4,  3, -0.0);
        glEnd();

        glPopMatrix();

        glDisable(GL_TEXTURE_2D);



        // Vllt Fehler wenn nil zugriff!! Deshalb Status Variablen
        if ( drawAR && projectionCalculated ) {

            glMatrixMode(GL_PROJECTION);
            glLoadIdentity() ;
            glLoadMatrixd((double*)persp.data);


            glMatrixMode(GL_MODELVIEW);
            glPushMatrix();
            glLoadMatrixd((double*)modelView_matrix.data);

            glTranslatef(0.005,0.008,0);    // Kleine Korrektur


            // Würfel - Rechte Hand Regel: Daumen zeigt nach außen.
            glBegin(GL_QUADS) ;

                // Back
                glColor3f(1,0,0);
                glVertex3f( -0.01f, -0.01f, -0.01f);
                glVertex3f( -0.01f,  0.01f, -0.01f);
                glVertex3f(  0.01f,  0.01f, -0.01f);
                glVertex3f(  0.01f, -0.01f, -0.01f);
                // Front
                glColor3f(0,0,1);
                glVertex3f(  0.01f, -0.01f, 0.01f);
                glVertex3f(  0.01f,  0.01f, 0.01f);
                glVertex3f( -0.01f,  0.01f, 0.01f);
                glVertex3f( -0.01f, -0.01f, 0.01f);

                // Right
                glColor3f(0,1,0);
                glVertex3f(  0.01f,  0.01f,  0.01f);
                glVertex3f(  0.01f, -0.01f,  0.01f);
                glVertex3f(  0.01f, -0.01f, -0.01f);
                glVertex3f(  0.01f,  0.01f, -0.01f);
                // Left
                glColor3f(0,1,1);
                glVertex3f( -0.01f, -0.01f,  0.01f);
                glVertex3f( -0.01f,  0.01f,  0.01f);
                glVertex3f( -0.01f,  0.01f, -0.01f);
                glVertex3f( -0.01f, -0.01f, -0.01f);

                // Top
                glColor3f(1,1,0);
                glVertex3f(  0.01f,  0.01f, -0.01f);
                glVertex3f( -0.01f,  0.01f, -0.01f);
                glVertex3f( -0.01f,  0.01f,  0.01f);
                glVertex3f(  0.01f,  0.01f,  0.01f);

                // Bottom
                glColor3f(1,1,0);
                glVertex3f( -0.01f,  -0.01f,  0.01f);
                glVertex3f( -0.01f,  -0.01f, -0.01f);
                glVertex3f(  0.01f,  -0.01f, -0.01f);
                glVertex3f(  0.01f,  -0.01f,  0.01f);


            glEnd() ;

            // drawAxis();

            glPopMatrix();

            drawAR = false ;
        }

    } // if detectorInitialized


}

//---------------------------------------------------------------------------------------------------------

void MyGLWidget::drawAxis() {

      // X - Axis red
      glBegin(GL_LINES);
      glColor3f(1.0f, 0.0f, 0.0f);
      glVertex3f(-0.05f, 0.0f, 0.0f);
      glVertex3f(0.05f, 0.0f, 0.0f);
      glEnd();
      // Y - Axis green
      glBegin(GL_LINES);
      glColor3f(0.0f, 1.0f, 0.0f);
      glVertex3f(0.0f, -0.05f, 0.0f);
      glVertex3f(0.0f, 0.05f, 0.0f);
      glEnd();
      // Z - Axis blue
      glBegin(GL_LINES);
      glColor3f(0.0f, 0.0f, 1.0f);
      glVertex3f(0.0f, 0.0f, 0.05f);
      glVertex3f(0.0f, 0.0f, -0.05f);
      glEnd();

}

//---------------------------------------------------------------------------------------------------------

void MyGLWidget::loadProjectionMatrix() {

    fetchArData();  // Get the Data: Camera Matrix
    double nearVal = 0.1;
    double farVal = 100.0 ;

    // Beim Einlesen der cameraMatrix aus dem Config-File gehen Kodierungsinfos verloren.
    // Deshalb hier auf Nummer sicher gehen.
    cv::Mat cameraMatrixConverted ;
    cameraMatrix.convertTo(cameraMatrixConverted,CV_32F);



    if(persp.empty()) {
                persp.create(4,4);
                persp.setTo(0);

                // http://kgeorge.github.io/2014/03/08/calculating-opengl-perspective-matrix-from-opencv-intrinsic-matrix/
                qDebug() << cameraMatrixConverted.at<float>(0,0);
                qDebug() << cameraMatrixConverted.at<float>(1,1);
                qDebug() << cameraMatrixConverted.at<float>(0,2);
                qDebug() << cameraMatrixConverted.at<float>(1,2);

                double fx = cameraMatrixConverted.at<float>(0,0);
                double fy = cameraMatrixConverted.at<float>(1,1);
                double cx = cameraMatrixConverted.at<float>(0,2);
                double cy = cameraMatrixConverted.at<float>(1,2);
                persp(0,0) = fx/cx;
                persp(1,1) = fy/cy;
                persp(2,2) = -(farVal+nearVal)/(farVal-nearVal);
                persp(2,3) = -2.0*farVal*nearVal / (farVal-nearVal);
                persp(3,2) = -1.0;


                persp = persp.t(); //to col-major

                glMatrixMode(GL_PROJECTION);;
                glLoadMatrixd((double*)persp.data);
                projectionCalculated = true ;

            }

}

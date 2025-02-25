#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    explicit GLWidget(QWidget *parent = nullptr);
    ~GLWidget();
protected:
    void initializeGL() override;  // Runs once when opengl initializes
    void resizeGL(int w, int h) override;  // Handles resizing
    void paintGL() override;  // Rendering logic
};

#endif // GLWIDGET_H

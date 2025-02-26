#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include "model.h"

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    explicit GLWidget(QWidget *parent, std::string path);
    ~GLWidget();

    void loadModel(const std::string &path); // Function to load new model
protected:
    Model model;
    void initializeGL() override;  // Runs once when opengl initializes
    void resizeGL(int w, int h) override;  // Handles resizing
    void paintGL() override;  // Rendering logic

private:
    std::string initialFilePath;
};

#endif // GLWIDGET_H

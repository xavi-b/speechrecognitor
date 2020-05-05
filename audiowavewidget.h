#ifndef AUDIOWAVEWIDGET_H
#define AUDIOWAVEWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLContext>
#include <QVariantAnimation>
#include <QTimer>
#include <QPalette>
#include <QtMath>
#include <QDebug>

class AudioWaveWidget : public QOpenGLWidget
{
    Q_OBJECT
private:
    QColor waveColor;
    QColor bgColor;
    QVariantAnimation* animation = nullptr;
    QTimer* fpsTimer = nullptr;
    float timeVariant = 1;

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

public:
    AudioWaveWidget(QWidget *parent = nullptr);

    void setWaveColor(QColor const& color)
    {
        this->waveColor = color;
    }
    void setBackgroundColor(QColor const& color)
    {
        this->bgColor = color;
    }

    void start();
    void stop();
};

#endif // AUDIOWAVEWIDGET_H

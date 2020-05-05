#include "audiowavewidget.h"

void AudioWaveWidget::initializeGL()
{

}

void AudioWaveWidget::resizeGL(int w, int h)
{

}

void AudioWaveWidget::paintGL()
{
    float wavelength = 0.5;
    float amplitude = 0.8;
    float inc = 0.005;
    float x, y;
    float thickness = 5;
    float widthRatio = 0.95;

    glLineWidth(thickness);
    glEnable(GL_LINE_SMOOTH);

    glClearColor(
                this->bgColor.redF(),
                this->bgColor.greenF(),
                this->bgColor.blueF(),
                1);
    glClear(GL_COLOR_BUFFER_BIT);

    glBegin(GL_LINE_STRIP);

        glColor3f(
                    this->waveColor.redF(),
                    this->waveColor.greenF(),
                    this->waveColor.blueF()
                    );

        for(x = -1; x <= 1; x+=inc)
        {
            y = amplitude * qSin(this->timeVariant * 2 * M_PI / wavelength + 3 * M_PI * x) ;
            y = y * (-x*x+1);
            glVertex3f(x * widthRatio, y, 0);
        }

    glEnd();
}

AudioWaveWidget::AudioWaveWidget(QWidget *parent)
    : QOpenGLWidget(parent)
{
    this->setMinimumWidth(100);

    this->waveColor = QColor("#e53935");
    this->bgColor = this->palette().color(QPalette::Window);

    this->animation = new QVariantAnimation(this);
    this->animation->setDuration(6000);
    this->animation->setStartValue(M_PI_2);
    this->animation->setEndValue(-M_PI_2);
    this->animation->setLoopCount(-1);

    connect(this->animation, &QVariantAnimation::valueChanged, this, [=](QVariant const& value)
    {
        this->timeVariant = value.toFloat();
    });

    this->fpsTimer = new QTimer(this);
    this->fpsTimer->setInterval(1000/60.0); // 60FPS

    connect(this->fpsTimer, &QTimer::timeout, this, [=]()
    {
        this->update();
    });
}

void AudioWaveWidget::start()
{
    this->animation->start();
    this->fpsTimer->start();
}

void AudioWaveWidget::stop()
{
    this->animation->stop();
    this->fpsTimer->stop();
}


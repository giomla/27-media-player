#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QMainWindow>
#include <QWidget>
#include <QMediaPlayer>

class QAbstractButton;
class QSlider;
class QGraphicsVideoItem;

class videoplayer : public QWidget
{
    Q_OBJECT //Macro koji mora da se nalazi u klasi ukoliko ona deklarise signale ili slotove.

public:
    videoplayer(QWidget *parent = nullptr);
    ~videoplayer();
    bool isAvaliable() const;
    QMediaPlayer::State state() const;


public slots:
    void openFile(); //TODO
    void playOnClick(); //TODO

signals:
    void play();
    void stop();
    void pause();


private slots:
    void mediaStateChanged(QMediaPlayer::State state);
    //TODO

private:
    QMediaPlayer* m_mediaPlayer = nullptr;
        QMediaPlayer::State m_playerState = QMediaPlayer::StoppedState;
        QGraphicsVideoItem* m_videoItem = nullptr;
        QAbstractButton* m_playButton = nullptr;
        QAbstractButton* m_stopButton = nullptr;
        QAbstractButton* m_forwardButton = nullptr;
        QAbstractButton* m_backwardButton = nullptr;
        QSlider* m_Slider = nullptr;


};
#endif // VIDEOPLAYER_H

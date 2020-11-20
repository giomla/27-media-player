#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QMainWindow>
#include <QWidget>
#include <QMenuBar>
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
    bool isMuted() const;


public slots:
    void openFile(); //TODO
    void playClicked();
    void muteClicked();
    void setMuted(bool muted);

signals:
    void play();
    void stop();
    void pause();
    void changeMuting(bool muting);

private slots:
    void mediaStateChanged(QMediaPlayer::State state);
    //TODO

private:
    QMediaPlayer* m_mediaPlayer = nullptr;
    bool m_playerMuted = false;
    QMediaPlayer::State m_playerState = QMediaPlayer::StoppedState;
    QGraphicsVideoItem* m_videoItem = nullptr;
    QAbstractButton* m_playButton = nullptr;
    QAbstractButton* m_stopButton = nullptr;
    QAbstractButton* m_forwardButton = nullptr;
    QAbstractButton* m_backwardButton = nullptr;
    QAbstractButton* m_muteButton = nullptr;
    QSlider* m_Slider = nullptr;
    QMenuBar *m_menuBar = nullptr;

};
#endif // VIDEOPLAYER_H

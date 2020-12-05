#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

//maximum speed of playback rate, and default step of forward and backward rate play buttons
#define MAX_PLAYBACK_RATE 4.00
#define PLAYBACK_STEP 0.25

#include <QMainWindow>
#include <QWidget>
#include <QMenuBar>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QKeySequence>
#include <QGraphicsVideoItem>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QLabel>

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
    void createMenuBar();
    int volume() const;
    void updateDurationInfo(qint64 currInfo);
    void fitView();

    void  loadPlaylist(QList<QUrl> urls);


public slots:
    void openFile();
    void durationChanged(qint64 duration);
    void calcVideoFactor(QSizeF size);
    void positionChanged(qint64 progress);
    void playClicked();
    void stopClicked();
    void forwardClicked();
    void backwardClicked();
    void seekForwardClicked();
    void seekBackwardClicked();
    void muteClicked();
    void volumeIncrease();
    void volumeDecrease();
    void setMuted(bool muted);
    void seek(int seconds);
    void exit();
    void setVolume(qint64 vol);


signals:
    void play();
    void stop();
    void pause();
    void changeVolume(int volume);

private slots:
    void mediaStateChanged(QMediaPlayer::State state);
    void onVolumeSliderChanged();

    //TODO

private:
    QMediaPlayer* m_mediaPlayer = nullptr;
    QMediaPlaylist *m_playlist = nullptr;
    bool m_playerMuted = false;
    QMediaPlayer::State m_playerState = QMediaPlayer::StoppedState;
    QGraphicsVideoItem* m_videoItem = nullptr;
    QGraphicsScene* m_scene = nullptr; //Pravljenje scene
    QGraphicsView* m_graphicsView = nullptr;
    QAbstractButton* m_playButton = nullptr;
    QAbstractButton* m_stopButton = nullptr;
    QAbstractButton* m_forwardButton = nullptr;
    QAbstractButton* m_backwardButton = nullptr;
    QAbstractButton* m_muteButton = nullptr;
    QAbstractButton* m_openButton = nullptr;
    QAbstractButton* m_seekForwardButton = nullptr;
    QAbstractButton* m_seekBackwardButton = nullptr;

    QSlider* m_Slider = nullptr;
    QSlider* m_volumeSlider = nullptr;
    QLabel* m_durationInfo=nullptr;
    QMenuBar *m_menuBar = nullptr;
    QMenu *m_rightClickMenu = nullptr;
    qint64 m_duration;
    QLabel *m_text = nullptr;

protected:
    void resizeEvent(QResizeEvent *) override;
    void showEvent(QShowEvent *) override;
    void keyPressEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
};
#endif // VIDEOPLAYER_H

#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

//maximum speed of playback rate, and default step of forward and backward rate play buttons
#define MAX_PLAYBACK_RATE 4.00
#define SEEK_STEP 5000
#define VOLUME_STEP 5
#define DEFAULT_ANNOTATIONS_FOLDER "annotations"

#include <QMainWindow>
#include <QWidget>
#include <QMenuBar>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QKeySequence>
#include <QGraphicsVideoItem>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsSceneDragDropEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QLabel>
#include <QGraphicsItem>
#include <QWheelEvent>
#include <QMap>
#include <QPair>
#include <QGraphicsTextItem>
#include <QTime>
#include "../headers/subtitles.h"
#include "../headers/commands.h"
#include "../headers/annotation.h"
#include <QVector>
#include <QFile>
#include <QJsonDocument>
#include <string>

class QGraphicsVideoItem;
class Annotation;


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
    void updateVolumeSlider();
    void loadPlaylist(QList<QUrl> urls);
    void addToPlaylist(QList<QUrl> urls);
    void connections();
    void setTitle(QList<QUrl> urls);
    void addButtons(QRect screenGeometry);
    void playlistDoubleClickPlay();
    bool loadFile(const QString &fileName);


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
    void addSubtitle();
    void addAnnotation();
    void setAnnotationsFromJson();
    void saveAnnotationsToJsonFile();
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
    QGraphicsTextItem* subtitleText = nullptr;
    QMenuBar *m_menuBar = nullptr;
    QMenu *m_rightClickMenu = nullptr;
    subtitles* subtitle;
    commands* cmnds;
    qint64 m_duration = 0;
    int volumeBeforeMute = -1;
    QLabel *m_text = nullptr;
    const QBrush *darkGrayColor = nullptr;
    QAction* addSubtitles = nullptr;
    QVector<Annotation*> m_videoAnnotations;

    int numOfAnnotations = 0;
protected:
    void resizeEvent(QResizeEvent *) override;
    void showEvent(QShowEvent *) override;
    void keyPressEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;

    void wheelEvent(QWheelEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;

};
#endif // VIDEOPLAYER_H

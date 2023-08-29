#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

// maximum speed of playback rate, and default step of forward and backward rate
// play buttons
#define MAX_PLAYBACK_RATE 4.00
#define SEEK_STEP 5000
#define VOLUME_STEP 5
#define DEFAULT_ANNOTATIONS_FOLDER "annotations"

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFile>
#include <QFormLayout>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsSceneDragDropEvent>
#include <QGraphicsTextItem>
#include <QGraphicsVideoItem>
#include <QGraphicsView>
#include <QJsonDocument>
#include <QKeySequence>
#include <QLabel>
#include <QMainWindow>
#include <QMap>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QMenuBar>
#include <QPair>
#include <QStandardPaths>
#include <QTime>
#include <QVector>
#include <QWheelEvent>
#include <QWidget>
#include <string>

#include "../headers/annotation.h"
#include "../headers/commands.h"
#include "../headers/playlist.h"
#include "../headers/rightClickMenu.h"
#include "../headers/subtitles.h"

class QGraphicsVideoItem;
class Annotation;

class videoplayer : public QWidget {
  Q_OBJECT // Macro koji mora da se nalazi u klasi ukoliko ona deklarise
           // signale ili slotove.

      public : videoplayer(QWidget *parent = nullptr);
  ~videoplayer();

  bool isAvaliable() const;
  QMediaPlayer::State state() const;
  /*! function that tells us if video is muted */
  bool isMuted() const;
  /*! fuction that creates upper menu bar */
  void createMenuBar();
  /*! fuction that returns current volume */
  int volume() const;
  /*! function that updates duration of a video when a video is loaded */
  void updateDurationInfo(qint64 currInfo);
  /*! function that fits the view of the video player */
  void fitView();
  /*! function that updates volume slider according to volume */
  void updateVolumeSlider();
  /*! function that loads playlist */
  void loadPlaylist(QList<QUrl> urls);
  /*! function that adds a video to playlist */
  void addToPlaylist(QList<QUrl> urls);
  /*! function that holds all connections so that readabilty is better */
  void connections();
  /*! function that sets the title */
  void setTitle(QList<QUrl> urls);
  /*! function that adds buttons to the video player */
  void addButtons(QRect screenGeometry);
  /*! function that loads a video from playlist after the name of the video is
   * double clicked */
  void playlistDoubleClickPlay();
  /*! function that loads a video */
  bool loadFile(const QString &fileName);
  /*! function that checks regex */
  static bool annotationFieldRegexCheck(QLineEdit *widthLineEdit,
                                        QLineEdit *heightLineEdit,
                                        QLineEdit *durationLineEdit,
                                        QLineEdit *beginLineEdit,
                                        QLineEdit *contentLineEdit);

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
  static void exit();
  void setVolume(qint64 vol);
  void addSubtitle();
  void addAnnotation();
  void setAnnotationsFromJson();
  void saveAnnotationsToJsonFile();
  static void aboutPlayer();
signals:
  void play();
  void stop();
  void pause();
  void changeVolume(int volume);

private slots:
  void mediaStateChanged(QMediaPlayer::State state);
  void onVolumeSliderChanged();

private:
  QMediaPlayer *m_mediaPlayer = nullptr;
  bool m_playerMuted = false;
  QMediaPlayer::State m_playerState = QMediaPlayer::StoppedState;
  QGraphicsVideoItem *m_videoItem = nullptr;
  QGraphicsScene *m_scene = nullptr; // Pravljenje scene
  QGraphicsView *m_graphicsView = nullptr;
  QGraphicsTextItem *subtitleText = nullptr;
  QMenuBar *m_menuBar = nullptr;
  subtitles *subtitle;
  commands *cmnds;
  qint64 m_duration = 0;
  int volumeBeforeMute = -1;
  QLabel *m_text = nullptr;
  const QBrush *blackBackgroundColor = nullptr;
  QAction *addSubtitles = nullptr;
  QVector<Annotation *> m_videoAnnotations;
  rightClickMenu *m_rightClickMenu = nullptr;
  playlist *Playlist = nullptr;
  QFormLayout *formLayout = nullptr;
  QTimer *mouseHideTimer = nullptr;

  int numOfAnnotations = 0;

protected:
  void resizeEvent(QResizeEvent *) override;
  void showEvent(QShowEvent *) override;
  void keyPressEvent(QKeyEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mouseDoubleClickEvent(QMouseEvent *event) override;
  void contextMenuEvent(QContextMenuEvent *event) override;
  void hideMouse();

  void wheelEvent(QWheelEvent *event) override;
  void dropEvent(QDropEvent *event) override;
  void dragEnterEvent(QDragEnterEvent *event) override;
};
#endif // VIDEOPLAYER_H

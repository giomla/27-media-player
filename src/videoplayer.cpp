#include "../headers/videoplayer.h"

#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QtWidgets>
#include <cstdlib>
#include <iostream>

videoplayer::videoplayer(QWidget *parent) : QWidget(parent) {
  const QRect screenGeometry = QApplication::desktop()->screenGeometry(this);

  m_videoItem = new QGraphicsVideoItem;
  m_videoItem->setSize(
      QSizeF(screenGeometry.width() / 2, screenGeometry.height()));
  m_videoItem->setAspectRatioMode(Qt::AspectRatioMode::KeepAspectRatio);

  m_mediaPlayer = new QMediaPlayer(this, QMediaPlayer::VideoSurface);
  Playlist = new playlist(this, m_mediaPlayer);
  m_mediaPlayer->setPlaylist(Playlist->m_playlist);
  m_mediaPlayer->setVideoOutput(m_videoItem);

  blackBackgroundColor = new QBrush(QColor(0, 0, 0));

  m_scene = new QGraphicsScene(this);
  m_scene->addItem(m_videoItem);
  m_scene->setBackgroundBrush(*blackBackgroundColor);

  m_graphicsView = new QGraphicsView(m_scene);
  m_graphicsView->setContentsMargins(0, 0, 0, 0);
  m_graphicsView->setAcceptDrops(false);
  m_graphicsView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
  subtitle = new subtitles();
  cmnds = new commands(this);
  m_rightClickMenu = new rightClickMenu(this);

  subtitleText = new QGraphicsTextItem(m_videoItem);
  subtitleText->setDefaultTextColor(QColor("white"));

  this->createMenuBar();

  this->setStyleSheet(QStringLiteral("background-color:#222222"));
  this->setAcceptDrops(true);

  cmnds->m_Slider->setRange(
      0, m_mediaPlayer->duration() /
             1000); // range of slider from zero to length of media played
  cmnds->createSliderLayout();
  cmnds->createCommandLayout();

  // vertical box with all elements
  auto *layout = new QVBoxLayout(this);
  layout->setSpacing(0);
  layout->addWidget(m_menuBar);
  layout->addWidget(m_graphicsView);
  layout->addLayout(cmnds->sliderLayout);
  layout->addLayout(cmnds->commandsLayout);
  layout->addWidget(Playlist->m_playlist_entries);
  layout->setSpacing(0);

  m_graphicsView->setFocus();

  mouseHideTimer = new QTimer();
  mouseHideTimer->setInterval(2000);
  mouseHideTimer->setSingleShot(true);

  this->connections();
}

videoplayer::~videoplayer() {

  delete subtitle;
  delete blackBackgroundColor;
  delete m_mediaPlayer;
  for (auto it : qAsConst(m_videoAnnotations)) {
    delete it;
  }
}

auto videoplayer::isAvaliable() const -> bool {
  return m_mediaPlayer->isAvailable();
}

void videoplayer::connections() {
  connect(m_mediaPlayer, &QMediaPlayer::stateChanged, this,
          &videoplayer::mediaStateChanged);
  connect(cmnds->m_volumeSlider, &QSlider::valueChanged, this,
          &videoplayer::onVolumeSliderChanged);
  connect(cmnds->m_Slider, &QSlider::sliderMoved, this, &videoplayer::seek);
  connect(m_mediaPlayer, &QMediaPlayer::durationChanged, this,
          &videoplayer::durationChanged);
  connect(m_mediaPlayer, &QMediaPlayer::positionChanged, this,
          &videoplayer::positionChanged);
  connect(cmnds->m_playButton, &QAbstractButton::clicked, this,
          &videoplayer::playClicked);
  connect(cmnds->m_openButton, &QAbstractButton::clicked, this,
          &videoplayer::openFile);
  connect(cmnds->m_muteButton, &QAbstractButton::clicked, this,
          &videoplayer::muteClicked);
  connect(cmnds->m_forwardButton, &QAbstractButton::clicked, this,
          &videoplayer::forwardClicked);
  connect(cmnds->m_backwardButton, &QAbstractButton::clicked, this,
          &videoplayer::backwardClicked);
  connect(cmnds->m_seekForwardButton, &QAbstractButton::clicked, this,
          &videoplayer::seekForwardClicked);
  connect(cmnds->m_seekBackwardButton, &QAbstractButton::clicked, this,
          &videoplayer::seekBackwardClicked);
  connect(cmnds->m_stopButton, &QAbstractButton::clicked, this,
          &videoplayer::stopClicked);
  connect(m_videoItem, &QGraphicsVideoItem::nativeSizeChanged, this,
          &videoplayer::calcVideoFactor);
  connect(Playlist->m_playlist_entries, &QListWidget::doubleClicked, this,
          &videoplayer::playlistDoubleClickPlay);
  connect(cmnds->m_showPlaylistButton, &QAbstractButton::clicked, Playlist,
          &playlist::showPlaylist);
  connect(cmnds->m_showPlaylistButton, &QAbstractButton::clicked, this,
          &videoplayer::fitView);
  connect(mouseHideTimer, &QTimer::timeout, this, &videoplayer::hideMouse);
  // when another video is loaded up, the native resolution of the video
  // changes so we monitor for that, so we can adjust the resolution
  // accordingly
}

void videoplayer::positionChanged(qint64 progress) {
  if (!cmnds->m_Slider->isSliderDown())
    cmnds->m_Slider->setValue(progress / 1000);
  // update annotations for video, go through the vector and update for each
  // accordingly
  if (!m_videoAnnotations.empty()) {
    for (auto it : qAsConst(m_videoAnnotations)) {
      it->setCurrTimeOfVideo(progress);
      it->update();
    }
  }
  updateDurationInfo(progress / 1000);
}

void videoplayer::durationChanged(qint64 duration) {
  m_duration = duration / 1000;
  cmnds->m_Slider->setMaximum(m_duration);
}

void videoplayer::updateDurationInfo(qint64 currInfo) {
  QString tStr;

  if ((currInfo != 0) || (m_duration != 0)) {
    QTime currentTime((currInfo / 3600) % 60, (currInfo / 60) % 60,
                      currInfo % 60, (currInfo * 1000) % 1000);
    QTime totalTime((m_duration / 3600) % 60, (m_duration / 60) % 60,
                    m_duration % 60, (m_duration * 1000) % 1000);
    QString format = QStringLiteral("mm:ss");

    if (m_duration > 3600)
      format = QLatin1String("hh:mm:ss");

    if (subtitle->isAddedSubtitle()) {
      for (auto &sub : subtitle->subs) {
        if (currentTime.toString(QStringLiteral("hh:mm:ss")) ==
            sub.getBeginTime()) {
          subtitleText->setHtml("<p align='center'>" + sub.getLine());
          subtitleText->show();
        }
        if (currentTime.toString(QStringLiteral("hh:mm:ss")) ==
            sub.getEndTime()) {
          subtitleText->hide();
        }
      }
    }
    tStr = currentTime.toString(format) + " / " + totalTime.toString(format);
    cmnds->m_durationInfo->setText(tStr);
  }
}

auto videoplayer::state() const -> QMediaPlayer::State { return m_playerState; }

void videoplayer::mediaStateChanged(QMediaPlayer::State state) {
  if (m_playerState != state) {
    m_playerState = state;

    switch (state) {
    case QMediaPlayer::PlayingState:
      cmnds->enableAllCommands();
      cmnds->m_playButton->setIcon(
          style()->standardIcon(QStyle::SP_MediaPause));
      m_rightClickMenu->playingState();
      break;
    case QMediaPlayer::StoppedState:
      cmnds->m_playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
      m_rightClickMenu->StoppedState();
      break;
    case QMediaPlayer::PausedState:
      cmnds->m_playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
      m_rightClickMenu->PausedState();
      break;
    default:
      cmnds->m_stopButton->setEnabled(false);
      cmnds->m_playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
      break;
    }
  }
}

// this is to avoid a known bug, so should work ok on all platforms, and not
// just some https://bugreports.qt.io/browse/QTBUG-28850

void videoplayer::calcVideoFactor(QSizeF size) {
  QRectF rect = QRectF(0, 0, size.width(), size.height());
  m_videoItem->setSize(QSizeF(size.width(), size.height()));
  m_graphicsView->fitInView(rect, Qt::AspectRatioMode::KeepAspectRatio);
  m_graphicsView->setSceneRect(0, 0, size.width(), size.height());
  fitView();
}

void videoplayer::showEvent(QShowEvent * /*event*/) { fitView(); }

void videoplayer::resizeEvent(QResizeEvent * /*event*/) { fitView(); }

void videoplayer::fitView() {
  const QRectF rect = m_graphicsView->rect();

  m_videoItem->setSize(QSizeF(rect.width(), rect.height()));
  m_graphicsView->fitInView(rect, Qt::AspectRatioMode::KeepAspectRatio);
  m_graphicsView->setSceneRect(rect);
  for (auto it : qAsConst(m_videoAnnotations))
    it->resizeOccured();

  if (isMaximized()) {
    subtitleText->setFont(QFont(QStringLiteral("Times"), 25));
    subtitleText->setTextWidth(rect.width() / 2);
    subtitleText->setPos(rect.width() / 4, rect.height() - 110);

  } else if (isFullScreen()) {
    subtitleText->setPos(rect.width() / 4, rect.height() - 150);
    subtitleText->setFont(QFont(QStringLiteral("Times"), 35));
    subtitleText->setTextWidth(rect.width() / 2);

  } else {
    subtitleText->setPos(rect.width() / 4, rect.height() - 70);
    subtitleText->setFont(QFont(QStringLiteral("Times"), 20));
    subtitleText->setTextWidth(rect.width() / 2);
  }
}

void videoplayer::openFile() {
  QFileDialog fileDialog(this);
  fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
  fileDialog.setWindowTitle(tr("Open Files"));
  // Defaulting to the home location, prevents possible empty string
  // being returned, as moviesLocation may not be set on system
  QString path = QStandardPaths::standardLocations(QStandardPaths::HomeLocation)
                     .value(0, QDir::homePath());
  fileDialog.setDirectory(path);
  fileDialog.setNameFilter(tr("Video File(*.mkv *.mp4 *.avi)"));
  fileDialog.setFileMode(QFileDialog::ExistingFiles);
  fileDialog.show();

  if (fileDialog.exec() != 0) {
    subtitle->subs.clear();
    subtitle->setAddedSubtitle(false);
    subtitleText->hide();
    Playlist->loadPlaylist(fileDialog.selectedUrls());
    m_graphicsView->setFocus();
    setAnnotationsFromJson();
  }
}
void videoplayer::dragEnterEvent(QDragEnterEvent *event) {
  if (event->mimeData()->hasUrls()) {
    event->acceptProposedAction();
  }
}

void videoplayer::dropEvent(QDropEvent *event) {
  Playlist->loadPlaylist(event->mimeData()->urls());
}

void videoplayer::playClicked() {
  switch (m_playerState) {
  case QMediaPlayer::StoppedState:
  case QMediaPlayer::PausedState:
    m_mediaPlayer->play();
    QTimer::singleShot(2000, m_text, &QLabel::hide);
    m_text->show();
    m_text->setText(QStringLiteral("Playing"));
    break;
  case QMediaPlayer::PlayingState:
    m_mediaPlayer->pause();
    m_text->show();
    m_text->setText(QStringLiteral("Paused"));
    QTimer::singleShot(2000, m_text, &QLabel::hide);
    break;
  }
}

auto videoplayer::isMuted() const -> bool { return m_playerMuted; }

void videoplayer::setMuted(bool muted) {
  if (muted != m_playerMuted) {
    m_playerMuted = muted;
  }
}

void videoplayer::volumeIncrease() {
  m_mediaPlayer->setVolume(m_mediaPlayer->volume() + VOLUME_STEP);
}

void videoplayer::volumeDecrease() {
  m_mediaPlayer->setVolume(m_mediaPlayer->volume() - VOLUME_STEP);
}

void videoplayer::seek(int seconds) {
  m_mediaPlayer->setPosition(seconds * 1000);
}

void videoplayer::muteClicked() {
  if (!m_mediaPlayer->isMuted()) {
    m_mediaPlayer->setMuted(true);
    cmnds->m_muteButton->setIcon(
        style()->standardIcon(QStyle::SP_MediaVolumeMuted));
    QTimer::singleShot(2000, m_text, &QLabel::hide);
    m_text->show();
    m_text->setText(QStringLiteral("Muted"));
  } else {
    m_mediaPlayer->setMuted(false);
    cmnds->m_muteButton->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
    QTimer::singleShot(2000, m_text, &QLabel::hide);
    m_text->show();
    m_text->setText(QStringLiteral("Unmuted"));
  }
}

void videoplayer::createMenuBar() {
  m_menuBar = new QMenuBar();
  m_menuBar->setStyleSheet(QStringLiteral("color:white"));
  m_text = new QLabel();
  m_text->setStyleSheet(QStringLiteral("background-color:rgba(255,255,255,0)"));
  m_text->setFixedSize(150, 20);
  m_text->hide();
  m_menuBar->setCornerWidget(m_text);
  auto *file = new QMenu(QStringLiteral("File"));
  auto *playback = new QMenu(QStringLiteral("Playback"));
  auto *help = new QMenu(QStringLiteral("Help"));

  // file padajuci meni
  QAction *openFile = file->addAction(QStringLiteral("Open file"));
  QAction *exit = file->addAction(
      QStringLiteral("Abort")); // iz nekog razloga ne izbacuje EXIT akciju
  file->setStyleSheet(
      QStringLiteral(" QMenu {Background-color: #111111;color:white;}"));

  // audio padajuci meni
  QAction *mute = playback->addAction(QStringLiteral("Mute"));
  QAction *seekBack = playback->addAction(QStringLiteral("Seek backward"));
  QAction *seekFwd = playback->addAction(QStringLiteral("Seek forward"));
  playback->setStyleSheet(
      QStringLiteral(" QMenu {Background-color: #222222;color:white;}"));

  // help padajuci meni
  QAction *about = help->addAction(QStringLiteral("About player"));
  help->setStyleSheet(
      QStringLiteral(" QMenu {Background-color: #222222;color:white;}"));

  // povezivanje akcija sa funkcijama pri kliku
  connect(openFile, &QAction::triggered, this, &videoplayer::openFile);
  connect(exit, &QAction::triggered, this, &videoplayer::exit);
  connect(mute, &QAction::triggered, this, &videoplayer::muteClicked);
  connect(seekBack, &QAction::triggered, this,
          &videoplayer::seekBackwardClicked);
  connect(seekFwd, &QAction::triggered, this, &videoplayer::seekForwardClicked);
  connect(about, &QAction::triggered, this, &videoplayer::aboutPlayer);
  // precice na tastaturi
  openFile->setShortcut(QKeySequence::Open); // CTRL + O
  exit->setShortcut(Qt::CTRL + Qt::Key_Q);
  seekBack->setShortcut(Qt::Key_Left);
  seekFwd->setShortcut(Qt::Key_Right);
  // incVol->setShortcut(Qt::Key_Plus);
  // decVol->setShortcut(Qt::Key_Minus);
  mute->setShortcut(Qt::Key_M);

  // povezivanje menija u jedan meni bar
  m_menuBar->addMenu(file);
  m_menuBar->addMenu(playback);
  m_menuBar->addMenu(help);
}
// Ostale funckije potrebne pri konekciji

void videoplayer::stopClicked() {
  m_mediaPlayer->stop();
  QTimer::singleShot(2000, m_text, &QLabel::hide);
  m_text->show();
  m_text->setText(QStringLiteral("Stopped"));
}

void videoplayer::forwardClicked() {
  Playlist->m_playlist->next();
  QTimer::singleShot(2000, m_text, &QLabel::hide);
  m_text->show();
  m_text->setText(QStringLiteral("Forward"));
  QFileInfo fileInfo(
      Playlist->m_playlist->currentMedia().canonicalUrl().path());
  QString filename = fileInfo.fileName();
  this->setWindowTitle(filename.split('.')[0]);
}

void videoplayer::backwardClicked() {
  Playlist->m_playlist->previous();
  m_mediaPlayer->play();
  QTimer::singleShot(2000, m_text, &QLabel::hide);
  m_text->show();
  m_text->setText(QStringLiteral("Backward"));
  QFileInfo fileInfo(
      Playlist->m_playlist->currentMedia().canonicalUrl().path());
  QString filename = fileInfo.fileName();
  this->setWindowTitle(filename.split('.')[0]);
}

// Not all playback services support change of the playback rate.
// It is framework defined as to the status and quality of audio and video
// while fast forwarding or rewinding.
void videoplayer::seekBackwardClicked() {
  qint64 value = 0.0;
  if (m_mediaPlayer->position() > SEEK_STEP)
    value = m_mediaPlayer->position() - SEEK_STEP;

  m_mediaPlayer->setPosition(value);
  QTimer::singleShot(2000, m_text, &QLabel::hide);
  m_text->show();
  m_text->setText(QStringLiteral("Seek backward"));
}

void videoplayer::seekForwardClicked() {
  qint64 value = m_mediaPlayer->duration();

  if (m_mediaPlayer->position() < value)
    value = m_mediaPlayer->position() + SEEK_STEP;

  m_mediaPlayer->setPosition(value);

  QTimer::singleShot(2000, m_text, &QLabel::hide);
  m_text->show();
  m_text->setText(QStringLiteral("Seek forward"));
}

auto videoplayer::volume() const -> int {
  qreal linearVolume = QAudio::convertVolume(
      cmnds->m_volumeSlider->value() / qreal(100),
      QAudio::LogarithmicVolumeScale, QAudio::LinearVolumeScale);
  return qRound(linearVolume * 100);
}

void videoplayer::setVolume(qint64 volume) {
  qreal logarithmicVolume =
      QAudio::convertVolume(volume / qreal(100), QAudio::LinearVolumeScale,
                            QAudio::LogarithmicVolumeScale);
  cmnds->m_volumeSlider->setValue(qRound(logarithmicVolume * 100));
}

void videoplayer::onVolumeSliderChanged() {
  if (m_mediaPlayer->volume() == 0) {
    cmnds->m_muteButton->setIcon(
        style()->standardIcon(QStyle::SP_MediaVolumeMuted));
  } else {
    cmnds->m_muteButton->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
  }
  m_mediaPlayer->setVolume(volume());
  QTimer::singleShot(2000, m_text, &QLabel::hide);
  m_text->show();
  m_text->setText(QStringLiteral("Volume changed"));
}

void videoplayer::exit() { std::exit(EXIT_SUCCESS); }

/*! Function that that checks for keyboard shortcuts */
void videoplayer::keyPressEvent(QKeyEvent *event) {
  if (event->key() == Qt::Key_Space)
    cmnds->m_playButton->click();
  else if (event->key() == Qt::Key_Plus)
    cmnds->m_volumeSlider->triggerAction(
        QAbstractSlider::SliderAction::SliderSingleStepAdd);
  else if (event->key() == Qt::Key_Minus)
    cmnds->m_volumeSlider->triggerAction(
        QAbstractSlider::SliderAction::SliderSingleStepSub);
  else if (event->key() == Qt::Key_F) {
    if (!isFullScreen()) {
      QTimer::singleShot(3000, cmnds, &commands::hideCommands);
      // cmnds->hideCommands();
      Playlist->m_playlist_entries->hide();
      m_menuBar->hide();
      this->layout()->setContentsMargins(0, 0, 0, 0);
      this->layout()->setMargin(0);
      setMouseTracking(true);
      showFullScreen();
    } else {
      this->layout()->setContentsMargins(-1, -1, -1, -1);
      showNormal();
      cmnds->showCommands();
      Playlist->m_playlist_entries->show();
      m_menuBar->show();
      setMouseTracking(false);
    }
  } else if (event->key() == Qt::Key_Period)
    cmnds->m_forwardButton->click();
  else if (event->key() == Qt::Key_Comma)
    cmnds->m_backwardButton->click();
}

void videoplayer::mouseDoubleClickEvent(QMouseEvent *event) {
  if (event->buttons() == Qt::MouseEventCreatedDoubleClick &&
      m_graphicsView->underMouse()) {
    if (!isFullScreen()) {
      cmnds->hideCommands();
      setMouseTracking(true);
      this->layout()->setContentsMargins(0, 0, 0, 0);
      // QTimer::singleShot(3000,cmnds,&commands::hideCommands);
      Playlist->m_playlist_entries->hide();
      m_menuBar->hide();
      showFullScreen();
      cmnds->m_playButton->click();
    } else if (isFullScreen()) {
      unsetCursor();
      setMouseTracking(false);
      this->layout()->setContentsMargins(-1, -1, -1, -1);
      showNormal();
      cmnds->showCommands();
      Playlist->m_playlist_entries->show();
      m_menuBar->show();
      cmnds->m_playButton->click();
    }
  }
}

void videoplayer::contextMenuEvent(QContextMenuEvent *event) {
  // need a implementation of context menu event so that the propagation of
  // events from annotations works

  if (m_graphicsView->underMouse())
    m_rightClickMenu->m_RCMenu->popup(QCursor::pos());
}

void videoplayer::hideMouse() {}

void videoplayer::mousePressEvent(QMouseEvent *event) {
  if (event->button() == Qt::RightButton && m_graphicsView->underMouse())
    m_rightClickMenu->m_RCMenu->popup(QCursor::pos());
}

void videoplayer::mouseMoveEvent(QMouseEvent *event) {
  // resetuj tajmer ako je postavljen
  // pokrenem skrivanje komandi i misha
  mouseHideTimer->start();
}

void videoplayer::wheelEvent(QWheelEvent *event) {
  if (event->angleDelta().y() < 0) {
    if (m_mediaPlayer->volume() >= VOLUME_STEP)
      m_mediaPlayer->setVolume(m_mediaPlayer->volume() - VOLUME_STEP);
    else
      m_mediaPlayer->setVolume(0);
  } else if (event->angleDelta().x() >= 0) {
    m_mediaPlayer->setVolume(m_mediaPlayer->volume() + VOLUME_STEP);
  }
  setVolume(m_mediaPlayer->volume());
  event->accept();
}

void videoplayer::addSubtitle() {
  QString fileName = QFileDialog::getOpenFileName(
      this, tr("Select a Subtitle"), QLatin1String(""), tr("*.srt"));
  if (fileName.isEmpty()) {
    return;
  }
  QFile file(fileName);
  if (!file.open(QIODevice::ReadOnly)) {
    QMessageBox::information(this, tr("Unable to open file"),
                             file.errorString());
    return;
  }
  subtitle->getSubtitles(file);
  subtitle->setAddedSubtitle(true);
}

void videoplayer::addAnnotation() {
  // TODO: uvecavamo broj anotacije pre nego sto ga sacuvamo
  numOfAnnotations++;
  if (numOfAnnotations > 30) {
    std::cerr << "Can't have more than 30 annotations" << std::endl;
    std::exit(EXIT_FAILURE);
  }
  QDialog popupAnnotationMenu = QDialog();
  popupAnnotationMenu.setMinimumSize(200, 200);
  popupAnnotationMenu.setGeometry(100, 100, 500, 270);

  popupAnnotationMenu.setParent(this);
  popupAnnotationMenu.setWindowFlags(Qt::CustomizeWindowHint |
                                     Qt::WindowStaysOnTopHint);
  popupAnnotationMenu.activateWindow();

  formLayout = new QFormLayout();
  formLayout->setFormAlignment(Qt::AlignTop);

  auto *nameLineEdit = new QLineEdit();
  auto *beginLineEdit = new QLineEdit();
  auto *durationLineEdit = new QLineEdit();
  auto *textLineEdit = new QLineEdit();
  auto *heightLineEdit = new QLineEdit();
  auto *widthLineEdit = new QLineEdit();
  nameLineEdit->setStyleSheet(QStringLiteral("background-color:#303030"));
  beginLineEdit->setStyleSheet(QStringLiteral("background-color:#303030"));
  durationLineEdit->setStyleSheet(QStringLiteral("background-color:#303030"));
  textLineEdit->setStyleSheet(QStringLiteral("background-color:#303030"));
  heightLineEdit->setStyleSheet(QStringLiteral("background-color:#303030"));
  widthLineEdit->setStyleSheet(QStringLiteral("background-color:#303030"));

  QDialogButtonBox formButtonBox = new QDialogButtonBox();
  formButtonBox.addButton(tr("Accept"), QDialogButtonBox::AcceptRole);
  formButtonBox.addButton(tr("Cancel"), QDialogButtonBox::RejectRole);

  formLayout->setSpacing(10);
  formLayout->setMargin(10);

  formLayout->addRow(tr("Name:"), nameLineEdit);
  formLayout->addRow(tr("Begin annotation at(hh:mm:ss):"), beginLineEdit);
  formLayout->addRow(tr("Duration of annotation(mm:ss):"), durationLineEdit);
  formLayout->addRow(tr("Heigth:"), heightLineEdit);
  formLayout->addRow(tr("Width:"), widthLineEdit);
  formLayout->addRow(tr("Text Content: "), textLineEdit);

  formLayout->setContentsMargins(10, 10, 10, 10);

  connect(&formButtonBox, &QDialogButtonBox::accepted, &popupAnnotationMenu,
          &QDialog::accept);
  connect(&formButtonBox, &QDialogButtonBox::rejected, &popupAnnotationMenu,
          &QDialog::reject);

  popupAnnotationMenu.setLayout(formLayout);
  formLayout->addWidget(&formButtonBox);
  popupAnnotationMenu.showNormal();
  popupAnnotationMenu.setFocus();
  // provera vrednosti treba i za opseg vremena, anotacija moze da traje
  // najduze od vremena kreiranja do zavrsetka videa
  bool okWidth = true;
  bool okHeight = true;

  qint64 defaultSize = 200;
  int currTime = cmnds->m_Slider->value();
  QTime currentTime((currTime / 3600) % 60, (currTime / 60) % 60, currTime % 60,
                    (currTime * 1000) % 1000);

  beginLineEdit->setText(currentTime.toString(QStringLiteral("hh:mm:ss")));
  durationLineEdit->setText(QStringLiteral("01:00"));
  widthLineEdit->setText(QStringLiteral("200"));
  heightLineEdit->setText(QStringLiteral("200"));

  if ((popupAnnotationMenu.exec() != 0) &&
      formButtonBox.AcceptRole == QDialogButtonBox::AcceptRole) {
    // regex provere
    if (!annotationFieldRegexCheck(widthLineEdit, heightLineEdit,
                                   durationLineEdit, beginLineEdit,
                                   textLineEdit))
      return;
    // ovde ide inicijalizacija sa unesenim poljima

    QString name = nameLineEdit->text();
    qint64 width = widthLineEdit->text().toInt(&okWidth, 10);
    qint64 height = heightLineEdit->text().toInt(&okHeight, 10);
    QString content = textLineEdit->text();
    QString beginAt = beginLineEdit->text();
    if (beginLineEdit->text().isEmpty()) {
      currentTime.toString(QStringLiteral("hh:mm:ss"));
    }
    QString annDuration = durationLineEdit->text();
    if (durationLineEdit->text().isEmpty()) {
      QString durationLabel = cmnds->m_durationInfo->text();
      QStringList durationList = durationLabel.split(QStringLiteral(" / "));
      annDuration = durationList[1];
    }
    QStringList times = beginAt.split(tr(":"));
    QStringList durations = annDuration.split(tr(":"));
    qint64 beginAnnotation = times[0].toInt() * 1000 * 60 * 60 +
                             times[1].toInt() * 1000 * 60 +
                             times[2].toInt() * 1000;
    qint64 durationTime =
        durations[0].toInt() * 1000 * 60 + durations[1].toInt() * 1000;

    if (!okWidth) {
      width = defaultSize;
    }

    if (!okHeight) {
      height = defaultSize;
    }

    const QRect screenGeometry = QApplication::desktop()->screenGeometry(this);

    if (height > screenGeometry.height()) {
      height = defaultSize;
    }
    if (width > screenGeometry.width()) {
      width = defaultSize;
    }

    QString durationLabel = cmnds->m_durationInfo->text();
    QStringList durationList = durationLabel.split(QStringLiteral(" / "));
    QStringList durVideoList = durationList[1].split(QStringLiteral(":"));
    qint64 dur = 0;
    if (durVideoList.length() == 2) {
      dur =
          durVideoList[0].toInt() * 1000 * 60 + durVideoList[1].toInt() * 1000;
    } else {
      dur = durVideoList[0].toInt() * 1000 * 60 * 60 +
            durVideoList[1].toInt() * 1000 * 60 +
            durVideoList[2].toInt() * 1000;
    }
    if (beginAnnotation + durationTime > dur) {
      durationTime = dur - beginAnnotation;
    }

    // TODO brisanje svih unosa u vektoru
    m_videoAnnotations.append(new Annotation(m_videoItem, name, width, height,
                                             content, beginAnnotation,
                                             durationTime));
  }
}

void videoplayer::playlistDoubleClickPlay() {
  Playlist->m_playlist->setCurrentIndex(Playlist->m_playlist_entries->row(
      Playlist->m_playlist_entries->currentItem()));
  QFileInfo fileInfo(
      Playlist->m_playlist->currentMedia().canonicalUrl().path());
  QString filename = fileInfo.fileName();
  this->setWindowTitle(filename.split('.')[0]);
}

void videoplayer::setAnnotationsFromJson() {
  QString filePath =
      m_mediaPlayer->currentMedia().canonicalUrl().path().split('/').last() +
      ".json";
  FILE *f = fopen(filePath.toStdString().c_str(), "r");
  if (f == nullptr)
    return;

  QFile file = QFile();

  file.open(f, QIODevice::ReadWrite | QIODevice::Text);

  QJsonParseError jsonParseError;
  QJsonDocument annotationDocument =
      QJsonDocument::fromJson(file.readAll(), &jsonParseError);
  file.close();

  QJsonArray annotationJsonArray = annotationDocument.array();
  m_videoAnnotations.reserve(annotationJsonArray.size());
  QJsonArray::Iterator jsonBegin = annotationJsonArray.begin();
  QJsonArray::Iterator jsonEnd = annotationJsonArray.end();
  for (; jsonBegin < jsonEnd; ++jsonBegin) {
    QJsonObject obj = jsonBegin->toObject();
    m_videoAnnotations.append(new Annotation(
        m_videoItem, obj.value(QStringLiteral("name")).toString(),
        obj.value(QStringLiteral("width")).toInt(),
        obj.value(QStringLiteral("height")).toInt(),
        obj.value(QStringLiteral("content")).toString(),
        obj.value(QStringLiteral("beginAt")).toInt(),
        obj.value(QStringLiteral("duration")).toInt()));
  }
}

void videoplayer::saveAnnotationsToJsonFile() {
  if (m_videoAnnotations.isEmpty())
    return;

  QJsonArray jsonArr = QJsonArray();
  for (auto anno : qAsConst(m_videoAnnotations)) {
    QJsonObject obj = QJsonObject();
    obj.insert(QStringLiteral("name"), anno->name());
    obj.insert(QStringLiteral("width"), anno->width());
    obj.insert(QStringLiteral("height"), anno->height());
    obj.insert(QStringLiteral("content"), anno->text_content());
    obj.insert(QStringLiteral("beginAt"), anno->appearance_time());
    obj.insert(QStringLiteral("duration"), anno->duration());
    jsonArr.append(obj);
  }

  QJsonDocument jsonDoc = QJsonDocument(jsonArr);
  QFile out = QFile(
      m_mediaPlayer->currentMedia().canonicalUrl().path().split('/').last() +
      ".json");

  out.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
  QByteArray jsonFormated = jsonDoc.toJson();
  int retVal =
      out.write(jsonFormated.toStdString().c_str(), jsonFormated.size());
  if (retVal == -1)
    std::exit(EXIT_FAILURE);
  out.close();
}

auto videoplayer::annotationFieldRegexCheck(QLineEdit *widthLineEdit,
                                            QLineEdit *heightLineEdit,
                                            QLineEdit *durationLineEdit,
                                            QLineEdit *beginLineEdit,
                                            QLineEdit *contentLineEdit)
    -> bool {
  // provera vreme pocetka anotacija
  auto *re =
      new QRegularExpression(QStringLiteral("[0-9]{2}:[0-9]{2}:[0-9]{2}"));

  if (!re->match(beginLineEdit->text()).hasMatch()) {
    std::cerr << "Invalid format for annotation start time"
              << "\n";
    beginLineEdit->setText(QLatin1String(""));
    return false;
  }

  // provera duzine trajanja
  re->setPattern(QStringLiteral("[0-9]{2}:[0-9]{2}"));

  if (!re->match(durationLineEdit->text()).hasMatch()) {
    std::cerr << "Invalid format for annotation duration"
              << "\n";
    durationLineEdit->setText(QLatin1String(""));
    return false;
  }
  // provera sirine i visine
  re->setPattern(QStringLiteral("[12]?[0-9]{2}"));

  if (!re->match(widthLineEdit->text()).hasMatch()) {
    std::cerr << "Invalid format for text width"
              << "\n";
    widthLineEdit->setText(QLatin1String(""));
    return false;
  }

  if (!re->match(heightLineEdit->text()).hasMatch()) {
    std::cerr << "Invalid format for text height";
    heightLineEdit->setText(QLatin1String(""));
    return false;
  }
  // Tekst
  re->setPattern(QStringLiteral("[a-zA-Z0-9]+"));

  if (!re->match(contentLineEdit->text()).hasMatch()) {
    std::cerr << "Annotation content can't be empty"
              << "\n";
    contentLineEdit->setText(QLatin1String(""));
    return false;
  }

  return true;
}

void videoplayer::aboutPlayer() {
  QMessageBox messBox;
  messBox.setText(QStringLiteral(
      "Dobro dosli u nas videoplayer!\nOvo su uputstva i precice koje mozete koristiti:\n \
                    -Volume increase/decrease: +/-\n\
                    -Mute: M\n\
                    -Pause/Play: Space\n\
                    -Seek Forward/Backward: Right/Left Arrow\n\
                    -Open file: CTRL + O\n\
                    -Moving through playlist: comma/dot\n\
                    -Fullscreen: F\n\
                    -Exit: CTRL + Q\n\
                    "));
  messBox.exec();
}

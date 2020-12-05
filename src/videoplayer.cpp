#include "../headers/videoplayer.h"
#include "ui_videoplayer.h"
#include <QtWidgets>
#include <cstdlib>

videoplayer::videoplayer(QWidget *parent)
    : QWidget(parent)
    {
        this->setStyleSheet("background-color:#222222");
        m_mediaPlayer = new QMediaPlayer(this, QMediaPlayer::VideoSurface);

        const QRect screenGeometry = QApplication::desktop()->screenGeometry(this); // Pristupamo QDesktopWidget klasi koja ima metod
       // screenGeometry koji moze da nam vrati duzinu sirinu ekrana kao i broj ekrana.
        m_playlist = new QMediaPlaylist();
        m_mediaPlayer->setPlaylist(m_playlist);

        m_videoItem = new QGraphicsVideoItem;
        m_videoItem->setSize(QSizeF(screenGeometry.width()/2, screenGeometry.height()));
        m_videoItem->setAspectRatioMode(Qt::AspectRatioMode::KeepAspectRatio);

        m_scene = new QGraphicsScene(this); //Pravljenje scene
        m_graphicsView = new QGraphicsView(m_scene); //Postavljanje pogleda na scenu
        m_graphicsView->setContentsMargins(0,0,0,0);
        m_scene->addItem(m_videoItem);// Dodavanje itema na scenu
        const QBrush *darkGrayColor = new QBrush(QColor(50,50,50));
        m_scene->setBackgroundBrush(*darkGrayColor);

        //menu bar creation
        this->createMenuBar();

        m_playButton = new QPushButton;
        m_playButton->setEnabled(false);
        m_playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        m_playButton->setFixedSize(screenGeometry.width()/18,screenGeometry.height()/20);

        m_stopButton= new QPushButton;
        m_stopButton->setEnabled(false);
        m_stopButton->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
        m_stopButton->setFixedSize(screenGeometry.width()/18,screenGeometry.height()/20);

        m_forwardButton= new QPushButton;
        m_forwardButton->setEnabled(false);
        m_forwardButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));
        m_forwardButton->setFixedSize(screenGeometry.width()/18,screenGeometry.height()/20);

        m_backwardButton= new QPushButton;
        m_backwardButton->setEnabled(false);
        m_backwardButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward));
        m_backwardButton->setFixedSize(screenGeometry.width()/18,screenGeometry.height()/20);

        m_seekForwardButton= new QPushButton;
        m_seekForwardButton->setEnabled(false);
        m_seekForwardButton->setIcon(style()->standardIcon(QStyle::SP_MediaSeekForward));
        m_seekForwardButton->setFixedSize(screenGeometry.width()/18,screenGeometry.height()/20);

        m_seekBackwardButton= new QPushButton;
        m_seekBackwardButton->setEnabled(false);
        m_seekBackwardButton->setIcon(style()->standardIcon(QStyle::SP_MediaSeekBackward));
        m_seekBackwardButton->setFixedSize(screenGeometry.width()/18,screenGeometry.height()/20);

        m_muteButton = new QPushButton(this);
        m_muteButton->setEnabled(false);
        m_muteButton->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
        m_muteButton->setFixedSize(30,30);

        m_Slider = new QSlider(Qt::Horizontal);
        m_Slider->setRange(0, m_mediaPlayer->duration()/1000);//range of slider from zero to length of media played
        m_Slider->setEnabled(false);

        m_volumeSlider = new QSlider(Qt::Horizontal);
        m_volumeSlider->setRange(0,100);
        m_volumeSlider->setFixedSize(screenGeometry.width()/18,50);
        m_volumeSlider->setValue(100);
        m_volumeSlider->setEnabled(false);

        m_durationInfo = new QLabel("00:00:00/00:00:00",this);
        m_durationInfo->setEnabled(false);
        m_durationInfo->setStyleSheet("color:rgb(255,255,255)");
        m_durationInfo->setAlignment(Qt::AlignCenter);
        m_durationInfo->setFixedSize(2*screenGeometry.width()/18,screenGeometry.height()/50);

        m_openButton = new QPushButton(tr("Open"));
        m_openButton->setStyleSheet("color:white");

        //Connection for different controls

        connect(m_volumeSlider,&QSlider::valueChanged,this,&videoplayer::onVolumeSliderChanged);
        connect(m_Slider, &QSlider::sliderMoved, this, &videoplayer::seek);
        connect(m_mediaPlayer, &QMediaPlayer::durationChanged, this, &videoplayer::durationChanged);
        connect(m_mediaPlayer, &QMediaPlayer::positionChanged, this, &videoplayer::positionChanged);
        connect(m_playButton,&QAbstractButton::clicked,this,&videoplayer::playClicked);
        connect(m_openButton , &QAbstractButton::clicked,this,&videoplayer::openFile);
        connect(m_muteButton, &QAbstractButton::clicked, this, &videoplayer::muteClicked);
        connect(m_forwardButton,&QAbstractButton::clicked, this, &videoplayer::forwardClicked);
        connect(m_backwardButton,&QAbstractButton::clicked, this, &videoplayer::backwardClicked);
        connect(m_seekForwardButton,&QAbstractButton::clicked, this, &videoplayer::seekForwardClicked);
        connect(m_seekBackwardButton,&QAbstractButton::clicked, this, &videoplayer::seekBackwardClicked);
        connect(m_stopButton,&QAbstractButton::clicked, this, &videoplayer::stopClicked);
        //when another video is loaded up, the native resolution of the video changes
        //so we monitor for that, so we can adjust the resolution accordingly
        connect(m_videoItem, &QGraphicsVideoItem::nativeSizeChanged, this, &videoplayer::calcVideoFactor);

        //horizontal box with buttons
        QHBoxLayout* commandsLayout = new QHBoxLayout();
        commandsLayout->setMargin(0);
        commandsLayout->addWidget(m_durationInfo);
        commandsLayout->addWidget(m_backwardButton);
        commandsLayout->addWidget(m_seekBackwardButton);
        commandsLayout->addWidget(m_playButton);
        commandsLayout->addWidget(m_stopButton);
        commandsLayout->addWidget(m_seekForwardButton);
        commandsLayout->addWidget(m_forwardButton);
        commandsLayout->addWidget(m_muteButton);
        commandsLayout->addWidget(m_volumeSlider);
        commandsLayout->setSpacing(0);

       //horizontal box with slider and open button
        QHBoxLayout* sliderLayout = new QHBoxLayout();
        sliderLayout->setMargin(0);
        sliderLayout->addWidget(m_Slider);
        sliderLayout->addWidget(m_openButton);
        sliderLayout->setSpacing(0);

        //vertical box with all elements
        QVBoxLayout* layout = new QVBoxLayout(this);
        layout->addWidget(m_menuBar);
        layout->addWidget(m_graphicsView);
        layout->addLayout(sliderLayout);
        layout->addLayout(commandsLayout);
        layout->setSpacing(0);

        m_rightClickMenu = new QMenu(this);
        m_rightClickMenu->addAction("Play/Pause",this, SLOT(playClicked()));
        m_rightClickMenu->addSeparator();
        m_rightClickMenu->addAction("Leave",this,SLOT(exit()));
        m_rightClickMenu->setHidden(true);

        m_mediaPlayer->setVideoOutput(m_videoItem);

        connect(m_mediaPlayer,&QMediaPlayer::stateChanged,this,&videoplayer::mediaStateChanged);
    }

videoplayer::~videoplayer(){}

bool videoplayer::isAvaliable() const{
    return m_mediaPlayer->isAvailable();
}

void videoplayer::positionChanged(qint64 progress){
    if (!m_Slider->isSliderDown())
        m_Slider->setValue(progress/1000);
    updateDurationInfo(progress/1000);
}

void videoplayer::updateDurationInfo(qint64 currInfo){
    QString tStr;
    if (currInfo || m_duration) {
        QTime currentTime((currInfo / 3600) % 60, (currInfo / 60) % 60,
            currInfo % 60, (currInfo * 1000) % 1000);
        QTime totalTime((m_duration / 3600) % 60, (m_duration / 60) % 60,
            m_duration % 60, (m_duration * 1000) % 1000);
        QString format = "mm:ss";
        if (m_duration > 3600)
            format = "hh:mm:ss";
        tStr = currentTime.toString(format) + " / " + totalTime.toString(format);
    }
    m_durationInfo->setText(tStr);
}


QMediaPlayer::State videoplayer::state() const{
    return m_playerState;
}

void videoplayer::durationChanged(qint64 duration){
    m_duration = duration / 1000;
    m_Slider->setMaximum(m_duration);
}

void videoplayer::mediaStateChanged(QMediaPlayer::State state){
    if(m_playerState != state){
        m_playerState = state;

        switch(state){
            case QMediaPlayer::PlayingState:
                m_playButton->setEnabled(true);
                m_muteButton->setEnabled(true);
                m_forwardButton->setEnabled(true); 
                m_backwardButton->setEnabled(true);
                m_seekForwardButton->setEnabled(true);
                m_seekBackwardButton->setEnabled(true);
                m_stopButton->setEnabled(true);
                m_Slider->setEnabled(true);
                m_muteButton->setEnabled(true);
                m_playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
                m_volumeSlider->setEnabled(true);
                m_durationInfo->setEnabled(true);
                break;
            case QMediaPlayer::StoppedState:
                m_playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
                break;
            case QMediaPlayer::PausedState:
                m_playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
                break;
            default:
                m_stopButton->setEnabled(false);
                m_playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
                break;
        }
    }
}

//this is to avoid a known bug, so should work ok on all platforms, and not just some
//https://bugreports.qt.io/browse/QTBUG-28850

void videoplayer::calcVideoFactor(QSizeF size){
     QRectF rect = QRectF(0,0,size.width(),size.height());
     m_videoItem->setSize(QSizeF(size.width(),size.height()));
     m_graphicsView->fitInView(rect, Qt::AspectRatioMode::KeepAspectRatio);
     m_graphicsView->setSceneRect(0,0,size.width(),size.height());
}
void videoplayer::showEvent(QShowEvent *){
    fitView();
}
void videoplayer::resizeEvent(QResizeEvent *){
    fitView();
}
void videoplayer::fitView(){
    const QRectF rect = m_graphicsView->rect();
    m_videoItem->setSize(QSizeF(rect.width(),rect.height()));
    m_graphicsView->fitInView(rect, Qt::AspectRatioMode::KeepAspectRatio);
    m_graphicsView->setSceneRect(rect);
}

//TODO implementation of a true playlist with lists of urls and loading it up in the playlist
void videoplayer::openFile(){

    QFileDialog fileDialog;
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setWindowTitle(tr("Open Files"));
    fileDialog.setDirectory(QStandardPaths::standardLocations(QStandardPaths::MoviesLocation).value(0, QDir::homePath()));
    fileDialog.setNameFilter(tr("Video File(*.mkv *.mp4 *.avi)"));
    fileDialog.setFileMode(QFileDialog::ExistingFiles);
    fileDialog.show();

    if(fileDialog.exec())
        loadPlaylist(fileDialog.selectedUrls());
}

void videoplayer::loadPlaylist(QList<QUrl> urls){
    for (auto url : urls){
        m_playlist->addMedia(url);
    }
    m_playlist->setCurrentIndex(1);
    m_playlist->setPlaybackMode(QMediaPlaylist::Sequential);
    m_graphicsView->setFocus();
    m_mediaPlayer->play();
}

void videoplayer::playClicked(){
    switch (m_playerState) {
        case QMediaPlayer::StoppedState:
        case QMediaPlayer::PausedState:
            //with emit doesnt work on Ubuntu
            //emit play();
            m_mediaPlayer->play();
            QTimer::singleShot(2000, m_text, &QLabel::hide);
            m_text->show();
            m_text->setText("Playing");

            break;
        case QMediaPlayer::PlayingState:
            //emit pause();
            m_mediaPlayer->pause();
            m_text->show();
            m_text->setText("Paused");
            QTimer::singleShot(2000, m_text, &QLabel::hide);

            break;
        }
}

bool videoplayer::isMuted() const{
    return m_playerMuted;
}

void videoplayer::setMuted(bool muted){
    if (muted != m_playerMuted) {
        m_playerMuted = muted;
    }
}

void videoplayer::volumeIncrease(){
    m_mediaPlayer->setVolume(m_mediaPlayer->volume() + 5);
}

void videoplayer::volumeDecrease(){
    m_mediaPlayer->setVolume(m_mediaPlayer->volume() - 5);
}

void videoplayer::seek(int seconds){
    m_mediaPlayer->setPosition(seconds * 1000);
}
//for some reason, nothing with emit works on Ubuntu,
//hence the commented out lines of code, might work just fine on other platforms
void videoplayer::muteClicked(){
    if(m_mediaPlayer->isMuted()==false){
        m_mediaPlayer->setMuted(true);
        m_muteButton->setIcon(style()->standardIcon(QStyle::SP_MediaVolumeMuted));
        QTimer::singleShot(2000, m_text, &QLabel::hide);
        m_text->show();
        m_text->setText("Muted");
    }
    else{
        m_mediaPlayer->setMuted(false);
        m_muteButton->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
        QTimer::singleShot(2000, m_text, &QLabel::hide);
        m_text->show();
        m_text->setText("Unmuted");
    }
}
void videoplayer::createMenuBar(){
    m_menuBar = new QMenuBar();
    m_menuBar->setStyleSheet("color:white");
    m_text = new QLabel();
    m_text->setStyleSheet("background-color:rgba(255,255,255,0)");
    m_text->setFixedSize(150,20);
    m_text->hide();
    m_menuBar->setCornerWidget(m_text);
    QMenu *file = new QMenu("File");
    QMenu *edit = new QMenu("Edit");
    QMenu *audio = new QMenu("Audio");
    QMenu *video = new QMenu("Video");
    QMenu *help = new QMenu("Help");
    //file padajuci meni
    QAction* openFile = file->addAction("Open file");
    QAction* openFolder = file->addAction("Open folder");
    QAction* exit = file->addAction("Abort"); // iz nekog razloga ne izbacuje EXIT akciju
    file->setStyleSheet(" QMenu {Background-color: #222222;color:white;}");

    //edit padajuci meni
    QAction* copy = edit->addAction("Copy");
    QAction* cut = edit->addAction("Cut");
    QAction* paste = edit->addAction("Paste");
    QAction* del = edit->addAction("Delete");
    QAction* selectAll = edit->addAction("Select all");
    edit->setStyleSheet(" QMenu {Background-color: #222222;color:white; }");
    //audio padajuci meni
    QAction* incVol = audio->addAction("Increase volume");
    QAction* decVol = audio->addAction("Decrease volume");
    QAction* mute = audio->addAction("Mute");
    audio->setStyleSheet(" QMenu {Background-color: #222222;color:white;}");
    //video padajuci meni
    QAction* incBrightness = video->addAction("Brightness increase");
    QAction* decBrightness = video->addAction("Brightness decrease");
    QAction* incContrast = video->addAction("Contrast increase");
    QAction* decContrast = video->addAction("Contrast decrease");
    video->setStyleSheet(" QMenu {Background-color: #222222;color:white;}");
    //help padajuci meni
    help->addAction("Licence");
    help->setStyleSheet(" QMenu {Background-color: #222222;color:white;}");
    //povezivanje akcija sa funkcijama pri kliku
    connect(openFile, &QAction::triggered, this, &videoplayer::openFile);
    connect(exit, &QAction::triggered, this, &videoplayer::exit);
    connect(incVol, &QAction::triggered, this, &videoplayer::volumeIncrease);
    connect(decVol, &QAction::triggered, this, &videoplayer::volumeDecrease);
    connect(mute, &QAction::triggered, this, &videoplayer::muteClicked);
    //TODO: ostale funkcije za rad
    //precice na tastaturi
    openFile->setShortcut(QKeySequence::Open); // CTRL + O
    openFolder->setShortcut(Qt::CTRL + Qt::Key_F);
    exit->setShortcut(Qt::CTRL + Qt::Key_Q);
    copy->setShortcut(QKeySequence::Copy);
    paste->setShortcut(QKeySequence::Paste);
    del->setShortcut(QKeySequence::Delete);
    cut->setShortcut(QKeySequence::Cut);
    selectAll->setShortcut(Qt::CTRL + Qt::Key_A);
    //incVol->setShortcut(Qt::Key_Plus);
    //decVol->setShortcut(Qt::Key_Minus);
    mute->setShortcut(Qt::Key_M);
    incBrightness->setShortcut(Qt::SHIFT + Qt::Key_B);
    decBrightness->setShortcut(Qt::Key_B);
    incContrast->setShortcut(Qt::SHIFT + Qt::Key_C);
    decContrast->setShortcut(Qt::Key_C);

    //povezivanje menija u jedan meni bar
    m_menuBar->addMenu(file);
    m_menuBar->addMenu(edit);
    m_menuBar->addMenu(audio);
    m_menuBar->addMenu(video);
    m_menuBar->addMenu(help);
}
//Ostale funckije potrebne pri konekciji

void videoplayer::stopClicked(){
    m_mediaPlayer->stop();
    QTimer::singleShot(2000, m_text, &QLabel::hide);
    m_text->show();
    m_text->setText("Stopped");
    //emit stop();
}

void videoplayer::forwardClicked(){
    m_playlist->next();
    QTimer::singleShot(2000, m_text, &QLabel::hide);
    m_text->show();
    m_text->setText("Forward");
}

void videoplayer::backwardClicked(){
    m_playlist->previous();
    QTimer::singleShot(2000, m_text, &QLabel::hide);
    m_text->show();
    m_text->setText("Backward");
}
//Not all playback services support change of the playback rate.
//It is framework defined as to the status and quality of audio and video
//while fast forwarding or rewinding.
void videoplayer::seekBackwardClicked(){
    qreal value;
    if (m_mediaPlayer->playbackRate()==0.01)
        value = 0.0 + PLAYBACK_STEP;
    else
        value = m_mediaPlayer->playbackRate()-PLAYBACK_STEP;
    //because we cant divide by zero the value of zero is not zero, so it can playback in reverse :D
    //otherwise Gstreamer throws an exception
    if (value==0)
        m_mediaPlayer->setPlaybackRate(0.01);
    else if (value >= (-MAX_PLAYBACK_RATE))
        m_mediaPlayer->setPlaybackRate(value);
    QTimer::singleShot(2000, m_text, &QLabel::hide);
    m_text->show();
    m_text->setText("Seek backward");
}
void videoplayer::seekForwardClicked(){
    qreal value;
    if (m_mediaPlayer->playbackRate()==0.01)
        value = 0.0 + PLAYBACK_STEP;
    else
        value = m_mediaPlayer->playbackRate()+PLAYBACK_STEP;

    if (value==0)
        m_mediaPlayer->setPlaybackRate(0.01);
    else if (value <= (MAX_PLAYBACK_RATE))
        m_mediaPlayer->setPlaybackRate(value);
    QTimer::singleShot(2000, m_text, &QLabel::hide);
    m_text->show();
    m_text->setText("Seek forward");
}

int videoplayer::volume() const{
    qreal linearVolume =  QAudio::convertVolume(m_volumeSlider->value() / qreal(100),
                                                    QAudio::LogarithmicVolumeScale,
                                                    QAudio::LinearVolumeScale);

    return qRound(linearVolume * 100);
}

void videoplayer::setVolume(qint64 volume){
    qreal logarithmicVolume = QAudio::convertVolume(volume / qreal(100),
                                                    QAudio::LinearVolumeScale,
                                                    QAudio::LogarithmicVolumeScale);

    m_volumeSlider->setValue(qRound(logarithmicVolume * 100));
}

void videoplayer::onVolumeSliderChanged(){
    if(m_mediaPlayer->volume() == 0){
        m_muteButton->setIcon(style()->standardIcon(QStyle::SP_MediaVolumeMuted));
    }
    else{
        m_muteButton->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));

    }
    m_mediaPlayer->setVolume(volume());
    QTimer::singleShot(2000, m_text, &QLabel::hide);
    m_text->show();
    m_text->setText("Volume changed");
}

void videoplayer::exit(){
    std::exit(EXIT_SUCCESS);
}

void videoplayer::keyPressEvent(QKeyEvent *event){
    if(event->key() == Qt::Key_Space)
        m_playButton->click();

    else if(event->key()==Qt::Key_Plus)
        m_volumeSlider->triggerAction(QAbstractSlider::SliderAction::SliderSingleStepAdd);

    else if(event->key()==Qt::Key_Minus)
        m_volumeSlider->triggerAction(QAbstractSlider::SliderAction::SliderSingleStepSub);

    else if(event->key()==Qt::Key_F){
        if(!isFullScreen()){

            m_playButton->hide();
            m_muteButton->hide();
            m_forwardButton->hide();
            m_seekForwardButton->hide();
            m_backwardButton->hide();
            m_seekBackwardButton->hide();
            m_stopButton->hide();
            m_Slider->hide();
            m_muteButton->hide();
            m_playButton->hide();
            m_menuBar->hide();
            m_openButton->hide();
            m_volumeSlider->hide();
            m_durationInfo->hide();
            this->layout()->setContentsMargins(0,0,0,0);
            this->layout()->setMargin(0);
            showFullScreen();
        }
        else{
            this->layout()->setContentsMargins(-1,-1,-1,-1);
            showNormal();
            m_playButton->show();
            m_muteButton->show();
            m_forwardButton->show();
            m_seekForwardButton->show();
            m_backwardButton->show();
            m_seekBackwardButton->show();
            m_stopButton->show();
            m_Slider->show();
            m_muteButton->show();
            m_playButton->show();
            m_menuBar->show();
            m_openButton->show();
            m_volumeSlider->show();
            m_durationInfo->show();
        }
    }

    else if(event->key()==Qt::Key_Period)
        m_forwardButton->click();

    else if(event->key()==Qt::Key_Comma)
        m_backwardButton->click();
}

void videoplayer::mouseDoubleClickEvent(QMouseEvent *event){
    if(event->buttons()==Qt::MouseEventCreatedDoubleClick &&\
            m_graphicsView->underMouse()){
        if(!isFullScreen()){
            m_playButton->hide();
            m_muteButton->hide();
            m_forwardButton->hide();
            m_backwardButton->hide();
            m_seekForwardButton->hide();
            m_seekBackwardButton->hide();
            m_stopButton->hide();
            m_Slider->hide();
            m_muteButton->hide();
            m_playButton->hide();
            m_menuBar->hide();
            m_openButton->hide();
            m_volumeSlider->hide();
            m_durationInfo->hide();

            this->layout()->setContentsMargins(0,0,0,0);
            showFullScreen();
            m_playButton->click();
        }
        else if(isFullScreen()){
            this->layout()->setContentsMargins(-1,-1,-1,-1);
            showNormal();
            m_playButton->show();
            m_muteButton->show();
            m_forwardButton->show();
            m_backwardButton->show();
            m_seekForwardButton->show();
            m_seekBackwardButton->show();
            m_stopButton->show();
            m_Slider->show();
            m_muteButton->show();
            m_playButton->show();
            m_menuBar->show();
            m_openButton->show();
            m_volumeSlider->show();
            m_durationInfo->show();
            m_playButton->click();
          }
    }
}

void videoplayer::mousePressEvent(QMouseEvent *event){
    if(event->button()==Qt::LeftButton && m_graphicsView->underMouse())
        m_playButton->click();
    else if(event->button()==Qt::RightButton && m_graphicsView->underMouse())
        m_rightClickMenu->popup(QCursor::pos());
}

#include "../headers/videoplayer.h"
#include "ui_videoplayer.h"
#include <QtWidgets>
#include <QGraphicsVideoItem>
videoplayer::videoplayer(QWidget *parent)
    : QWidget(parent)
    {
        m_mediaPlayer = new QMediaPlayer(this, QMediaPlayer::VideoSurface);
        const QRect screenGeometry = QApplication::desktop()->screenGeometry(this); // Pristupamo QDesktopWidget klasi koja ima metod
       // screenGeometry koji moze da nam vrati duzinu sirinu ekrana kao i broj ekrana.
        m_playlist = new QMediaPlaylist();
        m_mediaPlayer->setPlaylist(m_playlist);

        m_videoItem = new QGraphicsVideoItem;
        m_videoItem->setSize(QSizeF(screenGeometry.width()/3, screenGeometry.height()/2));

        QGraphicsScene* scene = new QGraphicsScene(this); //Pravljenje scene
        QGraphicsView* graphicsView = new QGraphicsView(scene); //Postavljanje pogleda na scenu

        scene->addItem(m_videoItem);// Dodavanje itema na scenu
        //menu bar creation
        this->createMenuBar();

        m_playButton = new QPushButton;
        m_playButton->setEnabled(false);
        m_playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        m_playButton->setFixedSize(30,30);

        m_stopButton= new QPushButton;
        m_stopButton->setEnabled(false);
        m_stopButton->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
        m_stopButton->setFixedSize(30,30);

        m_forwardButton= new QPushButton;
        m_forwardButton->setEnabled(false);
        m_forwardButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));
        m_forwardButton->setFixedSize(30,30);

        m_backwardButton= new QPushButton;
        m_backwardButton->setEnabled(false);
        m_backwardButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward));
        m_backwardButton->setFixedSize(30,30);

        m_muteButton = new QPushButton(this);
        m_muteButton->setEnabled(false);
        m_muteButton->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
        m_muteButton->setFixedSize(30,30);

        m_Slider = new QSlider(Qt::Horizontal);
        //range of slider from zero to length of media played
        m_Slider->setRange(0, m_mediaPlayer->duration()/1000);
        m_Slider->setEnabled(false);

        QAbstractButton *openButton = new QPushButton(tr("Open"));

        //Connection for different controls
        connect(m_Slider, &QSlider::sliderMoved, this, &videoplayer::seek);
        connect(m_mediaPlayer, &QMediaPlayer::durationChanged, this, &videoplayer::durationChanged);
        connect(m_mediaPlayer, &QMediaPlayer::positionChanged, this, &videoplayer::positionChanged);
        connect(m_playButton,&QAbstractButton::clicked,this,&videoplayer::playClicked);
        connect(openButton , &QAbstractButton::clicked,this,&videoplayer::openFile);
        connect(m_muteButton, &QAbstractButton::clicked, this, &videoplayer::muteClicked);
        connect(m_forwardButton,&QAbstractButton::clicked, this, &videoplayer::forwardClicked);
        connect(m_backwardButton,&QAbstractButton::clicked, this, &videoplayer::backwardClicked);
        connect(m_stopButton,&QAbstractButton::clicked, this, &videoplayer::stopClicked);
        //when another video is loaded up, the native resolution of the video changes
        //so we monitor for that, so we can adjust the resolution accordingly
        connect(m_videoItem, &QGraphicsVideoItem::nativeSizeChanged, this, &videoplayer::calcVideoFactor);
        QHBoxLayout* commandsLayout = new QHBoxLayout();
        commandsLayout->setMargin(0);
        commandsLayout->addWidget(m_backwardButton);
        commandsLayout->addWidget(m_playButton);
        commandsLayout->addWidget(m_stopButton);
        commandsLayout->addWidget(m_forwardButton);
        commandsLayout->addWidget(m_muteButton);
        commandsLayout->addWidget(m_Slider);
        commandsLayout->addWidget(openButton);


        QVBoxLayout* layout = new QVBoxLayout(this);
        layout->addWidget(m_menuBar);
        layout->addWidget(graphicsView);
        layout->addLayout(commandsLayout);


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
}

QMediaPlayer::State videoplayer::state() const
{
    return m_playerState;
}

void videoplayer::durationChanged(qint64 duration)
{
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
                m_stopButton->setEnabled(true);
                m_Slider->setEnabled(true);
                m_muteButton->setEnabled(true);
                m_playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
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
     m_videoItem->setSize(QSizeF(size.width(),size.height()));
}
//TODO implementation of a true playlist with lists of urls and loading it up in the playlist
//TODO aditional checks for supported files only to appear in file dialog
void videoplayer::openFile(){
    QFileDialog dialog(this);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setWindowTitle(tr("Open Files"));
    dialog.setDirectory(QStandardPaths::standardLocations(QStandardPaths::MoviesLocation).value(0, QDir::homePath()));

    if (dialog.exec() == QDialog::Accepted){
        //we make a dummy playlist , with one video
        //for development and testing purposes

        m_playlist->addMedia(QFileDialog::getOpenFileUrl());
        m_playlist->setCurrentIndex(1);
        m_mediaPlayer->play();
    }
}

void videoplayer::playClicked(){
    switch (m_playerState) {
        case QMediaPlayer::StoppedState:
        case QMediaPlayer::PausedState:
            //with emit doesnt work on Ubuntu
            //emit play();
            m_mediaPlayer->play();
            break;
        case QMediaPlayer::PlayingState:
            //emit pause();
            m_mediaPlayer->pause();
            break;
        }
}

bool videoplayer::isMuted() const
{
    return m_playerMuted;
}

void videoplayer::setMuted(bool muted)
{
    if (muted != m_playerMuted) {
        m_playerMuted = muted;

        m_muteButton->setIcon(style()->standardIcon(muted
                ? QStyle::SP_MediaVolumeMuted
                : QStyle::SP_MediaVolume));
    }
}

void videoplayer::seek(int seconds)
{
    m_mediaPlayer->setPosition(seconds * 1000);
}
//for some reason, nothing with emit works on Ubuntu,
//hence the commented out lines of code, might work just fine on other platforms
void videoplayer::muteClicked()
{
    if(m_mediaPlayer->isMuted()==false){
        m_mediaPlayer->setMuted(true);
    }
    else{
        m_mediaPlayer->setMuted(false);
    }
}
void videoplayer::createMenuBar(){
    m_menuBar = new QMenuBar();
    QMenu *file = new QMenu("File");
    QMenu *edit = new QMenu("Edit");
    QMenu *audio = new QMenu("Audio");
    QMenu *video = new QMenu("Video");
    QMenu *help = new QMenu("Help");
    //file padajuci meni
    file->addAction("Open file");
    file->addAction("Open folder");
    file->addAction("Exit");
    //edit padajuci meni
    edit->addAction("Copy");
    edit->addAction("Cut");
    edit->addAction("Paste");
    edit->addAction("Delete");
    edit->addAction("Select all");
    //audio padajuci meni
    audio->addAction("Increase volume");
    audio->addAction("Decrease volume");
    audio->addAction("Mute");
    //video padajuci meni
    video->addAction("Brightness increase");
    video->addAction("Brightness decrease");
    video->addAction("Contrast increase");
    video->addAction("Contrast decrease");
    //help padajuci meni
    help->addAction("Licence");
    //povezivanje menija u jedan meni bar
    m_menuBar->addMenu(file);
    m_menuBar->addMenu(edit);
    m_menuBar->addMenu(audio);
    m_menuBar->addMenu(help);
}
//Ostale funckije potrebne pri konekciji

void videoplayer::stopClicked()
{
    m_mediaPlayer->stop();
    //emit stop();
}
//Not all playback services support change of the playback rate.
//It is framework defined as to the status and quality of audio and video
//while fast forwarding or rewinding.
void videoplayer::forwardClicked()
{
    qreal value;
    if (m_mediaPlayer->playbackRate()==0.01)
        value = 0.0 + PLAYBACK_STEP;
    else
        value = m_mediaPlayer->playbackRate()+PLAYBACK_STEP;

    if (value==0)
        m_mediaPlayer->setPlaybackRate(0.01);
    else if (value <= (MAX_PLAYBACK_RATE))
        m_mediaPlayer->setPlaybackRate(value);
}

void videoplayer::backwardClicked()
{
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
}

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
        m_videoItem = new QGraphicsVideoItem;
        m_videoItem->setSize(QSizeF(screenGeometry.width()/3, screenGeometry.height()/2));

        QGraphicsScene* scene = new QGraphicsScene(this); //Pravljenje scene
        QGraphicsView* graphicsView = new QGraphicsView(scene); //Postavljanje pogleda na scenu

        scene->addItem(m_videoItem);// Dodavanje itema na scenu
        //menu bar
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
        m_Slider->setRange(0, 0);

        QAbstractButton *openButton = new QPushButton(tr("Open"));

        //Connection for open, play and mute buttons
        // TODO - conncetions for stop,forward, backward
        connect(m_playButton,&QAbstractButton::clicked,this,&videoplayer::playClicked);
        connect(openButton,&QAbstractButton::clicked,this,&videoplayer::openFile);
        connect(m_muteButton, &QAbstractButton::clicked, this, &videoplayer::muteClicked);


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
        layout->addWidget(graphicsView);
        layout->addLayout(commandsLayout);


        m_mediaPlayer->setVideoOutput(m_videoItem);

        connect(m_mediaPlayer,&QMediaPlayer::stateChanged,this,&videoplayer::mediaStateChanged);
    }

videoplayer::~videoplayer(){}

bool videoplayer::isAvaliable() const{
    return m_mediaPlayer->isAvailable();
}

QMediaPlayer::State videoplayer::state() const
{
    return m_playerState;
}

void videoplayer::mediaStateChanged(QMediaPlayer::State state){
    if(m_playerState != state){
        m_playerState = state;

        switch(state){
            case QMediaPlayer::PlayingState:
                m_stopButton->setEnabled(true);
                m_playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
                break;
            case QMediaPlayer::StoppedState:
                m_stopButton->setEnabled(false);
                m_playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
                break;
            case QMediaPlayer::PausedState:
                m_stopButton->setEnabled(true);
                m_playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
                break;
            default:
                 m_playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
                 break;
        }
    }
}

void videoplayer::openFile(){
    //TO DO
}

void videoplayer::playClicked(){
    switch (m_playerState) {
        case QMediaPlayer::StoppedState:
        case QMediaPlayer::PausedState:
            emit play();
            break;
        case QMediaPlayer::PlayingState:
            emit pause();
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

void videoplayer::muteClicked(){
    emit changeMuting(!m_playerMuted);
}

//Ostale funckije potrebne pri konekciji

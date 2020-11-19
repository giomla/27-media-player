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

        m_Slider = new QSlider(Qt::Horizontal);
        m_Slider->setRange(0, 0);

        QAbstractButton *openButton = new QPushButton(tr("Open"));

        //TODO Connection za playButton, Slider i openButton
        connect(m_playButton,&QAbstractButton::clicked,this,&videoplayer::playOnClick);
        connect(openButton,&QAbstractButton::clicked,this,&videoplayer::openFile);

        QHBoxLayout* commandsLayout = new QHBoxLayout();
        commandsLayout->setMargin(1);
        commandsLayout->addWidget(m_backwardButton);
        commandsLayout->addWidget(m_playButton);
        commandsLayout->addWidget(m_stopButton);
        commandsLayout->addWidget(m_forwardButton);
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

void videoplayer::mediaStateChanged(QMediaPlayer::State state){
    switch(state){
        case QMediaPlayer::PlayingState:
	  m_playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
            break;
        default:
	  m_playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
            break;
    }
}

void videoplayer::openFile(){
    //TO DO
}

void videoplayer::playOnClick(){
    //TO DO
}

//Ostale funckije potrebne pri konekciji

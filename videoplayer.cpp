#include "videoplayer.h"
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

        m_Slider = new QSlider(Qt::Horizontal);
        m_Slider->setRange(0, 0);

        QAbstractButton *openButton = new QPushButton(tr("Otvori Fajl"));

        //TODO Connection za playButton, Slider i openButton

        QHBoxLayout* commandsLayout = new QHBoxLayout();
        commandsLayout->setMargin(0);
        commandsLayout->addWidget(m_playButton);
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

void videoplayer::play(){
    //TO DO
}

//Ostale funckije potrebne pri konekciji

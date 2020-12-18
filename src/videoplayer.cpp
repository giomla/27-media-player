#include "../headers/videoplayer.h"
#include "ui_videoplayer.h"
#include <QtWidgets>
#include <cstdlib>
#include <iostream>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <iostream>

videoplayer::videoplayer(QWidget *parent)
    : QWidget(parent)
{

    const QRect screenGeometry = QApplication::desktop()->screenGeometry(this); // Pristupamo QDesktopWidget klasi koja ima metod

    m_videoItem = new QGraphicsVideoItem;
    m_videoItem->setSize(QSizeF(screenGeometry.width()/2, screenGeometry.height()));
    m_videoItem->setAspectRatioMode(Qt::AspectRatioMode::KeepAspectRatio);

    m_playlist = new QMediaPlaylist();

    m_mediaPlayer = new QMediaPlayer(this, QMediaPlayer::VideoSurface);
    m_mediaPlayer->setPlaylist(m_playlist);
    m_mediaPlayer->setVideoOutput(m_videoItem);

    darkGrayColor = new QBrush(QColor(50,50,50));

    m_scene = new QGraphicsScene(this);
    m_scene->addItem(m_videoItem);
    m_scene->setBackgroundBrush(*darkGrayColor);

    m_graphicsView = new QGraphicsView(m_scene);
    m_graphicsView->setContentsMargins(0,0,0,0);
    m_graphicsView->setAcceptDrops(false);
    m_graphicsView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate	);
    subtitle = new subtitles();
    cmnds = new commands(this);

    subtitleText = new QGraphicsTextItem(m_videoItem);
    subtitleText->setDefaultTextColor(QColor("white"));

    this->createMenuBar();

    this->setStyleSheet("background-color:#222222");
    this->setAcceptDrops(true);

    cmnds->m_Slider->setRange(0, m_mediaPlayer->duration()/1000);//range of slider from zero to length of media played
    cmnds->createSliderLayout();
    cmnds->createCommandLayout();

    //vertical box with all elements
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->addWidget(m_menuBar);
    layout->addWidget(m_graphicsView);
    layout->addLayout(cmnds->sliderLayout);
    layout->addLayout(cmnds->commandsLayout);
    layout->addWidget(cmnds->m_playlist_entries);
    layout->setSpacing(0);

   //TO DO MENU CLASS
    m_rightClickMenu = new QMenu(this);
    m_rightClickMenu->addAction("Play/Pause",this, SLOT(playClicked()));
    m_rightClickMenu->addSeparator();
    m_rightClickMenu->addAction("Leave",this,SLOT(exit()));
    m_rightClickMenu->setStyleSheet("color: white");
    m_rightClickMenu->setHidden(true);

    addSubtitles = m_rightClickMenu->addAction("Add Subtitle");

    //adding annotations
    QAction* addAnnotations = m_rightClickMenu->addAction("Add Annotation");
    connect (addAnnotations,&QAction::triggered, this, &videoplayer::addAnnotation);

    this->connections();
}

videoplayer::~videoplayer(){
   delete subtitle;
   delete darkGrayColor;
   delete m_playlist;
   delete m_mediaPlayer;
   for(auto it: m_videoAnnotations){
       delete it;
   }
}

bool videoplayer::isAvaliable() const{

    return m_mediaPlayer->isAvailable();
}

void videoplayer::connections(){

    connect(addSubtitles,&QAction::triggered,this,&videoplayer::addSubtitle);
    connect(m_mediaPlayer,&QMediaPlayer::stateChanged,this,&videoplayer::mediaStateChanged);
    connect(cmnds->m_volumeSlider,&QSlider::valueChanged,this,&videoplayer::onVolumeSliderChanged);
    connect(cmnds->m_Slider, &QSlider::sliderMoved, this, &videoplayer::seek);
    connect(m_mediaPlayer, &QMediaPlayer::durationChanged, this, &videoplayer::durationChanged);
    connect(m_mediaPlayer, &QMediaPlayer::positionChanged, this, &videoplayer::positionChanged);
    connect(cmnds->m_playButton,&QAbstractButton::clicked,this,&videoplayer::playClicked);
    connect(cmnds->m_openButton , &QAbstractButton::clicked,this,&videoplayer::openFile);
    connect(cmnds->m_muteButton, &QAbstractButton::clicked, this, &videoplayer::muteClicked);
    connect(cmnds->m_forwardButton,&QAbstractButton::clicked, this, &videoplayer::forwardClicked);
    connect(cmnds->m_backwardButton,&QAbstractButton::clicked, this, &videoplayer::backwardClicked);
    connect(cmnds->m_seekForwardButton,&QAbstractButton::clicked, this, &videoplayer::seekForwardClicked);
    connect(cmnds->m_seekBackwardButton,&QAbstractButton::clicked, this, &videoplayer::seekBackwardClicked);
    connect(cmnds->m_stopButton,&QAbstractButton::clicked, this, &videoplayer::stopClicked);
    connect(m_videoItem, &QGraphicsVideoItem::nativeSizeChanged, this, &videoplayer::calcVideoFactor);
    connect(cmnds->m_playlist_entries, &QListWidget::doubleClicked, this, &videoplayer::playlistDoubleClickPlay);
    //when another video is loaded up, the native resolution of the video changes
    //so we monitor for that, so we can adjust the resolution accordingly

}

void videoplayer::positionChanged(qint64 progress){

    if (!cmnds->m_Slider->isSliderDown())
        cmnds->m_Slider->setValue(progress/1000);
    //update annotations for video, go through the vector and update for each accordingly
    if(!m_videoAnnotations.empty()){
        for(auto it : m_videoAnnotations){
            it->setCurrTimeOfVideo(progress);
            it->update();
        }
    }
    updateDurationInfo(progress/1000);
}


void videoplayer::durationChanged(qint64 duration){

    m_duration = duration / 1000;
    cmnds->m_Slider->setMaximum(m_duration);
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

        if(subtitle->isAddedSubtitle()){
            for(auto &sub : subtitle->subs){
                if(currentTime.toString("hh:mm:ss") == sub.getBeginTime()){
                    subtitleText->setHtml("<p align='center'>" + sub.getLine());
                    subtitleText->show();
                }
                if(currentTime.toString("hh:mm:ss") == sub.getEndTime()){
                    subtitleText->hide();
                }
            }
        }
        tStr = currentTime.toString(format) + " / " + totalTime.toString(format);
        cmnds->m_durationInfo->setText(tStr);
    }
}

QMediaPlayer::State videoplayer::state() const{

    return m_playerState;
}



void videoplayer::mediaStateChanged(QMediaPlayer::State state){

    if(m_playerState != state){
        m_playerState = state;

        switch(state){
            case QMediaPlayer::PlayingState:
                cmnds->enableAllCommands();
                cmnds->m_playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
                break;
            case QMediaPlayer::StoppedState:
                cmnds->m_playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
                break;
            case QMediaPlayer::PausedState:
                cmnds->m_playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
                break;
            default:
                cmnds->m_stopButton->setEnabled(false);
                cmnds->m_playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
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
        fitView();
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
    for(auto it : m_videoAnnotations)
        it->resizeOccured();

    if(isMaximized()){
        subtitleText->setFont(QFont("Times",25));
        subtitleText->setTextWidth(rect.width()/2);
        subtitleText->setPos(rect.width()/4,rect.height()-110);

    }else if(isFullScreen()){
        subtitleText->setPos(rect.width()/4,rect.height()-150);
        subtitleText->setFont(QFont("Times",35));
        subtitleText->setTextWidth(rect.width()/2);

    }else{
        subtitleText->setPos(rect.width()/4,rect.height()-70);
        subtitleText->setFont(QFont("Times",20));
        subtitleText->setTextWidth(rect.width()/2);

    }
}

void videoplayer::openFile(){

    QFileDialog fileDialog;
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setWindowTitle(tr("Open Files"));
    fileDialog.setDirectory(QStandardPaths::standardLocations(QStandardPaths::MoviesLocation).value(0, QDir::homePath()));
    fileDialog.setNameFilter(tr("Video File(*.mkv *.mp4 *.avi)"));
    fileDialog.setFileMode(QFileDialog::ExistingFiles);
    fileDialog.show();

    if(fileDialog.exec()){
        subtitle->subs.clear();
        subtitle->setAddedSubtitle(false);
        subtitleText->hide();
        loadPlaylist(fileDialog.selectedUrls());
    }
}
void videoplayer::dragEnterEvent(QDragEnterEvent *event){
    if(event->mimeData()->hasUrls()){
                event->acceptProposedAction();
    }
}

void videoplayer::dropEvent(QDropEvent *event) {
    loadPlaylist(event->mimeData()->urls());
}

void videoplayer::loadPlaylist(QList<QUrl> urls){

    cmnds->m_playlist_entries->setStyleSheet("color:white");
    for (auto url : urls){
        m_playlist->addMedia(url);
        cmnds->m_playlist_entries->addItem(url.fileName().left(url.fileName().lastIndexOf('.')));
        this->setWindowTitle(url.fileName().left(url.fileName().lastIndexOf('.')));
    }
        m_playlist->setCurrentIndex(m_playlist->currentIndex()+1);
        m_playlist->setPlaybackMode(QMediaPlaylist::Sequential);
        m_graphicsView->setFocus();
        m_mediaPlayer->play();
}

void videoplayer::addToPlaylist(QList<QUrl> urls){
    for (auto url : urls){
        m_playlist->addMedia(url);
        cmnds->m_playlist_entries->addItem(url.fileName().split('.')[0]);
    }
        m_playlist->setCurrentIndex(m_playlist->nextIndex());
        m_playlist->setPlaybackMode(QMediaPlaylist::Sequential);
        m_graphicsView->setFocus();
        m_mediaPlayer->play();
}

void videoplayer::playClicked(){
    switch (m_playerState) {
        case QMediaPlayer::StoppedState:
        case QMediaPlayer::PausedState:
            m_mediaPlayer->play();
            QTimer::singleShot(2000, m_text, &QLabel::hide);
            m_text->show();
            m_text->setText("Playing");
            break;
        case QMediaPlayer::PlayingState:
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
    m_mediaPlayer->setVolume(m_mediaPlayer->volume() + VOLUME_STEP);
}

void videoplayer::volumeDecrease(){
    m_mediaPlayer->setVolume(m_mediaPlayer->volume() - VOLUME_STEP);
}

void videoplayer::seek(int seconds){
    m_mediaPlayer->setPosition(seconds * 1000);
}

//for some reason, nothing with emit works on Ubuntu,
//hence the commented out lines of code, might work just fine on other platforms

void videoplayer::muteClicked(){
    if(m_mediaPlayer->isMuted()==false){
        m_mediaPlayer->setMuted(true);
        cmnds->m_muteButton->setIcon(style()->standardIcon(QStyle::SP_MediaVolumeMuted));
        QTimer::singleShot(2000, m_text, &QLabel::hide);
        m_text->show();
        m_text->setText("Muted");
    }
    else{
        m_mediaPlayer->setMuted(false);
        cmnds->m_muteButton->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
        QTimer::singleShot(2000, m_text, &QLabel::hide);
        m_text->show();
        m_text->setText("Unmuted");
    }
}

//TO DO MENU KLASA
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
    QMenu *playback = new QMenu("Playback");
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
    QAction* incVol = playback->addAction("Increase volume");
    QAction* decVol = playback->addAction("Decrease volume");
    QAction* mute = playback->addAction("Mute");
    QAction* seekBack = playback->addAction("Seek backward");
    QAction* seekFwd = playback->addAction("Seek forward");
    playback->setStyleSheet(" QMenu {Background-color: #222222;color:white;}");

    //help padajuci meni
    help->addAction("Licence");
    help->setStyleSheet(" QMenu {Background-color: #222222;color:white;}");

    //povezivanje akcija sa funkcijama pri kliku
    connect(openFile, &QAction::triggered, this, &videoplayer::openFile);
    connect(exit, &QAction::triggered, this, &videoplayer::exit);
    connect(incVol, &QAction::triggered, this, &videoplayer::volumeIncrease);
    connect(decVol, &QAction::triggered, this, &videoplayer::volumeDecrease);
    connect(mute, &QAction::triggered, this, &videoplayer::muteClicked);
    connect(seekBack, &QAction::triggered, this, &videoplayer::seekBackwardClicked);
    connect(seekFwd, &QAction::triggered, this, &videoplayer::seekForwardClicked);
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
    seekBack->setShortcut(Qt::Key_Left);
    seekFwd->setShortcut(Qt::Key_Right);
    //incVol->setShortcut(Qt::Key_Plus);
    //decVol->setShortcut(Qt::Key_Minus);
    mute->setShortcut(Qt::Key_M);

    //povezivanje menija u jedan meni bar
    m_menuBar->addMenu(file);
    m_menuBar->addMenu(edit);
    m_menuBar->addMenu(playback);
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
    QFileInfo fileInfo(m_playlist->currentMedia().canonicalUrl().path());
    QString filename = fileInfo.fileName();
    this->setWindowTitle(filename.split('.')[0]);
}

void videoplayer::backwardClicked(){
    m_playlist->previous();
    m_mediaPlayer->play();
    QTimer::singleShot(2000, m_text, &QLabel::hide);
    m_text->show();
    m_text->setText("Backward");
    QFileInfo fileInfo(m_playlist->currentMedia().canonicalUrl().path());
    QString filename = fileInfo.fileName();
    this->setWindowTitle(filename.split('.')[0]);
}

//Not all playback services support change of the playback rate.
//It is framework defined as to the status and quality of audio and video
//while fast forwarding or rewinding.
void videoplayer::seekBackwardClicked(){
    qint64 value = 0.0;
    if (m_mediaPlayer->position() > SEEK_STEP)
        value = m_mediaPlayer->position()-SEEK_STEP;

    m_mediaPlayer->setPosition(value);
    QTimer::singleShot(2000, m_text, &QLabel::hide);
    m_text->show();
    m_text->setText("Seek backward");
}

void videoplayer::seekForwardClicked(){
    qint64 value = m_mediaPlayer->duration();

    if(m_mediaPlayer->position() < value)
        value = m_mediaPlayer->position() + SEEK_STEP;

    m_mediaPlayer->setPosition(value);

    QTimer::singleShot(2000, m_text, &QLabel::hide);
    m_text->show();
    m_text->setText("Seek forward");
}

int videoplayer::volume() const{

    qreal linearVolume =  QAudio::convertVolume(cmnds->m_volumeSlider->value() / qreal(100),
                                                    QAudio::LogarithmicVolumeScale,
                                                    QAudio::LinearVolumeScale);
    return qRound(linearVolume * 100);
}

void videoplayer::setVolume(qint64 volume){
    qreal logarithmicVolume = QAudio::convertVolume(volume / qreal(100),
                                                    QAudio::LinearVolumeScale,
                                                    QAudio::LogarithmicVolumeScale);
    cmnds->m_volumeSlider->setValue(qRound(logarithmicVolume * 100));
}

void videoplayer::onVolumeSliderChanged(){

    if(m_mediaPlayer->volume() == 0){
        cmnds->m_muteButton->setIcon(style()->standardIcon(QStyle::SP_MediaVolumeMuted));
    }
    else{
        cmnds->m_muteButton->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
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
        cmnds->m_playButton->click();
    else if(event->key()==Qt::Key_Plus)
        cmnds->m_volumeSlider->triggerAction(QAbstractSlider::SliderAction::SliderSingleStepAdd);
    else if(event->key()==Qt::Key_Minus)
        cmnds->m_volumeSlider->triggerAction(QAbstractSlider::SliderAction::SliderSingleStepSub);
    else if(event->key()==Qt::Key_F){
        if(!isFullScreen()){
            cmnds->hideCommands();
            m_menuBar->hide();
            this->layout()->setContentsMargins(0,0,0,0);
            this->layout()->setMargin(0);
            showFullScreen();
        } else{
            this->layout()->setContentsMargins(-1,-1,-1,-1);
            showNormal();
            cmnds->showCommands();
            m_menuBar->show();
        }
    }
    else if(event->key()==Qt::Key_Period)
        cmnds->m_forwardButton->click();
    else if(event->key()==Qt::Key_Comma)
        cmnds->m_backwardButton->click();
}

void videoplayer::mouseDoubleClickEvent(QMouseEvent *event){

    if(event->buttons()==Qt::MouseEventCreatedDoubleClick &&\
                        m_graphicsView->underMouse()){
        if(!isFullScreen()){
            cmnds->hideCommands();
            m_menuBar->hide();
            showFullScreen();
            cmnds->m_playButton->click();
        }else if(isFullScreen()){
            this->layout()->setContentsMargins(-1,-1,-1,-1);
            showNormal();
            cmnds->showCommands();
            m_menuBar->show();
            cmnds->m_playButton->click();
          }
    }
}

void videoplayer::mousePressEvent(QMouseEvent *event){

    if(event->button()==Qt::LeftButton && m_graphicsView->underMouse())
        cmnds->m_playButton->click();
    else if(event->button()==Qt::RightButton && m_graphicsView->underMouse())
        m_rightClickMenu->popup(QCursor::pos());
}

void videoplayer::wheelEvent(QWheelEvent *event){

    if(event->delta() < 0){
        m_mediaPlayer->setVolume(m_mediaPlayer->volume() - VOLUME_STEP);
    }
    if(event->delta() >= 0){
        m_mediaPlayer->setVolume(m_mediaPlayer->volume() + VOLUME_STEP);
    }
    setVolume(m_mediaPlayer->volume());
    event->accept();
}

void videoplayer::addSubtitle(){
    QString fileName = QFileDialog::getOpenFileName(this,tr("Select a Subtitle"),"",tr("*.srt"));
    if(fileName.isEmpty()){
        return;
    }
    else{
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox::information(this, tr("Unable to open file"),
            file.errorString());
            return;
        }
        subtitle->getSubtitles(file);
        subtitle->setAddedSubtitle(true);
    }
}


void videoplayer::addAnnotation()
{

    QDialog popupAnnotationMenu = QDialog();


    popupAnnotationMenu.setParent(this);
    popupAnnotationMenu.setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
    popupAnnotationMenu.activateWindow();

    QFormLayout *formLayout = new QFormLayout();

    //dodati provere vrednosti u formu

    QLineEdit *nameLineEdit = new QLineEdit();
    QLineEdit *beginLineEdit = new QLineEdit();
    QLineEdit *durationLineEdit = new QLineEdit();
    QLineEdit *textLineEdit = new QLineEdit();
    QLineEdit *heightLineEdit = new QLineEdit();
    QLineEdit *widthLineEdit = new QLineEdit();

    QDialogButtonBox formButtonBox = new QDialogButtonBox();

    formButtonBox.addButton(tr("Accept"),QDialogButtonBox::AcceptRole);
    formButtonBox.addButton(tr("Cancel"),QDialogButtonBox::RejectRole);



    formLayout->setSpacing(10);
    formLayout->setMargin(10);

    formLayout->addRow(tr("Name:"), nameLineEdit);
    formLayout->addRow(tr("Begin annotation at(hh:mm:ss):"), beginLineEdit);
    formLayout->addRow(tr("Duration of annotation(mm:ss):"), durationLineEdit);
    formLayout->addRow(tr("Heigth:"),heightLineEdit);
    formLayout->addRow(tr("Width:"),widthLineEdit);
    formLayout->addRow(tr("Text Content: "), textLineEdit);

    formLayout->setContentsMargins(10,10,10,10);


    connect(&formButtonBox, SIGNAL(accepted()),
            &popupAnnotationMenu, SLOT(accept()));
    connect(&formButtonBox, SIGNAL(rejected()),
            &popupAnnotationMenu, SLOT(reject()));


    popupAnnotationMenu.setLayout(formLayout);
    formLayout->addWidget(&formButtonBox);
    popupAnnotationMenu.showNormal();
    popupAnnotationMenu.setFocus();
    //provera vrednosti treba i za opseg vremena, anotacija moze da traje najduze od vremena kreiranja do zavrsetka videa


    if( popupAnnotationMenu.exec() && formButtonBox.AcceptRole==QDialogButtonBox::AcceptRole ){
        //ovde ide inicijalizacija sa unesenim poljima

        QString name = nameLineEdit->text();
        qint64 width = widthLineEdit->text().toInt();
        qint64 height = heightLineEdit->text().toInt();
        QString content = textLineEdit->text();
        QString beginAt = beginLineEdit->text();
        QString annDuration = durationLineEdit->text();
        QStringList times = beginAt.split(tr(":"));
        QStringList durations = annDuration.split(tr(":"));
        qint64 beginAnnotation = times[0].toInt()*1000*60*60 + times[1].toInt()*1000*60 + times[2].toInt()*1000;
        qint64 durationTime = durations[0].toInt()*1000*60 +durations[1].toInt()*1000;

        //TODO brisanje svih unosa u vektoru
        m_videoAnnotations.append(new Annotation(m_videoItem, width, height, content, beginAnnotation, durationTime));

    }
}


void videoplayer::playlistDoubleClickPlay(){
    m_playlist->setCurrentIndex(cmnds->m_playlist_entries->row(cmnds->m_playlist_entries->currentItem()));
    QFileInfo fileInfo(m_playlist->currentMedia().canonicalUrl().path());
    QString filename = fileInfo.fileName();
    this->setWindowTitle(filename.split('.')[0]);
}



#include "../headers/rightClickMenu.h"

#include <QAction>
#include <QDebug>
#include <QPalette>
#include <QStyle>

#include "../headers/videoplayer.h"
rightClickMenu::rightClickMenu(QWidget *parent) : QWidget(parent) {
  m_playOrPause = new QAction(parent);
  m_stop = new QAction(parent);
  m_previous = new QAction(parent);
  m_next = new QAction(parent);
  m_open = new QAction(parent);
  m_exit = new QAction(parent);
  m_addSubtitles = new QAction(parent);
  m_addAnnotations = new QAction(parent);
  m_saveAnnotations = new QAction(parent);

  m_playOrPause->setEnabled(false);
  m_playOrPause->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
  m_playOrPause->setText(QStringLiteral("Play"));

  m_stop->setEnabled(false);
  m_stop->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
  m_stop->setText(QStringLiteral("Stop"));

  m_previous->setEnabled(false);
  m_previous->setIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward));
  m_previous->setText(QStringLiteral("Previous"));

  m_next->setEnabled(false);
  m_next->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));
  m_next->setText(QStringLiteral("Next"));

  m_open->setEnabled(true);
  m_open->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
  m_open->setText(QStringLiteral("Open Media"));

  m_exit->setEnabled(true);
  m_exit->setIcon(style()->standardIcon(QStyle::SP_DialogCloseButton));
  m_exit->setText(QStringLiteral("Leave"));

  m_addSubtitles->setEnabled(false);
  m_addSubtitles->setIcon(style()->standardIcon(QStyle::SP_FileIcon));
  m_addSubtitles->setText(QStringLiteral("Add Subtitle"));

  m_addAnnotations->setEnabled(false);
  m_addAnnotations->setIcon(
      style()->standardIcon(QStyle::SP_MessageBoxInformation));
  m_addAnnotations->setText(QStringLiteral("Add Annotation"));

  m_saveAnnotations->setEnabled(false);
  m_saveAnnotations->setIcon(
      style()->standardIcon(QStyle::SP_DialogSaveButton));
  m_saveAnnotations->setText(QStringLiteral("Save Annotation"));

  m_RCMenu->addAction(m_playOrPause);
  m_RCMenu->addAction(m_stop);
  m_RCMenu->addAction(m_previous);
  m_RCMenu->addAction(m_next);
  m_RCMenu->addSeparator();
  m_RCMenu->addAction(m_open);
  m_RCMenu->addSeparator();
  m_RCMenu->addAction(m_addSubtitles);
  m_RCMenu->addSeparator();
  m_RCMenu->addAction(m_addAnnotations);
  m_RCMenu->addAction(m_saveAnnotations);
  m_RCMenu->addSeparator();
  m_RCMenu->addAction(m_exit);

  connect(m_open, SIGNAL(triggered()), parent, SLOT(openFile()));
  connect(m_playOrPause, SIGNAL(triggered()), parent, SLOT(playClicked()));
  connect(m_stop, SIGNAL(triggered()), parent, SLOT(stopClicked()));
  connect(m_previous, SIGNAL(triggered()), parent, SLOT(backwardClicked()));
  connect(m_next, SIGNAL(triggered()), parent, SLOT(forwardClicked()));
  connect(m_exit, SIGNAL(triggered()), parent, SLOT(exit()));
  connect(m_addSubtitles, SIGNAL(triggered()), parent, SLOT(addSubtitle()));
  connect(m_addAnnotations, SIGNAL(triggered()), parent, SLOT(addAnnotation()));
  connect(m_saveAnnotations, SIGNAL(triggered()), parent,
          SLOT(saveAnnotationsToJsonFile()));
}

rightClickMenu::~rightClickMenu() { delete m_RCMenu; }

/*! If the video is playing switch from play to pause, enable adding subtitles
 * and annotations, and enable stop next and previous */
void rightClickMenu::playingState() {
  m_playOrPause->setEnabled(true);
  m_playOrPause->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
  m_playOrPause->setText(QStringLiteral("Pause"));
  m_stop->setEnabled(true);
  m_previous->setEnabled(true);
  m_next->setEnabled(true);
  m_addAnnotations->setEnabled(true);
  m_saveAnnotations->setEnabled(true);
  m_addSubtitles->setEnabled(true);
}

void rightClickMenu::PausedState() {
  m_playOrPause->setEnabled(true);
  m_playOrPause->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
  m_playOrPause->setText(QStringLiteral("Play"));
  m_stop->setEnabled(true);
  m_previous->setEnabled(true);
  m_next->setEnabled(true);
  m_addAnnotations->setEnabled(true);
  m_saveAnnotations->setEnabled(true);
  m_addSubtitles->setEnabled(true);
}

void rightClickMenu::StoppedState() {
  m_playOrPause->setEnabled(true);
  m_playOrPause->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
  m_playOrPause->setText(QStringLiteral("Play"));
  m_addAnnotations->setEnabled(false);
  m_saveAnnotations->setEnabled(false);
  m_stop->setEnabled(false);
  m_previous->setEnabled(true);
  m_next->setEnabled(true);
  m_addSubtitles->setEnabled(true);
}

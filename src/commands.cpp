#include "../headers/commands.h"

#include <QApplication>
#include <QPushButton>
#include <QStyle>
#include <QtWidgets>

class videoplayer;
commands::commands(QWidget *parent) : QWidget(parent)
{
	screenGeometry = QApplication::desktop()->screenGeometry(this);

	m_playButton = new QPushButton;
	m_playButton->setEnabled(false);
	m_playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
	m_playButton->setMinimumSize(screenGeometry.width() / 18,
	                             screenGeometry.height() / 20);

	m_stopButton = new QPushButton;
	m_stopButton->setEnabled(false);
	m_stopButton->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
	m_stopButton->setMinimumSize(screenGeometry.width() / 18,
	                             screenGeometry.height() / 20);

	m_forwardButton = new QPushButton;
	m_forwardButton->setEnabled(false);
	m_forwardButton->setIcon(
	    style()->standardIcon(QStyle::SP_MediaSkipForward));
	m_forwardButton->setMinimumSize(screenGeometry.width() / 18,
	                                screenGeometry.height() / 20);

	m_backwardButton = new QPushButton;
	m_backwardButton->setEnabled(false);
	m_backwardButton->setIcon(
	    style()->standardIcon(QStyle::SP_MediaSkipBackward));
	m_backwardButton->setMinimumSize(screenGeometry.width() / 18,
	                                 screenGeometry.height() / 20);

	m_seekForwardButton = new QPushButton;
	m_seekForwardButton->setEnabled(false);
	m_seekForwardButton->setIcon(
	    style()->standardIcon(QStyle::SP_MediaSeekForward));
	m_seekForwardButton->setMinimumSize(screenGeometry.width() / 18,
	                                    screenGeometry.height() / 20);

	m_seekBackwardButton = new QPushButton;
	m_seekBackwardButton->setEnabled(false);
	m_seekBackwardButton->setIcon(
	    style()->standardIcon(QStyle::SP_MediaSeekBackward));
	m_seekBackwardButton->setMinimumSize(screenGeometry.width() / 18,
	                                     screenGeometry.height() / 20);

	m_muteButton = new QPushButton(this);
	m_muteButton->setEnabled(false);
	m_muteButton->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
	m_muteButton->setFixedSize(30, 30);

	m_openButton = new QPushButton(tr("Open"));
	m_openButton->setStyleSheet("color:white");

	m_showPlaylistButton = new QPushButton;
	m_showPlaylistButton->setIcon(
	    style()->standardIcon(QStyle::SP_FileDialogDetailedView));
	m_showPlaylistButton->setMinimumSize(screenGeometry.width() / 18,
	                                     screenGeometry.height() / 20);

	m_volumeSlider = new QSlider(Qt::Horizontal);
	m_volumeSlider->setRange(0, 100);
	m_volumeSlider->setMinimumSize(screenGeometry.width() / 18, 50);
	m_volumeSlider->setMaximumSize(screenGeometry.width() / 13, 50);
	m_volumeSlider->setValue(100);
	m_volumeSlider->setEnabled(false);

	m_durationInfo = new QLabel("00:00:00/00:00:00", this);
	m_durationInfo->setEnabled(false);
	m_durationInfo->setStyleSheet("color:rgb(255,255,255)");
	m_durationInfo->setAlignment(Qt::AlignCenter);
	m_durationInfo->setMinimumSize(2 * screenGeometry.width() / 15,
	                               screenGeometry.height() / 20);

	m_Slider = new QSlider(Qt::Horizontal);
	m_Slider->setEnabled(false);

	commandsLayout = new QHBoxLayout;
	sliderLayout = new QHBoxLayout;
}


/*! hides each button, used when going to fullscreen */
void commands::hideCommands()
{
	m_playButton->hide();
	m_stopButton->hide();
	m_forwardButton->hide();
	m_backwardButton->hide();
	m_seekForwardButton->hide();
	m_seekBackwardButton->hide();
	m_muteButton->hide();
	m_openButton->hide();
	m_volumeSlider->hide();
	m_durationInfo->hide();
	m_Slider->hide();
	m_showPlaylistButton->hide();
}

void commands::showCommands()
{
	m_playButton->show();
	m_stopButton->show();
	m_forwardButton->show();
	m_backwardButton->show();
	m_seekForwardButton->show();
	m_seekBackwardButton->show();
	m_muteButton->show();
	m_openButton->show();
	m_volumeSlider->show();
	m_durationInfo->show();
	m_Slider->show();
	m_showPlaylistButton->show();
}

void commands::enableAllCommands()
{
	m_playButton->setEnabled(true);
	m_stopButton->setEnabled(true);
	m_forwardButton->setEnabled(true);
	m_backwardButton->setEnabled(true);
	m_seekForwardButton->setEnabled(true);
	m_seekBackwardButton->setEnabled(true);
	m_muteButton->setEnabled(true);
	m_volumeSlider->setEnabled(true);
	m_durationInfo->setEnabled(true);
	m_Slider->setEnabled(true);
}

void commands::createCommandLayout()
{
	commandsLayout->setMargin(0);
	commandsLayout->addWidget(m_durationInfo);
	commandsLayout->addWidget(m_backwardButton);
	commandsLayout->addWidget(m_seekBackwardButton);
	commandsLayout->addWidget(m_playButton);
	commandsLayout->addWidget(m_stopButton);
	commandsLayout->addWidget(m_seekForwardButton);
	commandsLayout->addWidget(m_forwardButton);
	commandsLayout->addWidget(m_showPlaylistButton);
	commandsLayout->addWidget(m_muteButton);
	commandsLayout->addWidget(m_volumeSlider);

	commandsLayout->setSpacing(0);
}

void commands::createSliderLayout()
{
	sliderLayout->setMargin(0);
	sliderLayout->addWidget(m_Slider);
	sliderLayout->addWidget(m_openButton);
	sliderLayout->setSpacing(0);
}

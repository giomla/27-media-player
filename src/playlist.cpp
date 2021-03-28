#include "../headers/playlist.h"

#include <qset.h>

#include <QItemSelectionModel>
playlist::playlist(QWidget* parent, QMediaPlayer* player)
    : QWidget(parent), m_player(player)
{
	m_playlist = new QMediaPlaylist(this);
	m_playlist_entries = new QListWidget(this);
	m_playlist_entries->hide();
};

void playlist::loadPlaylist(QList<QUrl> urls)
{
	m_playlist_entries->setStyleSheet("color:white");
	int i = 0;

	for (auto url : urls) {
		if (listUrls.contains(url)) {
			continue;
		}
		listUrls.append(url);
		m_playlist->addMedia(url);
		m_playlist_entries->addItem(
		    url.fileName().left(url.fileName().lastIndexOf('.')));
		// this->setWindowTitle(url.fileName().left(url.fileName().lastIndexOf('.')));
		this->parentWidget()->setWindowTitle(
		    url.fileName().left(url.fileName().lastIndexOf('.')));
		i++;
	}
	m_playlist->setCurrentIndex(m_playlist->currentIndex() + 1);
	m_playlist->setPlaybackMode(QMediaPlaylist::Sequential);
	m_player->play();
}

void playlist::showPlaylist()
{
	if (m_playlist_entries->isHidden()) {
		m_playlist_entries->show();
		m_playlist_entries->setFocus();
	} else {
		m_playlist_entries->hide();
		this->focusNextChild();
	}
}

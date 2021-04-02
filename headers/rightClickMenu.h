#ifndef RIGHTCLICKMENU_H
#define RIGHTCLICKMENU_H

#include <QAction>
#include <QMenu>
#include <QWidget>

class rightClickMenu : public QWidget
{
     public:
	rightClickMenu(QWidget* parent = nullptr);
	~rightClickMenu();

	QMenu* m_RCMenu = new QMenu();
    /*! fuction that switches from play to pause while video is being played */
	void playingState();
    /*! fuction that switches from pause to play when video is paused */
	void PausedState();
    /*! fuction that enables Stop after video is loaded */
	void StoppedState();

     private:
	QAction* m_playOrPause = nullptr;
	QAction* m_stop = nullptr;
	QAction* m_previous = nullptr;
	QAction* m_next = nullptr;
	QAction* m_open = nullptr;
	QAction* m_exit = nullptr;
	QAction* m_addSubtitles = nullptr;
	QAction* m_addAnnotations = nullptr;
	QAction* m_saveAnnotations = nullptr;
};

#endif  // RIGHTCLICKMENU_H

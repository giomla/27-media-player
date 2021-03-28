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
	void playingState();
	void PausedState();
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

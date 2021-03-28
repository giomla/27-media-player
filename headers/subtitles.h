#ifndef SUBTITLES_H
#define SUBTITLES_H

#endif  // SUBTITLES_H

#include <QFile>
#include <QString>
class subtitles
{
     public:
	subtitles(){};
	subtitles(QString line, QString beginTime, QString endTime);

	QList<subtitles> subs;
	void getSubtitles(QFile &file);

	QString getLine() const { return m_line; };
	QString getBeginTime() const { return m_beginTime; }
	QString getEndTime() const { return m_endTime; }

	bool isAddedSubtitle() { return addedSubtitle; }
	void setAddedSubtitle(bool isAdded) { addedSubtitle = isAdded; }

	~subtitles();

     private:
	QString m_line;
	QString m_beginTime;
	QString m_endTime;
	bool addedSubtitle = false;
};

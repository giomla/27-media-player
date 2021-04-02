#ifndef SUBTITLES_H
#define SUBTITLES_H

#endif  // SUBTITLES_H

#include <QFile>
#include <QString>
class subtitles
{
     public:
	subtitles(){};
    subtitles(const QString &line, const QString &beginTime, const QString &endTime);

	QList<subtitles> subs;
    /*! fuction that parses .srt file */
	void getSubtitles(QFile &file);
    /*! function that returns one line of subtitles */
	QString getLine() const { return m_line; };
    /*! fuction that returns the begining time of the line, used to show when a line should apper in a video */
	QString getBeginTime() const { return m_beginTime; }
    /*! function that retures the end time of the line, used to show when a line should disappear from a video */
	QString getEndTime() const { return m_endTime; }
    /*! fuction that checks if subtitle is added */
	bool isAddedSubtitle() { return addedSubtitle; }
    /*! function that sets the right subtitle to the video */
	void setAddedSubtitle(bool isAdded) { addedSubtitle = isAdded; }

	~subtitles();

     private:
	QString m_line;
	QString m_beginTime;
	QString m_endTime;
	bool addedSubtitle = false;
};

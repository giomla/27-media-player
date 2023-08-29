#include "../headers/subtitles.h"

#include <QTextStream>
#include <QTime>
#include <utility>
subtitles::subtitles(QString line, QString beginTime, QString endTime)
    : m_line(std::move(line)), m_beginTime(std::move(beginTime)),
      m_endTime(std::move(endTime)) {}

/*! Function that reads .srt file line by line and stores the begin time of a
 * line, line itself and the end time of a line. */
void subtitles::getSubtitles(QFile &file) {
  QTextStream in(&file);
  in.readLine();
  QString tmp = in.readLine();
  QStringList times = tmp.split(QStringLiteral(" --> "));
  QString beginT = times.at(0);
  QString endT = times.at(1);
  QTime pocetnoVremeTmp;
  QString pocetnoVreme;
  QString zavrsnoVreme;
  QTime zavrsnoVremeTmp;

  pocetnoVremeTmp = QTime::fromString(beginT, QStringLiteral("hh:mm:ss,zzz"));
  pocetnoVreme = pocetnoVremeTmp.toString(QStringLiteral("hh:mm:ss"));

  zavrsnoVremeTmp = QTime::fromString(endT, QStringLiteral("hh:mm:ss,zzz"));
  zavrsnoVreme = zavrsnoVremeTmp.toString(QStringLiteral("hh:mm:ss"));

  QString fullSentence;
  QString line;
  times.clear();
  while (!in.atEnd()) {
    line = in.readLine();
    if (!line.isEmpty()) {
      fullSentence.append(line).append("\n");
    } else {
      subs.append(subtitles(fullSentence, pocetnoVreme, zavrsnoVreme));
      fullSentence.clear();
      times.clear();
      QString num = in.readLine();

      if (!num.contains(QRegExp("^[0-9]+$"))) {
        break;
      }
      tmp = in.readLine();
      times = tmp.split(QStringLiteral(" --> "));
      beginT = times.at(0);
      endT = times.at(1);

      pocetnoVremeTmp =
          QTime::fromString(beginT, QStringLiteral("hh:mm:ss,zzz"));
      pocetnoVreme = pocetnoVremeTmp.toString(QStringLiteral("hh:mm:ss"));

      zavrsnoVremeTmp = QTime::fromString(endT, QStringLiteral("hh:mm:ss,zzz"));
      zavrsnoVreme = zavrsnoVremeTmp.toString(QStringLiteral("hh:mm:ss"));

      times.clear();
    }
  }
}

subtitles::~subtitles() = default;

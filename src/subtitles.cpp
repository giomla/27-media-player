#include "../headers/subtitles.h"
#include <QTime>
#include <QTextStream>
subtitles::subtitles(QString line, QString beginTime, QString endTime):
    m_line(line), m_beginTime(beginTime), m_endTime(endTime)
{

}

void subtitles::getSubtitles(QFile &file){
    QTextStream in(&file);
    QString num = in.readLine();
    QString tmp = in.readLine();
    QStringList times = tmp.split(" --> ");
    QString beginT = times.at(0);
    QString endT = times.at(1);
    QTime pocetnoVremeTmp;
    QString pocetnoVreme;
    QString zavrsnoVreme;
    QTime zavrsnoVremeTmp;

    pocetnoVremeTmp = QTime::fromString(beginT,"hh:mm:ss,zzz");
    pocetnoVreme = pocetnoVremeTmp.toString("hh:mm:ss");

    zavrsnoVremeTmp = QTime::fromString(endT,"hh:mm:ss,zzz");
    zavrsnoVreme = zavrsnoVremeTmp.toString("hh:mm:ss");

    QString fullSentence;
    QString line;
    times.clear();
    while(!in.atEnd()){

        line = in.readLine();
        if(!line.isEmpty()){
            fullSentence.append(line).append("\n");
        }else{

            subs.append(subtitles(fullSentence, pocetnoVreme, zavrsnoVreme));
            fullSentence.clear();
            times.clear();
            QString num = in.readLine();

                if(!num.contains(QRegExp("^[0-9]+$"))){
                    break;
                }
            QString tmp = in.readLine();
            QStringList times = tmp.split(" --> ");
            beginT = times.at(0);
            endT = times.at(1);

            pocetnoVremeTmp = QTime::fromString(beginT,"hh:mm:ss,zzz");
            pocetnoVreme = pocetnoVremeTmp.toString("hh:mm:ss");

            zavrsnoVremeTmp = QTime::fromString(endT,"hh:mm:ss,zzz");
            zavrsnoVreme = zavrsnoVremeTmp.toString("hh:mm:ss");

            times.clear();

        }
   }
}

subtitles::~subtitles(){}
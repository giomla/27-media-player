#ifndef ANNOTATION_H
#define ANNOTATION_H

#include <QObject>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QMouseEvent>
#include <QPlainTextEdit>
#include "../headers/videoplayer.h"

class Annotation : public QGraphicsItem, public QObject
{
public:
    Annotation(QGraphicsItem *parent, qint64 width, qint64 height, QString content, qint64 beginAt, qint64 duration);
    ~Annotation();
    bool resize_on = false;
    void modifyText();
    qint64 width() const;
    void setWidth(const qint64 &width);

    qint64 height() const;
    void setHeight(const qint64 &height);

    QString text_content() const;
    void setText_content(const QString &text_content);

    QString *name() const;
    void setName(QString *name);

    qint64 appearance_time() const;
    void setAppearance_time(const qint64 &appearance_time);

    qint64 duration() const;
    void setDuration(const qint64 &duration);

    qint64 getCurrTimeOfVideo() const;
    void setCurrTimeOfVideo(const qint64 &value);

    bool getCurrActive() const;
    void setCurrActive(bool value);

    bool getAlreadyModifying() const;
    void setAlreadyModifying(bool value);

public slots:
    void modified();
protected:
    void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0) override;
    QRectF boundingRect() const override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
private:
    QRectF *m_rect = nullptr;
    QString *m_name = nullptr;
    qint64 m_appearance_time;
    qint64 m_duration;
    QString m_text_content = "";
    qint64 m_width,m_height;
    QPlainTextEdit *editor = nullptr;

    QDialog *modifyDialog = nullptr;
    qint64 currTimeOfVideo = 0;
    bool currActive = true;
    bool alreadyModifying = false;
};

#endif // ANNOTATION_H

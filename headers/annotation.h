#ifndef ANNOTATION_H
#define ANNOTATION_H

#define MIN_HEIGHT 50
#define MIN_WIDTH 100

#include <QGraphicsItem>
#include <QGraphicsObject>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QLineEdit>
#include <QMatrix>
#include <QMouseEvent>
#include <QObject>
#include <QPainter>
#include <QPlainTextEdit>

#include "../headers/videoplayer.h"

class Annotation : public QGraphicsObject
{
	Q_OBJECT
     public:
	Annotation(QGraphicsItem *parent, qint64 width, qint64 height,
	           QString content, qint64 beginAt, qint64 duration);
	~Annotation();
	bool resize_on = false;

	void resizeOccured();
	void setRect(QRectF rect);
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
	void modifiedDur();
	void canceled();
	void canceledDur();
	void modifyText();
	void resizing();
	void stopResizing();
	void modifyDur();



    protected:



	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
	           QWidget *widget = 0) override;
	QRectF boundingRect() const override;
	void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
	void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
	void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
	void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
	void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;
	void wheelEvent(QGraphicsSceneWheelEvent *event) override;

     private:
	QRectF *m_rect = nullptr;
	QString *m_name = nullptr;
	qint64 m_appearance_time;
	qint64 m_duration;
	QString m_text_content = "";
	qint64 m_width, m_height;
	QMenu *menu = nullptr;

	qint64 m_currDisplayPos = 0;

	QLineEdit *beginEdit = nullptr;
	QLineEdit *durationEdit = nullptr;

	QPlainTextEdit *editor = nullptr;
	QDialog *modifyDialog = nullptr;
	QDialog *durDialog = nullptr;
	qint64 currTimeOfVideo = 0;
	bool currActive = true;
	bool alreadyModifying = false;
};

#endif  // ANNOTATION_H

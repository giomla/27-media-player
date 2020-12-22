#include "../headers/annotation.h"
#include <QMenu>
#include <QDialog>
#include <QString>
#include <QPushButton>
#include <QLayout>
#include <QKeyEvent>
#include <QMatrix>

Annotation::Annotation(QGraphicsItem *parent, qint64 width, qint64 height, QString content, qint64 beginAt, qint64 duration)
{
    this->setParentItem(parent);
    this->setFlag(QGraphicsItem::ItemIsMovable, true);
    this->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    this->setFlag(QGraphicsItem::ItemIsFocusable, true);
    this->setFlag(QGraphicsItem::ItemIsSelectable, true);
    this->setFocus(Qt::MouseFocusReason);
    this->setAcceptHoverEvents(true);
    this->setText_content(content);
    this->setHeight(height);
    this->setWidth(width);
    this->setDuration(duration);
    this->setAppearance_time(beginAt);
    m_rect=new QRectF(0,0,width,height);
    //TODO provere granica da ne nestanu van scene anotacije
}

Annotation::~Annotation(){
}

void Annotation::modifyText()
{
    if(!this->getAlreadyModifying()){
    modifyDialog = new QDialog();
    modifyDialog->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
    modifyDialog->activateWindow();

    QHBoxLayout *hlayout = new QHBoxLayout;
    QPushButton *saveButton = new QPushButton("Save changes");
    QPushButton *cancelButton = new QPushButton("Cancel changes");
    cancelButton->setShortcut(QKeySequence::Cancel);

    editor = new QPlainTextEdit(this->text_content());

    editor->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    editor->setFocus();
    editor->createStandardContextMenu();

    hlayout->addWidget(editor);
    hlayout->addWidget(saveButton);
    hlayout->addWidget(cancelButton);
    modifyDialog->setLayout(hlayout);
    this->setAlreadyModifying(true);
    modifyDialog->show();
    QObject::connect(saveButton, &QPushButton::clicked , this, &Annotation::modified);
    QObject::connect(cancelButton, &QPushButton::clicked , this, &Annotation::canceled);
    }
}

void Annotation::resizeOccured()
{
    if (x() < 0){
        setPos(0, y());
    }else if (x() + boundingRect().right() > scene()->views().at(0)->viewport()->width()){
        setPos(scene()->views().at(0)->viewport()->width() - boundingRect().width(), y());
    }

    if (y() < 0){
        setPos(x(), 0);
    }
    else if ( y()+ boundingRect().bottom() > scene()->views().at(0)->viewport()->height()){
        setPos(x(),scene()->views().at(0)->viewport()->height() - boundingRect().height());
    }
}

void Annotation::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED( option )
    Q_UNUSED( widget )
        //ako si u dozvoljenom opsegu za pojaviljivanje
        if(this->getCurrTimeOfVideo() >= this->appearance_time()
                && this->getCurrTimeOfVideo() < (this->appearance_time()+this->duration())){

            if(!this->getCurrActive()){
                this->setCurrActive(true);
                this->setFlag(QGraphicsItem::ItemIsMovable, true);
                this->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
                this->setFlag(QGraphicsItem::ItemIsFocusable, true);
                this->setFlag(QGraphicsItem::ItemIsSelectable, true);
            }
            this->setAcceptHoverEvents(true);
            this->setActive(true);

            resizeOccured();

            painter->setRenderHint(QPainter::Antialiasing);
            painter->fillRect(*m_rect, QBrush(Qt::white));
            QRectF boundingRect;
            //QRectF rect = QRectF(0,0,width(),height());
            QPen pen;
            pen.setColor(Qt::black);
            painter->drawRect(*m_rect);
            QFont font = painter->font();
            font.setPixelSize(24);
            font.setWordSpacing(3);
            painter->setFont(font);
            painter->drawText(*m_rect,Qt::TextWrapAnywhere,this->text_content(),&boundingRect);

        } else {
            if(this->getCurrActive()){
                this->setCurrActive(false);
                this->setFlag(QGraphicsItem::ItemIsMovable, false);
                this->setFlag(QGraphicsItem::ItemSendsGeometryChanges, false);
                this->setFlag(QGraphicsItem::ItemIsFocusable, false);
                this->setFlag(QGraphicsItem::ItemIsSelectable, false);

                this->setActive(false);
            } else {
                this->setCurrActive(false);
            }
        }
}

QRectF Annotation::boundingRect() const
{
    QRectF rect = QRectF(0,0,width(),height());
    return rect;
}

void Annotation::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
  if(event->button()==Qt::LeftButton){
        // we propagate the event to parent item as the annotation is not currently active
        if(!this->getCurrActive())
            event->ignore();
    }
}

void Annotation::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    //we prevent default multiple selection of objects

    if(this->isSelected()){
        QGraphicsItem::mouseMoveEvent(event);
        //we check if the annotation is outside the current viewport of the scene and we move it
        //so its still inside of it
        resizeOccured();
    } else
        event->ignore();
}

void Annotation::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    if(!this->getAlreadyModifying() && this->getCurrActive()){
        menu = new QMenu;
        //TODO ACTION FOR RESIZE
        menu->addAction("Resize annotation");
        menu->addAction("Edit text", this, SLOT(modifyText()));
        menu->popup(event->screenPos());
        event->setAccepted(true);
    } else
        event->ignore();
}

void Annotation::modified()
{
    this->setText_content(editor->toPlainText());
    update();
    delete modifyDialog;
    this->setAlreadyModifying(false);
}
void Annotation::canceled()
{
    delete modifyDialog;
    this->setAlreadyModifying(false);
}

//geters and seters

bool Annotation::getAlreadyModifying() const
{
    return alreadyModifying;
}

void Annotation::setAlreadyModifying(bool value)
{
    alreadyModifying = value;
}

bool Annotation::getCurrActive() const
{
    return currActive;
}

void Annotation::setCurrActive(bool value)
{
    currActive = value;
}

qint64 Annotation::getCurrTimeOfVideo() const
{
    return currTimeOfVideo;
}

void Annotation::setCurrTimeOfVideo(const qint64 &value)
{
    currTimeOfVideo = value;
}

qint64 Annotation::duration() const
{
    return m_duration;
}

void Annotation::setDuration(const qint64 &duration)
{
    m_duration = duration;
}

qint64 Annotation::appearance_time() const
{
    return m_appearance_time;
}

void Annotation::setAppearance_time(const qint64 &appearance_time)
{
    m_appearance_time = appearance_time;
}

QString *Annotation::name() const
{
    return m_name;
}

void Annotation::setName(QString *name)
{
    m_name = name;
}

QString Annotation::text_content() const
{
    return m_text_content;
}

void Annotation::setText_content(const QString &text_content)
{
    m_text_content = text_content;
}

qint64 Annotation::height() const
{
    return m_height;
}

void Annotation::setHeight(const qint64 &height)
{
    m_height = height;
}

qint64 Annotation::width() const
{
    return m_width;
}

void Annotation::setWidth(const qint64 &width)
{
    m_width = width;
}

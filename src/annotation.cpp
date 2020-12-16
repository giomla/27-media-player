#include "../headers/annotation.h"
#include <QMenu>
#include <QDialog>
#include <QString>
#include <QPushButton>
#include <QLayout>

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
    m_rect=new QRectF(100,100,width,height);

    //TODO provere granica da ne nestanu van scene anotacije
}

Annotation::~Annotation(){
    //TODO brisanje
}

void Annotation::modifyText()
{

    modifyDialog = new QDialog();
    modifyDialog->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
    modifyDialog->activateWindow();

    QHBoxLayout *hlayout = new QHBoxLayout;
    QPushButton *saveButton = new QPushButton("Save changes");



    editor = new QPlainTextEdit(this->text_content());

    editor->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    editor->setFocus();
    editor->createStandardContextMenu();

    hlayout->addWidget(editor);
    hlayout->addWidget(saveButton);

    modifyDialog->setLayout(hlayout);

    modifyDialog->show();
    QObject::connect(saveButton, &QPushButton::clicked , this, &Annotation::modified);


}

void Annotation::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED( option )
    Q_UNUSED( widget )
        //ako si u dozvoljenom opsegu za pojaviljivanje
        if(this->getCurrTimeOfVideo() >= this->appearance_time()
                && this->getCurrTimeOfVideo() < (this->appearance_time()+this->duration())){

            //moze i provera da li si aktivan, da ne bi bezveze ovo radio, al je nesto bagovalo

            this->setCurrActive(true);
            this->setFlag(QGraphicsItem::ItemIsMovable, true);
            this->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
            this->setFlag(QGraphicsItem::ItemIsFocusable, true);
            this->setFlag(QGraphicsItem::ItemIsSelectable, true);
            this->setAcceptHoverEvents(true);


            painter->setRenderHint(QPainter::Antialiasing);
            painter->fillRect(*m_rect, QBrush(Qt::blue));
            QRectF boundingRect;
            QRectF rect = QRectF(100,100,width(),height());
            QPen pen;
            pen.setColor(Qt::black);
            painter->drawRect(rect);
            QFont font = painter->font();
            font.setPixelSize(24);
            font.setWordSpacing(3);
            painter->setFont(font);

            painter->drawText(rect,Qt::AlignJustify | Qt::TextWordWrap ,this->text_content(),&boundingRect);

        } else {
            if(this->getCurrActive()){
                this->setCurrActive(false);
                this->setFlag(QGraphicsItem::ItemIsMovable, false);
                this->setFlag(QGraphicsItem::ItemSendsGeometryChanges, false);
                this->setFlag(QGraphicsItem::ItemIsFocusable, false);
                this->setFlag(QGraphicsItem::ItemIsSelectable, false);
            } else {
                this->setCurrActive(false);
            }
        }
}

QRectF Annotation::boundingRect() const
{
    QRectF rect = QRectF(100,100,width(),height());
    return rect;
}

void Annotation::mousePressEvent(QGraphicsSceneMouseEvent *event)
{

    if(event->button()==Qt::RightButton){
        if(!this->getAlreadyModifying() && this->getCurrActive()){
            this->setAlreadyModifying(true);
            modifyText();
        }
    }
}


//geteri i seteri za polja

void Annotation::modified()
{

    this->setText_content(editor->toPlainText());
    update();
    delete modifyDialog;
    this->setAlreadyModifying(false);
}

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


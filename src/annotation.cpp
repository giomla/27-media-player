#include "../headers/annotation.h"

#include <QDialog>
#include <QKeyEvent>
#include <QLayout>
#include <QLineEdit>
#include <QMenu>
#include <QPushButton>
#include <QString>
#include <utility>

Annotation::Annotation(QGraphicsItem *parent, const QString &name, qint64 width,
                       qint64 height, const QString &content, qint64 beginAt,
                       qint64 duration) {
  this->setParentItem(parent);
  this->setFlag(QGraphicsItem::ItemIsMovable, true);
  this->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
  this->setFlag(QGraphicsItem::ItemIsFocusable, true);
  this->setFlag(QGraphicsItem::ItemIsSelectable, true);
  this->setFocus(Qt::MouseFocusReason);
  this->setAcceptHoverEvents(true);
  this->setName(std::move(name));
  this->setText_content(content);
  this->setHeight(height);
  this->setWidth(width);
  this->setDuration(duration);
  this->setAppearance_time(beginAt);
  m_rect = new QRectF(0, 0, width, height);
}

Annotation::~Annotation() {
  // delete m_name;
}

void Annotation::modifyText() {
  if (!this->getAlreadyModifying()) {
    modifyDialog = new QDialog();
    modifyDialog->setWindowFlags(Qt::WindowStaysOnTopHint);
    modifyDialog->setMinimumSize(300, 300);
    modifyDialog->activateWindow();

    auto *hlayout = new QVBoxLayout;
    auto *saveButton = new QPushButton(QStringLiteral("Save changes"));
    auto *cancelButton = new QPushButton(QStringLiteral("Cancel changes"));
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
    QObject::connect(saveButton, &QPushButton::clicked, this,
                     &Annotation::modified);
    QObject::connect(cancelButton, &QPushButton::clicked, this,
                     &Annotation::canceled);
  }
}

void Annotation::modifyDur() {
  if (!getAlreadyModifying()) {
    durDialog = new QDialog();
    durDialog->setWindowFlags(Qt::CustomizeWindowHint |
                              Qt::WindowStaysOnTopHint);
    durDialog->activateWindow();

    qint64 durationMils = this->duration();
    qint64 mins = (durationMils) / (60 * 1000);
    qint64 secs = (durationMils - mins * (60 * 1000)) / 1000;

    QString durationText = QStringLiteral("%1:%2").arg(mins).arg(secs);

    qint64 beginMils = this->appearance_time();
    qint64 bhours = beginMils / (60 * 60 * 1000);
    qint64 bmins = (beginMils - bhours * (60 * 60 * 1000)) / (60 * 1000);
    qint64 bsecs =
        (beginMils - bhours * (60 * 60 * 1000) - bmins * (60 * 1000)) / 1000;

    QString beginText =
        QStringLiteral("%1:%2:%3").arg(bhours).arg(bmins).arg(bsecs);

    auto *vlayout = new QVBoxLayout;
    auto *saveButton = new QPushButton(QStringLiteral("Save changes"));
    auto *cancelButton = new QPushButton(QStringLiteral("Cancel changes"));
    cancelButton->setShortcut(QKeySequence::Cancel);

    beginEdit = new QLineEdit();
    beginEdit->setText(beginText);
    auto *beginLabel = new QLabel();
    beginLabel->setText(QStringLiteral("Annotation beginning(hh:mm:ss): "));

    durationEdit = new QLineEdit();

    auto *durationLabel = new QLabel();
    durationEdit->setText(durationText);
    durationLabel->setText(QStringLiteral("Annotation duration(mm:ss): "));

    auto *hlayout1 = new QHBoxLayout();
    hlayout1->addWidget(beginLabel);
    hlayout1->addWidget(beginEdit);

    auto *hlayout2 = new QHBoxLayout();
    hlayout2->addWidget(durationLabel);
    hlayout2->addWidget(durationEdit);

    vlayout->addLayout(hlayout1);
    vlayout->addLayout(hlayout2);
    vlayout->addWidget(saveButton);
    vlayout->addWidget(cancelButton);

    durDialog->setLayout(vlayout);
    this->setAlreadyModifying(true);
    durDialog->show();
    QObject::connect(saveButton, &QPushButton::clicked, this,
                     &Annotation::modifiedDur);
    QObject::connect(cancelButton, &QPushButton::clicked, this,
                     &Annotation::canceledDur);
  }
}

void Annotation::resizing() {
  resize_on = true;
  this->setSelected(true);
}

void Annotation::stopResizing() {
  resize_on = false;
  this->setSelected(false);
}

void Annotation::resizeOccured() {
  if (this->height() >= scene()->views().at(0)->viewport()->height()) {
    this->setHeight(scene()->views().at(0)->viewport()->height() / 2);
  } else if (this->width() >= scene()->views().at(0)->viewport()->width()) {
    this->setWidth(scene()->views().at(0)->viewport()->width() / 2);
  }
  if (x() < 0) {
    setPos(0, y());
  } else if (x() + boundingRect().right() >
             scene()->views().at(0)->viewport()->width()) {
    setPos(scene()->views().at(0)->viewport()->width() - boundingRect().width(),
           y());
  }

  if (y() < 0) {
    setPos(x(), 0);
  } else if (y() + boundingRect().bottom() >
             scene()->views().at(0)->viewport()->height()) {
    setPos(x(), scene()->views().at(0)->viewport()->height() -
                    boundingRect().height());
  }
}

void Annotation::paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget *widget) {
  Q_UNUSED(option)
  Q_UNUSED(widget)
  // ako si u dozvoljenom opsegu za pojaviljivanje
  if (this->getCurrTimeOfVideo() >= this->appearance_time() &&
      this->getCurrTimeOfVideo() <
          (this->appearance_time() + this->duration())) {
    if (!this->getCurrActive()) {
      this->setCurrActive(true);
      if (!resize_on)
        this->setFlag(QGraphicsItem::ItemIsMovable, true);
      this->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
      this->setFlag(QGraphicsItem::ItemIsFocusable, true);
      this->setFlag(QGraphicsItem::ItemIsSelectable, true);
    }
    this->setAcceptHoverEvents(true);
    this->setActive(true);
    painter->setRenderHint(QPainter::Antialiasing);

    resizeOccured();

    painter->fillRect(this->boundingRect(), QBrush(Qt::white));
    QRectF boundingRect;
    QPen pen;
    pen.setColor(Qt::black);
    painter->drawRect(this->boundingRect());
    QFont font = painter->font();
    font.setPixelSize(18);
    font.setWordSpacing(3);
    painter->setFont(font);
    QString displayText = text_content();
    QRectF boundary = this->boundingRect();
    boundary.setRect(boundary.x() + 5, boundary.y() + 5, boundary.width() - 10,
                     boundary.height() - 10);
    painter->drawText(boundary, Qt::TextWrapAnywhere,
                      displayText.mid(m_currDisplayPos), &boundingRect);

  } else {
    if (this->getCurrActive()) {
      this->setCurrActive(false);
      this->setFlag(QGraphicsItem::ItemIsMovable, false);
      this->setFlag(QGraphicsItem::ItemSendsGeometryChanges, false);
      this->setFlag(QGraphicsItem::ItemIsFocusable, false);
      this->setFlag(QGraphicsItem::ItemIsSelectable, false);
      this->setActive(false);
    }
  }
}

auto Annotation::boundingRect() const -> QRectF {
  QRectF rect = QRectF(0, 0, width(), height());
  return rect;
}

void Annotation::mousePressEvent(QGraphicsSceneMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {
    // we propagate the event to parent item as the annotation is not
    // currently active
    if (!this->getCurrActive())
      event->ignore();
    else if (resize_on) {
      // TODO provera da li si van scene
      if (scene()->views().at(0)->rect().contains(QCursor::pos())) {
        if (!(event->pos().x() < MIN_WIDTH))
          this->setWidth(event->pos().x());
        if (!(event->pos().y() < MIN_HEIGHT))
          this->setHeight(event->pos().y());
        this->prepareGeometryChange();
        update();
      }
    }
  }
}

void Annotation::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
  // we prevent default multiple selection of objects
  if (this->isSelected() && !resize_on) {
    this->setCursor(Qt::ClosedHandCursor);
    QGraphicsItem::mouseMoveEvent(event);
    // we check if the annotation is outside the current viewport of the
    // scene and we move it so its still inside of it
    resizeOccured();
  } else if (resize_on) {
    // if the cursor is inside the scene view rect
    if (scene()->views().at(0)->rect().contains(event->pos().x(),
                                                event->pos().y())) {
      if (!(event->pos().x() < MIN_WIDTH))
        this->setWidth(event->pos().x());
      if (!(event->pos().y() < MIN_HEIGHT))
        this->setHeight(event->pos().y());
      this->prepareGeometryChange();
      update();
    }
  } else
    event->ignore();
}

void Annotation::contextMenuEvent(QGraphicsSceneContextMenuEvent *event) {
  if (!this->getAlreadyModifying() && this->getCurrActive()) {
    menu = new QMenu;
    if (!resize_on)
      menu->addAction(QStringLiteral("Resize annotation"), this,
                      &Annotation::resizing);
    else
      menu->addAction(QStringLiteral("End resizing"), this,
                      &Annotation::stopResizing);
    menu->addAction(QStringLiteral("Edit text"), this, &Annotation::modifyText);
    menu->addAction(QStringLiteral("Edit Duration"), this,
                    &Annotation::modifyDur);
    menu->popup(event->screenPos());
    event->setAccepted(true);
  } else
    event->ignore();
}

void Annotation::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
  if (this->getCurrActive()) {
    this->setCursor(Qt::OpenHandCursor);
  } else
    event->ignore();
}

void Annotation::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
  if (this->getCurrActive()) {
    this->setCursor(Qt::ArrowCursor);
  } else
    event->ignore();
}

void Annotation::hoverMoveEvent(QGraphicsSceneHoverEvent *event) {
  if (this->getCurrActive()) {
    if (!(this->cursor().shape() == Qt::OpenHandCursor))
      this->setCursor(Qt::OpenHandCursor);
  } else
    event->ignore();
}

void Annotation::wheelEvent(QGraphicsSceneWheelEvent *event) {
  if (this->getCurrActive()) {
    if (event->delta() < 0) {
      if (!(m_currDisplayPos >= text_content().length())) {
        m_currDisplayPos += 50;
        update();
      }
    }
    if (event->delta() >= 0) {
      if (m_currDisplayPos > 0) {
        m_currDisplayPos -= 50;
        update();
      }
    }
  } else
    event->ignore();
}
// TODO: should save the changes in the qvideoplayer

void Annotation::modified() {
  this->setText_content(editor->toPlainText());
  update();
  delete modifyDialog;
  this->setAlreadyModifying(false);
}

void Annotation::modifiedDur() {
  QString beginContent = beginEdit->text();
  QString duraContent = durationEdit->text();

  QStringList beginTimes = beginContent.split(tr(":"));
  QStringList durTimes = duraContent.split(tr(":"));

  qint64 beginAnnotation = beginTimes[0].toInt() * 1000 * 60 * 60 +
                           beginTimes[1].toInt() * 1000 * 60 +
                           beginTimes[2].toInt() * 1000;
  qint64 durationTime =
      durTimes[0].toInt() * 1000 * 60 + durTimes[1].toInt() * 1000;

  this->setAppearance_time(beginAnnotation);
  this->setDuration(durationTime);
  update();
  delete durDialog;
  this->setAlreadyModifying(false);
}
void Annotation::canceled() {
  delete modifyDialog;
  this->setAlreadyModifying(false);
}

void Annotation::canceledDur() {
  delete durDialog;
  this->setAlreadyModifying(false);
}

// geters and seters

auto Annotation::getAlreadyModifying() const -> bool {
  return alreadyModifying;
}

void Annotation::setAlreadyModifying(bool value) { alreadyModifying = value; }

auto Annotation::getCurrActive() const -> bool { return currActive; }

void Annotation::setCurrActive(bool value) { currActive = value; }

auto Annotation::getCurrTimeOfVideo() const -> qint64 {
  return currTimeOfVideo;
}

void Annotation::setCurrTimeOfVideo(qint64 value) { currTimeOfVideo = value; }

auto Annotation::duration() const -> qint64 { return m_duration; }

void Annotation::setDuration(qint64 duration) { m_duration = duration; }

auto Annotation::appearance_time() const -> qint64 { return m_appearance_time; }

void Annotation::setAppearance_time(qint64 appearance_time) {
  m_appearance_time = appearance_time;
}

auto Annotation::name() const -> const QString & { return m_name; }

void Annotation::setName(const QString &name) { m_name = std::move(name); }

auto Annotation::text_content() const -> QString { return m_text_content; }

void Annotation::setText_content(const QString &text_content) {
  m_text_content = text_content;
}

auto Annotation::height() const -> qint64 { return m_height; }

void Annotation::setHeight(qint64 height) { m_height = height; }

auto Annotation::width() const -> qint64 { return m_width; }

void Annotation::setWidth(qint64 width) { m_width = width; }

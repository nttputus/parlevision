#include "PipelineElementWidget.h"

#include <list>
#include <algorithm>
#include <QtGui>

#include "PipelineElement.h"
#include "ConnectionLine.h"
#include "PinWidget.h"
#include "Pin.h"
#include "RefPtr.h"

using namespace plvgui;
using namespace plv;
using namespace std;

PipelineElementWidget::PipelineElementWidget(PipelineElement* element,
                                             QGraphicsItem* parent,
                                             Qt::WindowFlags wFlags) :
        QGraphicsObject(parent),
        element(element)
{
    Q_UNUSED(wFlags);
    this->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    this->setFlag(QGraphicsItem::ItemIsMovable, true);
    this->setFlag(QGraphicsItem::ItemIsSelectable, true);

    connect(this, SIGNAL(xChanged()), this, SLOT(savePositionProperties()));
    connect(this, SIGNAL(yChanged()), this, SLOT(savePositionProperties()));

    QGraphicsTextItem* titleLabel = new QGraphicsTextItem(element->getName(), this);
    this->addToGroup(titleLabel);

    setFiltersChildEvents(false);

    // draw all inputpins
    const PipelineElement::InputPinMap& inPins = element->getInputPins();
    int y = 20;
    qreal leftColumnWidth = 0;
    for( PipelineElement::InputPinMap::const_iterator itr = inPins.begin()
        ; itr!=inPins.end(); ++itr)
    {
        RefPtr<IInputPin> pin = itr->second;
        assert(pin.isNotNull());
        PinWidget* pw = new PinWidget(this, pin);
        this->pinWidgets[pin.getPtr()] = pw;
        pw->translate(0, y);
        this->addToGroup(pw);
        leftColumnWidth = max(pw->boundingRect().width(), leftColumnWidth);
        y+=20;
    }


    leftColumnWidth = max(titleLabel->boundingRect().width()/2.0, leftColumnWidth);

    // outputpins should be aligned right.
    // in order to do this, we first make all the widgets,
    // then add them with proper translation.
    const PipelineElement::OutputPinMap& outPins = element->getOutputPins();
    QList<PinWidget*> outWidgets = QList<PinWidget*>();
    qreal maxWidth = 0;
    for( PipelineElement::OutputPinMap::const_iterator itr = outPins.begin()
        ; itr!=outPins.end(); ++itr)
    {
        RefPtr<IOutputPin> pin = itr->second;
        assert(pin.isNotNull());
        PinWidget* pw = new PinWidget(this, pin);
        this->pinWidgets[pin.getPtr()] = pw;
        outWidgets.append(pw);
        maxWidth = max(pw->boundingRect().width(), maxWidth);
    }

    y = 20;
    foreach(PinWidget* pw, outWidgets)
    {
        qreal offset = leftColumnWidth + 20 + maxWidth - pw->boundingRect().width();
        pw->translate(offset, y);
        this->addToGroup(pw);
        y+=20;
    }

    // center the title
    qreal offset = (this->boundingRect().width()-titleLabel->boundingRect().width())/2.0;
    if(offset > 0)
        titleLabel->translate(offset,0);

}

void PipelineElementWidget::addLine(ConnectionLine *line, QString pin)
{
    Q_UNUSED(pin);
    this->lines.append(line);
}

QRectF PipelineElementWidget::boundingRect() const
{
    return childrenBoundingRect();
}

QVariant PipelineElementWidget::itemChange(GraphicsItemChange change,
                     const QVariant &value)
{
    if (change == QGraphicsItem::ItemPositionChange) {
//        qDebug() << "Element changed " << change;
        foreach (ConnectionLine* line, lines) {
            line->updatePosition();
        }
    }

    return value;
}

void PipelineElementWidget::mousePressEvent ( QGraphicsSceneMouseEvent * event )
{
    QGraphicsItem::mousePressEvent(event);
}

void PipelineElementWidget::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
    foreach(QGraphicsItem* child, this->childItems())
    {
        if(child->contains(child->mapFromParent(event->pos())))
        {
            PinWidget* pw = dynamic_cast<PinWidget*> (child);
            if(pw)
            {
                pw->handleMouseDoubleClick();
            }
        }
    }
}


bool PipelineElementWidget::event(QEvent * event)
{
//    qDebug() << "PEW got event " << event;
    return QGraphicsObject::event(event);
}

void PipelineElementWidget::addToGroup(QGraphicsItem* item)
{
    item->setParentItem(this);
    prepareGeometryChange();
    update();
}

void PipelineElementWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);
    Q_UNUSED(option);

    if(this->isSelected())
    {
        //TODO move this outside paint
        QPen pen;
        pen.setStyle(Qt::DashDotLine);
        pen.setWidth(3);
        pen.setColor(Qt::black);
        painter->setPen(pen);
    }
    else
    {
        painter->setPen(Qt::black);
    }

    painter->setBrush(Qt::green);
    painter->drawRoundedRect(this->boundingRect(),8,8, Qt::AbsoluteSize);
}

void PipelineElementWidget::savePositionProperties()
{
    this->element->setProperty("sceneCoordX", this->scenePos().x());
    this->element->setProperty("sceneCoordY", this->scenePos().y());
}

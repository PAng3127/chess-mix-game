#include "gravitypiece.h"
#include <QBrush>
#include <QPen>

GravityPiece::GravityPiece(int player, qreal radius, QGraphicsItem *parent)
    : QGraphicsEllipseItem(-radius, -radius, radius * 2, radius * 2, parent)
    , m_player(player)
{
    // 设置棋子样式
    if (player == 1) {
        setBrush(QBrush(Qt::black));
        setPen(QPen(Qt::darkGray, 1));
    } else {
        setBrush(QBrush(Qt::white));
        setPen(QPen(Qt::gray, 1));
    }
    
    // 添加阴影效果
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(8);
    shadow->setOffset(2, 2);
    shadow->setColor(QColor(0, 0, 0, 100));
    setGraphicsEffect(shadow);
    
    setAcceptHoverEvents(true);
    setFlags(QGraphicsItem::ItemIsSelectable);
}
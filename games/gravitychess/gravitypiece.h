#ifndef GRAVITYPIECE_H
#define GRAVITYPIECE_H

#include <QGraphicsEllipseItem>
#include <QGraphicsDropShadowEffect>
#include <QColor>

class GravityPiece : public QGraphicsEllipseItem
{
public:
    // player: 1=黑色, 2=白色
    GravityPiece(int player, qreal radius, QGraphicsItem *parent = nullptr);
    
    int getPlayer() const { return m_player; }
    void setPlayer(int player) { m_player = player; }
    
private:
    int m_player;  // 1=黑, 2=白
};

#endif // GRAVITYPIECE_H
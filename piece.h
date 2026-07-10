#ifndef PIECE_H
#define PIECE_H

#include <QGraphicsEllipseItem>
#include <QGraphicsDropShadowEffect>
#include <QColor>

class Piece : public QGraphicsEllipseItem
{
public:
    // player: 1=黑色, 2=白色
    Piece(int player, qreal radius, QGraphicsItem *parent = nullptr);

    int getPlayer() const { return m_player; }
    void setPlayer(int player) { m_player = player; }

private:
    int m_player;  // 1=黑, 2=白
};

#endif // PIECE_H

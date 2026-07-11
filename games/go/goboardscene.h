#ifndef GOBOARDSCENE_H
#define GOBOARDSCENE_H

#include <QGraphicsScene>
#include <QVector>
#include <QPoint>
#include <QSet>

class GoBoardScene : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit GoBoardScene(int size = 19, QObject *parent = nullptr);

    // 核心方法：在指定位置落子
    bool placePiece(int col, int row);

    // 重置棋盘
    void reset(int size = 19);

    // 获取棋盘状态
    int getCell(int col, int row) const;
    int getSize() const { return m_size; }
    int getCurrentPlayer() const { return m_currentPlayer; }

    // 获取最后落子的位置
    QPoint getLastMove() const { return m_lastMove; }

    // 检查游戏是否已结束
    bool isGameOver() const { return m_gameOver; }

    // 获取某位置棋子的气
    int getLibertyCount(int col, int row) const;

    // 获取提子数量
    int getCaptures(int player) const { return m_captures[player]; }

signals:
    void piecePlaced(int col, int row, int player);
    void pieceCaptured(int col, int row, int player);
    void gameOver(int winner);
    void currentPlayerChanged(int player);
    void capturesChanged(int black, int white);

private:
    int m_size;
    QVector<QVector<int>> m_board;
    int m_currentPlayer;
    QPoint m_lastMove;
    bool m_gameOver;
    QVector<int> m_captures;  // [0]=黑方提子数, [1]=白方提子数

    // 劫争相关
    QPoint m_koPoint;  // 禁着点
    bool m_koFlag;

    // 棋盘上所有棋子的集合（用于快速查找）
    QSet<QPoint> m_blackStones;
    QSet<QPoint> m_whiteStones;

    // 核心围棋方法
    bool checkKo(int col, int row) const;
    bool isSuicide(int col, int row, int player);  // 移除 const
    QVector<QPoint> getGroup(int col, int row) const;
    QVector<QPoint> getLiberties(const QVector<QPoint> &group) const;
    bool removeCapturedStones(int col, int row, int player);
    int countTerritory(int player) const;
    bool isEye(int col, int row, int player) const;

    // 绘制方法
    void drawBoard();
    void addPieceToScene(int col, int row, int player);
    void removePieceFromScene(int col, int row);
    QPointF getCellCenter(int col, int row) const;
    QSizeF calculateSceneSize() const;
    void drawStarPoints();

    // 查找场景中的棋子
    QGraphicsItem* findPieceAt(int col, int row) const;
};

#endif // GOBOARDSCENE_H

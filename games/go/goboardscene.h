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

    // 玩家Pass（停手）
    bool passTurn();

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

    // 获取游戏结果详情
    QString getGameResult() const;

    // 获取是否已停手
    bool isPassed() const { return m_consecutivePasses >= 2; }

signals:
    void piecePlaced(int col, int row, int player);
    void pieceCaptured(int col, int row, int player);
    void gameOver(int winner, const QString &resultDetail);
    void currentPlayerChanged(int player);
    void capturesChanged(int black, int white);
    void gameInfoChanged(const QString &info);

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

    // Pass计数（连续pass结束）
    int m_consecutivePasses;

    // 棋盘上所有棋子的集合（用于快速查找）
    QSet<QPoint> m_blackStones;
    QSet<QPoint> m_whiteStones;

    // 核心围棋方法
    bool checkKo(int col, int row) const;
    bool isSuicide(int col, int row, int player);
    QVector<QPoint> getGroup(int col, int row) const;
    QVector<QPoint> getLiberties(const QVector<QPoint> &group) const;
    bool removeCapturedStones(int col, int row, int player);
    bool isEye(int col, int row, int player) const;

    // 死活判定
    QSet<QPoint> getDeadStones(int player) const;
    int countDeadStones(int player) const;

    // 计分方法
    int countTerritory(int player) const;
    int countStones(int player) const;
    double calculateScore(int player) const;

    // 绘制方法
    void drawBoard();
    void addPieceToScene(int col, int row, int player);
    void removePieceFromScene(int col, int row);
    QPointF getCellCenter(int col, int row) const;
    QSizeF calculateSceneSize() const;
    void drawStarPoints();

    // 查找场景中的棋子
    QGraphicsItem* findPieceAt(int col, int row) const;

    // 计算胜负
    void calculateAndEmitResult();
};

#endif // GOBOARDSCENE_H

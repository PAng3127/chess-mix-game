#ifndef GOMOKUBOARDSCENE_H
#define GOMOKUBOARDSCENE_H

#include <QGraphicsScene>
#include <QVector>
#include <QPoint>

class GomokuBoardScene : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit GomokuBoardScene(int cols = 15, int rows = 15, int winCount = 5, QObject *parent = nullptr);

    // 核心方法：在指定位置落子（无重力，点击格子落子）
    bool placePiece(int col, int row);

    // 重置棋盘
    void reset(int cols = 15, int rows = 15, int winCount = 5);

    // 获取棋盘状态
    int getCell(int col, int row) const;
    int getCols() const { return m_cols; }
    int getRows() const { return m_rows; }
    int getWinCount() const { return m_winCount; }
    int getCurrentPlayer() const { return m_currentPlayer; }

    // 获取最后落子的位置
    QPoint getLastMove() const { return m_lastMove; }

    // 检查是否棋盘已满
    bool isFull() const;

    // 检查游戏是否已结束
    bool isGameOver() const { return m_gameOver; }

signals:
    void piecePlaced(int col, int row, int player);
    void gameOver(int winner);
    void currentPlayerChanged(int player);

private:
    int m_cols;
    int m_rows;
    int m_winCount;
    QVector<QVector<int>> m_board;
    int m_currentPlayer;
    QPoint m_lastMove;
    bool m_gameOver;

    bool checkWin(int col, int row, int player) const;
    int checkDirection(int col, int row, int dCol, int dRow, int player) const;
    void drawBoard();
    void addPieceToScene(int col, int row, int player);
    QPointF getCellCenter(int col, int row) const;
    QSizeF calculateSceneSize() const;
    void drawStarPoints();
};

#endif // GOMOKUBOARDSCENE_H

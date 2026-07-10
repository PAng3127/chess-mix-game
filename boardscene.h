#ifndef BOARDSCENE_H
#define BOARDSCENE_H

#include <QGraphicsScene>
#include <QVector>
#include <QPoint>

class BoardScene : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit BoardScene(int cols = 7, int rows = 6, int winCount = 4, QObject *parent = nullptr);

    // 核心方法：在指定列落子（重力棋）
    bool placePiece(int col);

    // 重置棋盘
    void reset(int cols = 7, int rows = 6, int winCount = 4);

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
    // 棋子已放置信号
    void piecePlaced(int col, int row, int player);
    // 游戏结束信号 (winner: 0=平局, 1=黑胜, 2=白胜)
    void gameOver(int winner);
    // 当前玩家改变信号
    void currentPlayerChanged(int player);

private:
    int m_cols;          // 列数
    int m_rows;          // 行数
    int m_winCount;      // 胜利所需连子数
    QVector<QVector<int>> m_board;  // 0=空, 1=黑, 2=白
    int m_currentPlayer; // 当前玩家: 1=黑, 2=白
    QPoint m_lastMove;   // 最后落子位置
    bool m_gameOver;     // 游戏是否已结束

    // 查找指定列最低空行
    int findLowestEmpty(int col) const;

    // 检查胜利
    bool checkWin(int col, int row, int player) const;

    // 检查四个方向
    int checkDirection(int col, int row, int dCol, int dRow, int player) const;

    // 更新界面显示（绘制棋盘）
    void drawBoard();

    // 添加棋子到场景
    void addPieceToScene(int col, int row, int player);

    // 获取单元格中心坐标
    QPointF getCellCenter(int col, int row) const;

    // 计算场景大小
    QSizeF calculateSceneSize() const;
};

#endif // BOARDSCENE_H

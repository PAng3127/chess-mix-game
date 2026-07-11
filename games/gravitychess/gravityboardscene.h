#ifndef GRAVITYBOARDSCENE_H
#define GRAVITYBOARDSCENE_H

#include <QGraphicsScene>
#include <QVector>
#include <QPoint>

class GravityBoardScene : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit GravityBoardScene(int cols = 7, int rows = 6, int winCount = 4, QObject *parent = nullptr);
    
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
    
    int findLowestEmpty(int col) const;
    bool checkWin(int col, int row, int player) const;
    int checkDirection(int col, int row, int dCol, int dRow, int player) const;
    void drawBoard();
    void addPieceToScene(int col, int row, int player);
    QPointF getCellCenter(int col, int row) const;
    QSizeF calculateSceneSize() const;
};

#endif // GRAVITYBOARDSCENE_H
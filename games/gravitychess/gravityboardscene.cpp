#include "gravityboardscene.h"
#include "gravitypiece.h"
#include <QGraphicsEllipseItem>
#include <QPen>
#include <QBrush>

GravityBoardScene::GravityBoardScene(int cols, int rows, int winCount, QObject *parent)
    : QGraphicsScene(parent)
    , m_cols(cols)
    , m_rows(rows)
    , m_winCount(winCount)
    , m_currentPlayer(1)
    , m_gameOver(false)
{
    m_board.resize(m_cols);
    for (int col = 0; col < m_cols; ++col) {
        m_board[col].resize(m_rows, 0);
    }
    
    QSizeF sceneSize = calculateSceneSize();
    setSceneRect(0, 0, sceneSize.width(), sceneSize.height());
    drawBoard();
}

void GravityBoardScene::drawBoard()
{
    clear();
    
    QRectF sceneRect = this->sceneRect();
    qreal boardWidth = sceneRect.width();
    qreal boardHeight = sceneRect.height();
    qreal cellSize = qMin(boardWidth / m_cols, boardHeight / m_rows);
    
    // 棋盘背景
    QBrush bgBrush(QColor(222, 184, 135));
    QPen bgPen(Qt::NoPen);
    addRect(0, 0, boardWidth, boardHeight, bgPen, bgBrush);
    
    // 网格线
    QPen gridPen(Qt::black, 1);
    for (int col = 0; col <= m_cols; ++col) {
        qreal x = col * cellSize;
        addLine(x, 0, x, boardHeight, gridPen);
    }
    for (int row = 0; row <= m_rows; ++row) {
        qreal y = row * cellSize;
        addLine(0, y, boardWidth, y, gridPen);
    }
    
    // 棋位圆圈
    QBrush circleBrush(QColor(180, 140, 100));
    QPen circlePen(QColor(150, 110, 70), 1);
    for (int col = 0; col < m_cols; ++col) {
        for (int row = 0; row < m_rows; ++row) {
            qreal x = col * cellSize + cellSize / 2.0;
            qreal y = row * cellSize + cellSize / 2.0;
            qreal radius = cellSize * 0.4;
            QGraphicsEllipseItem *circle = new QGraphicsEllipseItem(
                x - radius, y - radius, radius * 2, radius * 2
            );
            circle->setBrush(circleBrush);
            circle->setPen(circlePen);
            addItem(circle);
        }
    }
    
    // 已有棋子
    for (int col = 0; col < m_cols; ++col) {
        for (int row = 0; row < m_rows; ++row) {
            if (m_board[col][row] != 0) {
                addPieceToScene(col, row, m_board[col][row]);
            }
        }
    }
}

int GravityBoardScene::findLowestEmpty(int col) const
{
    if (col < 0 || col >= m_cols) return -1;
    for (int row = m_rows - 1; row >= 0; --row) {
        if (m_board[col][row] == 0) {
            return row;
        }
    }
    return -1;
}

bool GravityBoardScene::placePiece(int col)
{
    if (m_gameOver) return false;
    if (col < 0 || col >= m_cols) return false;
    
    int row = findLowestEmpty(col);
    if (row == -1) return false;
    
    m_board[col][row] = m_currentPlayer;
    m_lastMove = QPoint(col, row);
    addPieceToScene(col, row, m_currentPlayer);
    
    emit piecePlaced(col, row, m_currentPlayer);
    
    if (checkWin(col, row, m_currentPlayer)) {
        m_gameOver = true;
        emit gameOver(m_currentPlayer);
        return true;
    }
    
    if (isFull()) {
        m_gameOver = true;
        emit gameOver(0);
        return true;
    }
    
    m_currentPlayer = (m_currentPlayer == 1) ? 2 : 1;
    emit currentPlayerChanged(m_currentPlayer);
    return true;
}

bool GravityBoardScene::checkWin(int col, int row, int player) const
{
    int directions[4][2] = {{1,0}, {0,1}, {1,1}, {1,-1}};
    for (int i = 0; i < 4; ++i) {
        int count = 1;
        int dCol = directions[i][0];
        int dRow = directions[i][1];
        count += checkDirection(col, row, dCol, dRow, player);
        count += checkDirection(col, row, -dCol, -dRow, player);
        if (count >= m_winCount) return true;
    }
    return false;
}

int GravityBoardScene::checkDirection(int col, int row, int dCol, int dRow, int player) const
{
    int count = 0;
    int c = col + dCol;
    int r = row + dRow;
    while (c >= 0 && c < m_cols && r >= 0 && r < m_rows) {
        if (m_board[c][r] == player) {
            count++;
            c += dCol;
            r += dRow;
        } else break;
    }
    return count;
}

bool GravityBoardScene::isFull() const
{
    for (int col = 0; col < m_cols; ++col) {
        if (m_board[col][0] == 0) return false;
    }
    return true;
}

void GravityBoardScene::reset(int cols, int rows, int winCount)
{
    m_cols = cols;
    m_rows = rows;
    m_winCount = winCount;
    m_gameOver = false;
    m_currentPlayer = 1;
    m_lastMove = QPoint(-1, -1);
    
    m_board.clear();
    m_board.resize(m_cols);
    for (int col = 0; col < m_cols; ++col) {
        m_board[col].resize(m_rows, 0);
    }
    
    QSizeF sceneSize = calculateSceneSize();
    setSceneRect(0, 0, sceneSize.width(), sceneSize.height());
    drawBoard();
}

int GravityBoardScene::getCell(int col, int row) const
{
    if (col >= 0 && col < m_cols && row >= 0 && row < m_rows) {
        return m_board[col][row];
    }
    return -1;
}

void GravityBoardScene::addPieceToScene(int col, int row, int player)
{
    QPointF center = getCellCenter(col, row);
    QRectF sceneRect = this->sceneRect();
    qreal cellSize = qMin(sceneRect.width() / m_cols, sceneRect.height() / m_rows);
    qreal radius = cellSize * 0.4;
    GravityPiece *piece = new GravityPiece(player, radius);
    piece->setPos(center);
    addItem(piece);
}

QPointF GravityBoardScene::getCellCenter(int col, int row) const
{
    QRectF sceneRect = this->sceneRect();
    qreal cellSize = qMin(sceneRect.width() / m_cols, sceneRect.height() / m_rows);
    qreal x = col * cellSize + cellSize / 2.0;
    qreal y = row * cellSize + cellSize / 2.0;
    return QPointF(x, y);
}

QSizeF GravityBoardScene::calculateSceneSize() const
{
    int cellSize = 60;
    return QSizeF(m_cols * cellSize, m_rows * cellSize);
}
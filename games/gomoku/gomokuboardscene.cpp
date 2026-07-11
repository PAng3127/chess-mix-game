#include "gomokuboardscene.h"
#include "gomokupiece.h"
#include <QGraphicsEllipseItem>
#include <QPen>
#include <QBrush>

GomokuBoardScene::GomokuBoardScene(int cols, int rows, int winCount, QObject *parent)
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

void GomokuBoardScene::drawBoard()
{
    clear();

    QRectF sceneRect = this->sceneRect();
    qreal boardWidth = sceneRect.width();
    qreal boardHeight = sceneRect.height();
    qreal cellSize = qMin(boardWidth / m_cols, boardHeight / m_rows);

    // 计算实际棋盘边距（居中显示）
    qreal boardSize = qMin(boardWidth, boardHeight);
    qreal marginX = (boardWidth - cellSize * (m_cols - 1)) / 2;
    qreal marginY = (boardHeight - cellSize * (m_rows - 1)) / 2;

    // 棋盘背景 - 木质色
    QBrush bgBrush(QColor(222, 184, 135));
    QPen bgPen(Qt::NoPen);
    addRect(0, 0, boardWidth, boardHeight, bgPen, bgBrush);

    // 绘制网格线（五子棋是画在交点上，所以从边距开始）
    QPen gridPen(Qt::black, 1.5);

    // 垂直线
    for (int col = 0; col < m_cols; ++col) {
        qreal x = marginX + col * cellSize;
        addLine(x, marginY, x, marginY + (m_rows - 1) * cellSize, gridPen);
    }

    // 水平线
    for (int row = 0; row < m_rows; ++row) {
        qreal y = marginY + row * cellSize;
        addLine(marginX, y, marginX + (m_cols - 1) * cellSize, y, gridPen);
    }

    // 绘制星位（天元、小目等）
    drawStarPoints();

    // 已有棋子
    for (int col = 0; col < m_cols; ++col) {
        for (int row = 0; row < m_rows; ++row) {
            if (m_board[col][row] != 0) {
                addPieceToScene(col, row, m_board[col][row]);
            }
        }
    }
}

void GomokuBoardScene::drawStarPoints()
{
    QRectF sceneRect = this->sceneRect();
    qreal cellSize = qMin(sceneRect.width() / m_cols, sceneRect.height() / m_rows);
    qreal boardSize = qMin(sceneRect.width(), sceneRect.height());
    qreal marginX = (sceneRect.width() - cellSize * (m_cols - 1)) / 2;
    qreal marginY = (sceneRect.height() - cellSize * (m_rows - 1)) / 2;

    // 定义星位位置（对于标准15×15棋盘）
    QVector<QPoint> starPositions;

    if (m_cols == 15 && m_rows == 15) {
        // 标准五子棋星位
        starPositions = {
            {3, 3}, {11, 3}, {7, 7}, {3, 11}, {11, 11}
        };
    } else if (m_cols >= 9 && m_rows >= 9) {
        // 对于其他尺寸，在四角和中心添加星位
        int midCol = m_cols / 2;
        int midRow = m_rows / 2;
        int edgeOffset = 3;

        if (m_cols > edgeOffset * 2 && m_rows > edgeOffset * 2) {
            starPositions = {
                {edgeOffset, edgeOffset},
                {m_cols - 1 - edgeOffset, edgeOffset},
                {midCol, midRow},
                {edgeOffset, m_rows - 1 - edgeOffset},
                {m_cols - 1 - edgeOffset, m_rows - 1 - edgeOffset}
            };
        }
    }

    // 绘制星位
    QBrush starBrush(Qt::black);
    QPen starPen(Qt::NoPen);
    qreal starRadius = cellSize * 0.12;

    for (const QPoint &pos : starPositions) {
        if (pos.x() < m_cols && pos.y() < m_rows) {
            qreal x = marginX + pos.x() * cellSize;
            qreal y = marginY + pos.y() * cellSize;
            QGraphicsEllipseItem *star = new QGraphicsEllipseItem(
                x - starRadius, y - starRadius, starRadius * 2, starRadius * 2
                );
            star->setBrush(starBrush);
            star->setPen(starPen);
            addItem(star);
        }
    }
}

bool GomokuBoardScene::placePiece(int col, int row)
{
    if (m_gameOver) return false;
    if (col < 0 || col >= m_cols || row < 0 || row >= m_rows) return false;
    if (m_board[col][row] != 0) return false;  // 位置已被占用

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

bool GomokuBoardScene::checkWin(int col, int row, int player) const
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

int GomokuBoardScene::checkDirection(int col, int row, int dCol, int dRow, int player) const
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

bool GomokuBoardScene::isFull() const
{
    for (int col = 0; col < m_cols; ++col) {
        for (int row = 0; row < m_rows; ++row) {
            if (m_board[col][row] == 0) return false;
        }
    }
    return true;
}

void GomokuBoardScene::reset(int cols, int rows, int winCount)
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

int GomokuBoardScene::getCell(int col, int row) const
{
    if (col >= 0 && col < m_cols && row >= 0 && row < m_rows) {
        return m_board[col][row];
    }
    return -1;
}

void GomokuBoardScene::addPieceToScene(int col, int row, int player)
{
    QPointF center = getCellCenter(col, row);
    QRectF sceneRect = this->sceneRect();
    qreal cellSize = qMin(sceneRect.width() / m_cols, sceneRect.height() / m_rows);
    qreal radius = cellSize * 0.42;
    GomokuPiece *piece = new GomokuPiece(player, radius);
    piece->setPos(center);
    addItem(piece);
}

QPointF GomokuBoardScene::getCellCenter(int col, int row) const
{
    QRectF sceneRect = this->sceneRect();
    qreal cellSize = qMin(sceneRect.width() / m_cols, sceneRect.height() / m_rows);
    qreal marginX = (sceneRect.width() - cellSize * (m_cols - 1)) / 2;
    qreal marginY = (sceneRect.height() - cellSize * (m_rows - 1)) / 2;
    qreal x = marginX + col * cellSize;
    qreal y = marginY + row * cellSize;
    return QPointF(x, y);
}

QSizeF GomokuBoardScene::calculateSceneSize() const
{
    int cellSize = 60;
    // 五子棋需要留边距，所以尺寸稍微大一点
    qreal width = (m_cols - 1) * cellSize + cellSize * 1.2;
    qreal height = (m_rows - 1) * cellSize + cellSize * 1.2;
    return QSizeF(width, height);
}

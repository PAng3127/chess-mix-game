#include "boardscene.h"
#include "piece.h"
#include <QGraphicsEllipseItem>
#include <QPen>
#include <QBrush>
#include <QMessageBox>

BoardScene::BoardScene(int cols, int rows, int winCount, QObject *parent)
    : QGraphicsScene(parent)
    , m_cols(cols)
    , m_rows(rows)
    , m_winCount(winCount)
    , m_currentPlayer(1)
    , m_gameOver(false)
{
    // 初始化棋盘
    m_board.resize(m_cols);
    for (int col = 0; col < m_cols; ++col) {
        m_board[col].resize(m_rows, 0);
    }

    // 设置场景大小
    QSizeF sceneSize = calculateSceneSize();
    setSceneRect(0, 0, sceneSize.width(), sceneSize.height());

    // 绘制棋盘
    drawBoard();
}

void BoardScene::drawBoard()
{
    // 清除之前的所有图形项
    clear();

    // 获取当前场景大小
    QRectF sceneRect = this->sceneRect();
    qreal boardWidth = sceneRect.width();
    qreal boardHeight = sceneRect.height();

    // 计算格子大小
    qreal cellSize = qMin(boardWidth / m_cols, boardHeight / m_rows);

    // 绘制棋盘背景
    QBrush bgBrush(QColor(222, 184, 135));  // 木质色
    QPen bgPen(Qt::NoPen);
    addRect(0, 0, boardWidth, boardHeight, bgPen, bgBrush);

    // 绘制网格线
    QPen gridPen(Qt::black, 1);

    // 垂直线
    for (int col = 0; col <= m_cols; ++col) {
        qreal x = col * cellSize;
        addLine(x, 0, x, boardHeight, gridPen);
    }

    // 水平线
    for (int row = 0; row <= m_rows; ++row) {
        qreal y = row * cellSize;
        addLine(0, y, boardWidth, y, gridPen);
    }

    // 绘制棋位圆圈（凹槽效果）
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

    // 重新绘制所有已有的棋子
    for (int col = 0; col < m_cols; ++col) {
        for (int row = 0; row < m_rows; ++row) {
            if (m_board[col][row] != 0) {
                addPieceToScene(col, row, m_board[col][row]);
            }
        }
    }
}

int BoardScene::findLowestEmpty(int col) const
{
    if (col < 0 || col >= m_cols) return -1;

    // 从底部往上找第一个空位
    for (int row = m_rows - 1; row >= 0; --row) {
        if (m_board[col][row] == 0) {
            return row;
        }
    }
    return -1;  // 该列已满
}

bool BoardScene::placePiece(int col)
{
    // 如果游戏已结束，不允许下棋
    if (m_gameOver) {
        return false;
    }

    if (col < 0 || col >= m_cols) {
        return false;
    }

    int row = findLowestEmpty(col);
    if (row == -1) {
        return false;  // 该列已满
    }

    // 放置棋子
    m_board[col][row] = m_currentPlayer;
    m_lastMove = QPoint(col, row);

    // 在场景中添加棋子图形
    addPieceToScene(col, row, m_currentPlayer);

    // 发射棋子放置信号
    emit piecePlaced(col, row, m_currentPlayer);

    // 检查胜利
    if (checkWin(col, row, m_currentPlayer)) {
        m_gameOver = true;
        emit gameOver(m_currentPlayer);
        return true;
    }

    // 检查平局
    if (isFull()) {
        m_gameOver = true;
        emit gameOver(0);  // 平局
        return true;
    }

    // 切换玩家
    m_currentPlayer = (m_currentPlayer == 1) ? 2 : 1;
    emit currentPlayerChanged(m_currentPlayer);

    return true;
}

bool BoardScene::checkWin(int col, int row, int player) const
{
    // 四个方向向量: 水平, 垂直, 主对角线, 副对角线
    int directions[4][2] = {
        {1, 0},   // 水平
        {0, 1},   // 垂直
        {1, 1},   // 主对角线
        {1, -1}   // 副对角线
    };

    for (int i = 0; i < 4; ++i) {
        int count = 1;  // 当前棋子计数
        int dCol = directions[i][0];
        int dRow = directions[i][1];

        // 正方向
        count += checkDirection(col, row, dCol, dRow, player);
        // 反方向
        count += checkDirection(col, row, -dCol, -dRow, player);

        if (count >= m_winCount) {
            return true;
        }
    }

    return false;
}

int BoardScene::checkDirection(int col, int row, int dCol, int dRow, int player) const
{
    int count = 0;
    int c = col + dCol;
    int r = row + dRow;

    while (c >= 0 && c < m_cols && r >= 0 && r < m_rows) {
        if (m_board[c][r] == player) {
            count++;
            c += dCol;
            r += dRow;
        } else {
            break;
        }
    }

    return count;
}

bool BoardScene::isFull() const
{
    for (int col = 0; col < m_cols; ++col) {
        if (m_board[col][0] == 0) {  // 检查最顶部是否有空位
            return false;
        }
    }
    return true;
}

void BoardScene::reset(int cols, int rows, int winCount)
{
    // 更新参数
    m_cols = cols;
    m_rows = rows;
    m_winCount = winCount;
    m_gameOver = false;
    m_currentPlayer = 1;
    m_lastMove = QPoint(-1, -1);

    // 重新初始化棋盘
    m_board.clear();
    m_board.resize(m_cols);
    for (int col = 0; col < m_cols; ++col) {
        m_board[col].resize(m_rows, 0);
    }

    // 重新计算并设置场景大小
    QSizeF sceneSize = calculateSceneSize();
    setSceneRect(0, 0, sceneSize.width(), sceneSize.height());

    // 重新绘制棋盘
    drawBoard();
}

int BoardScene::getCell(int col, int row) const
{
    if (col >= 0 && col < m_cols && row >= 0 && row < m_rows) {
        return m_board[col][row];
    }
    return -1;
}

void BoardScene::addPieceToScene(int col, int row, int player)
{
    QPointF center = getCellCenter(col, row);

    // 获取当前场景大小和格子大小
    QRectF sceneRect = this->sceneRect();
    qreal cellSize = qMin(sceneRect.width() / m_cols, sceneRect.height() / m_rows);
    qreal radius = cellSize * 0.4;

    // 创建棋子
    Piece *piece = new Piece(player, radius);
    piece->setPos(center);

    // 添加到场景
    addItem(piece);
}

QPointF BoardScene::getCellCenter(int col, int row) const
{
    QRectF sceneRect = this->sceneRect();
    qreal cellSize = qMin(sceneRect.width() / m_cols, sceneRect.height() / m_rows);
    qreal x = col * cellSize + cellSize / 2.0;
    qreal y = row * cellSize + cellSize / 2.0;
    return QPointF(x, y);
}

QSizeF BoardScene::calculateSceneSize() const
{
    // 固定每个格子大小为60像素，总大小根据行列数动态计算
    int cellSize = 60;
    return QSizeF(m_cols * cellSize, m_rows * cellSize);
}

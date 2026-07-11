#include "goboardscene.h"
#include "gopiece.h"
#include <QGraphicsEllipseItem>
#include <QPen>
#include <QBrush>
#include <QQueue>
#include <QMessageBox>

GoBoardScene::GoBoardScene(int size, QObject *parent)
    : QGraphicsScene(parent)
    , m_size(size)
    , m_currentPlayer(1)
    , m_gameOver(false)
    , m_captures(3, 0)  // 索引0不用，索引1=黑方提子，索引2=白方提子
    , m_koPoint(-1, -1)
    , m_koFlag(false)
{
    // 初始化棋盘
    m_board.resize(m_size);
    for (int col = 0; col < m_size; ++col) {
        m_board[col].resize(m_size, 0);
    }

    QSizeF sceneSize = calculateSceneSize();
    setSceneRect(0, 0, sceneSize.width(), sceneSize.height());
    drawBoard();
}

void GoBoardScene::drawBoard()
{
    clear();
    m_blackStones.clear();
    m_whiteStones.clear();

    QRectF sceneRect = this->sceneRect();
    qreal boardWidth = sceneRect.width();
    qreal boardHeight = sceneRect.height();
    qreal cellSize = qMin(boardWidth / m_size, boardHeight / m_size);

    // 计算实际棋盘边距（居中显示）
    qreal boardSize = qMin(boardWidth, boardHeight);
    qreal marginX = (boardWidth - cellSize * (m_size - 1)) / 2;
    qreal marginY = (boardHeight - cellSize * (m_size - 1)) / 2;

    // 棋盘背景 - 木质色
    QBrush bgBrush(QColor(222, 184, 135));
    QPen bgPen(Qt::NoPen);
    addRect(0, 0, boardWidth, boardHeight, bgPen, bgBrush);

    // 绘制网格线
    QPen gridPen(Qt::black, 1.5);

    // 垂直线
    for (int col = 0; col < m_size; ++col) {
        qreal x = marginX + col * cellSize;
        addLine(x, marginY, x, marginY + (m_size - 1) * cellSize, gridPen);
    }

    // 水平线
    for (int row = 0; row < m_size; ++row) {
        qreal y = marginY + row * cellSize;
        addLine(marginX, y, marginX + (m_size - 1) * cellSize, y, gridPen);
    }

    // 绘制星位
    drawStarPoints();

    // 已有棋子
    for (int col = 0; col < m_size; ++col) {
        for (int row = 0; row < m_size; ++row) {
            if (m_board[col][row] != 0) {
                addPieceToScene(col, row, m_board[col][row]);
                if (m_board[col][row] == 1) {
                    m_blackStones.insert(QPoint(col, row));
                } else {
                    m_whiteStones.insert(QPoint(col, row));
                }
            }
        }
    }
}

void GoBoardScene::drawStarPoints()
{
    QRectF sceneRect = this->sceneRect();
    qreal cellSize = qMin(sceneRect.width() / m_size, sceneRect.height() / m_size);
    qreal marginX = (sceneRect.width() - cellSize * (m_size - 1)) / 2;
    qreal marginY = (sceneRect.height() - cellSize * (m_size - 1)) / 2;

    // 定义星位位置
    QVector<QPoint> starPositions;

    if (m_size == 19) {
        // 标准19路围棋星位
        starPositions = {
            {3, 3}, {15, 3}, {9, 9}, {3, 15}, {15, 15}
        };
    } else if (m_size == 13) {
        starPositions = {
            {3, 3}, {9, 3}, {6, 6}, {3, 9}, {9, 9}
        };
    } else if (m_size == 9) {
        starPositions = {
            {2, 2}, {6, 2}, {4, 4}, {2, 6}, {6, 6}
        };
    } else if (m_size >= 9) {
        // 对于其他尺寸，在四角和中心添加星位
        int mid = m_size / 2;
        int edgeOffset = 3;
        if (m_size > edgeOffset * 2) {
            starPositions = {
                {edgeOffset, edgeOffset},
                {m_size - 1 - edgeOffset, edgeOffset},
                {mid, mid},
                {edgeOffset, m_size - 1 - edgeOffset},
                {m_size - 1 - edgeOffset, m_size - 1 - edgeOffset}
            };
        }
    }

    // 绘制星位
    QBrush starBrush(Qt::black);
    QPen starPen(Qt::NoPen);
    qreal starRadius = cellSize * 0.1;

    for (const QPoint &pos : starPositions) {
        if (pos.x() < m_size && pos.y() < m_size) {
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

bool GoBoardScene::placePiece(int col, int row)
{
    if (m_gameOver) return false;
    if (col < 0 || col >= m_size || row < 0 || row >= m_size) return false;
    if (m_board[col][row] != 0) return false;  // 位置已被占用

    // 检查劫争
    if (m_koFlag && col == m_koPoint.x() && row == m_koPoint.y()) {
        return false;  // 禁着点
    }

    // 检查是否自杀
    if (isSuicide(col, row, m_currentPlayer)) {
        return false;
    }

    // 落子
    m_board[col][row] = m_currentPlayer;
    if (m_currentPlayer == 1) {
        m_blackStones.insert(QPoint(col, row));
    } else {
        m_whiteStones.insert(QPoint(col, row));
    }
    addPieceToScene(col, row, m_currentPlayer);
    m_lastMove = QPoint(col, row);

    emit piecePlaced(col, row, m_currentPlayer);

    // 清除劫争标记
    m_koFlag = false;
    m_koPoint = QPoint(-1, -1);

    // 检查并移除被吃的棋子
    int opponent = (m_currentPlayer == 1) ? 2 : 1;
    bool captured = false;

    // 检查四个方向
    int directions[4][2] = {{1,0}, {-1,0}, {0,1}, {0,-1}};
    for (int i = 0; i < 4; ++i) {
        int checkCol = col + directions[i][0];
        int checkRow = row + directions[i][1];
        if (checkCol >= 0 && checkCol < m_size && checkRow >= 0 && checkRow < m_size) {
            if (m_board[checkCol][checkRow] == opponent) {
                if (removeCapturedStones(checkCol, checkRow, opponent)) {
                    captured = true;
                }
            }
        }
    }

    // 检查自己是否被吃（提子后可能自己气也变少）
    if (!captured) {
        // 检查刚下的棋子是否有气
        if (getLibertyCount(col, row) == 0) {
            // 自杀，撤回
            m_board[col][row] = 0;
            removePieceFromScene(col, row);
            return false;
        }
    }

    // 检查劫争
    if (captured) {
        // 如果是单子被提，记录劫争点
        // 简单实现：如果只有一个棋子被提，且提子方只提了一个子
        // 这里简化处理，只检查是否形成劫争
        if (m_board[col][row] != 0) {
            // 检查对方是否可以在同一位置提回
            int tempPlayer = m_currentPlayer;
            m_currentPlayer = opponent;
            // 检查对方能否在 col, row 落子提回
            // 简单劫争检测：如果对方在这个位置落子能提回一个子，且没有其他变化
            // 这里使用简化版本
            m_koPoint = QPoint(col, row);
            m_koFlag = true;
        }
    }

    // 检查游戏是否结束（这里简单处理，实际围棋需要更复杂的终局判断）
    // 这里我们简单检查棋盘是否已满
    bool isFull = true;
    for (int c = 0; c < m_size; ++c) {
        for (int r = 0; r < m_size; ++r) {
            if (m_board[c][r] == 0) {
                isFull = false;
                break;
            }
        }
        if (!isFull) break;
    }

    if (isFull) {
        m_gameOver = true;
        // 计分：简单比较提子数
        if (m_captures[1] > m_captures[2]) {
            emit gameOver(1);
        } else if (m_captures[2] > m_captures[1]) {
            emit gameOver(2);
        } else {
            emit gameOver(0);
        }
        return true;
    }

    // 切换玩家
    m_currentPlayer = opponent;
    emit currentPlayerChanged(m_currentPlayer);
    emit capturesChanged(m_captures[1], m_captures[2]);

    return true;
}

bool GoBoardScene::removeCapturedStones(int col, int row, int player)
{
    // 获取该棋子所在的组
    QVector<QPoint> group = getGroup(col, row);
    if (group.isEmpty()) return false;

    // 检查组的气
    QVector<QPoint> liberties = getLiberties(group);
    if (liberties.isEmpty()) {
        // 移除整组棋子
        for (const QPoint &p : group) {
            m_board[p.x()][p.y()] = 0;
            removePieceFromScene(p.x(), p.y());
            if (player == 1) {
                m_blackStones.remove(p);
                m_captures[2]++;  // 白方提黑子
            } else {
                m_whiteStones.remove(p);
                m_captures[1]++;  // 黑方提白子
            }
            emit pieceCaptured(p.x(), p.y(), player);
        }
        return true;
    }
    return false;
}

QVector<QPoint> GoBoardScene::getGroup(int col, int row) const
{
    QVector<QPoint> group;
    if (col < 0 || col >= m_size || row < 0 || row >= m_size) return group;
    if (m_board[col][row] == 0) return group;

    int player = m_board[col][row];
    QQueue<QPoint> queue;
    QSet<QPoint> visited;

    queue.enqueue(QPoint(col, row));
    visited.insert(QPoint(col, row));

    int directions[4][2] = {{1,0}, {-1,0}, {0,1}, {0,-1}};

    while (!queue.isEmpty()) {
        QPoint current = queue.dequeue();
        group.append(current);

        for (int i = 0; i < 4; ++i) {
            int newCol = current.x() + directions[i][0];
            int newRow = current.y() + directions[i][1];
            QPoint next(newCol, newRow);

            if (newCol >= 0 && newCol < m_size && newRow >= 0 && newRow < m_size) {
                if (!visited.contains(next) && m_board[newCol][newRow] == player) {
                    queue.enqueue(next);
                    visited.insert(next);
                }
            }
        }
    }

    return group;
}

QVector<QPoint> GoBoardScene::getLiberties(const QVector<QPoint> &group) const
{
    QSet<QPoint> liberties;

    int directions[4][2] = {{1,0}, {-1,0}, {0,1}, {0,-1}};

    for (const QPoint &p : group) {
        for (int i = 0; i < 4; ++i) {
            int newCol = p.x() + directions[i][0];
            int newRow = p.y() + directions[i][1];
            if (newCol >= 0 && newCol < m_size && newRow >= 0 && newRow < m_size) {
                if (m_board[newCol][newRow] == 0) {
                    liberties.insert(QPoint(newCol, newRow));
                }
            }
        }
    }

    return liberties.values().toVector();
}

int GoBoardScene::getLibertyCount(int col, int row) const
{
    if (col < 0 || col >= m_size || row < 0 || row >= m_size) return 0;
    if (m_board[col][row] == 0) return 0;

    QVector<QPoint> group = getGroup(col, row);
    QVector<QPoint> liberties = getLiberties(group);
    return liberties.size();
}

bool GoBoardScene::isSuicide(int col, int row, int player)  // 移除 const
{
    // 模拟落子
    int oldValue = m_board[col][row];
    m_board[col][row] = player;

    // 检查新棋子是否有气
    int libertyCount = getLibertyCount(col, row);

    // 检查是否提掉对方棋子
    int opponent = (player == 1) ? 2 : 1;
    bool willCapture = false;
    int directions[4][2] = {{1,0}, {-1,0}, {0,1}, {0,-1}};
    for (int i = 0; i < 4; ++i) {
        int checkCol = col + directions[i][0];
        int checkRow = row + directions[i][1];
        if (checkCol >= 0 && checkCol < m_size && checkRow >= 0 && checkRow < m_size) {
            if (m_board[checkCol][checkRow] == opponent) {
                QVector<QPoint> group = getGroup(checkCol, checkRow);
                QVector<QPoint> liberties = getLiberties(group);
                if (liberties.isEmpty()) {
                    willCapture = true;
                    break;
                }
            }
        }
    }

    // 恢复棋盘状态
    m_board[col][row] = oldValue;

    // 如果有气或者能提子，则不是自杀
    return (libertyCount == 0 && !willCapture);
}

bool GoBoardScene::checkKo(int col, int row) const
{
    return m_koFlag && (col == m_koPoint.x() && row == m_koPoint.y());
}

bool GoBoardScene::isEye(int col, int row, int player) const
{
    // 简单判断是否为眼：周围四个点都是己方棋子
    int directions[4][2] = {{1,0}, {-1,0}, {0,1}, {0,-1}};
    for (int i = 0; i < 4; ++i) {
        int newCol = col + directions[i][0];
        int newRow = row + directions[i][1];
        if (newCol >= 0 && newCol < m_size && newRow >= 0 && newRow < m_size) {
            if (m_board[newCol][newRow] != player) {
                return false;
            }
        }
    }
    return true;
}

int GoBoardScene::countTerritory(int player) const
{
    // 简化版计地：计算所有空位周围棋子颜色
    int territory = 0;
    for (int col = 0; col < m_size; ++col) {
        for (int row = 0; row < m_size; ++row) {
            if (m_board[col][row] == 0) {
                // 检查周围四个方向的棋子
                int directions[4][2] = {{1,0}, {-1,0}, {0,1}, {0,-1}};
                bool isPlayerTerritory = true;
                for (int i = 0; i < 4; ++i) {
                    int newCol = col + directions[i][0];
                    int newRow = row + directions[i][1];
                    if (newCol >= 0 && newCol < m_size && newRow >= 0 && newRow < m_size) {
                        if (m_board[newCol][newRow] != 0 && m_board[newCol][newRow] != player) {
                            isPlayerTerritory = false;
                            break;
                        }
                    }
                }
                if (isPlayerTerritory) {
                    territory++;
                }
            }
        }
    }
    return territory;
}

void GoBoardScene::reset(int size)
{
    m_size = size;
    m_gameOver = false;
    m_currentPlayer = 1;
    m_lastMove = QPoint(-1, -1);
    m_captures[1] = 0;
    m_captures[2] = 0;
    m_koFlag = false;
    m_koPoint = QPoint(-1, -1);
    m_blackStones.clear();
    m_whiteStones.clear();

    m_board.clear();
    m_board.resize(m_size);
    for (int col = 0; col < m_size; ++col) {
        m_board[col].resize(m_size, 0);
    }

    QSizeF sceneSize = calculateSceneSize();
    setSceneRect(0, 0, sceneSize.width(), sceneSize.height());
    drawBoard();
}

int GoBoardScene::getCell(int col, int row) const
{
    if (col >= 0 && col < m_size && row >= 0 && row < m_size) {
        return m_board[col][row];
    }
    return -1;
}

void GoBoardScene::addPieceToScene(int col, int row, int player)
{
    QPointF center = getCellCenter(col, row);
    QRectF sceneRect = this->sceneRect();
    qreal cellSize = qMin(sceneRect.width() / m_size, sceneRect.height() / m_size);
    qreal radius = cellSize * 0.42;
    GoPiece *piece = new GoPiece(player, radius);
    piece->setPos(center);
    addItem(piece);
}

void GoBoardScene::removePieceFromScene(int col, int row)
{
    QGraphicsItem *item = findPieceAt(col, row);
    if (item) {
        removeItem(item);
        delete item;
    }
}

QGraphicsItem* GoBoardScene::findPieceAt(int col, int row) const
{
    QPointF center = getCellCenter(col, row);
    QRectF sceneRect = this->sceneRect();
    qreal cellSize = qMin(sceneRect.width() / m_size, sceneRect.height() / m_size);
    qreal radius = cellSize * 0.42;

    // 搜索场景中的所有棋子
    QList<QGraphicsItem*> items = this->items();
    for (QGraphicsItem *item : items) {
        GoPiece *piece = dynamic_cast<GoPiece*>(item);
        if (piece) {
            QPointF pos = piece->pos();
            if (qAbs(pos.x() - center.x()) < radius && qAbs(pos.y() - center.y()) < radius) {
                return item;
            }
        }
    }
    return nullptr;
}

QPointF GoBoardScene::getCellCenter(int col, int row) const
{
    QRectF sceneRect = this->sceneRect();
    qreal cellSize = qMin(sceneRect.width() / m_size, sceneRect.height() / m_size);
    qreal marginX = (sceneRect.width() - cellSize * (m_size - 1)) / 2;
    qreal marginY = (sceneRect.height() - cellSize * (m_size - 1)) / 2;
    qreal x = marginX + col * cellSize;
    qreal y = marginY + row * cellSize;
    return QPointF(x, y);
}

QSizeF GoBoardScene::calculateSceneSize() const
{
    int cellSize = 60;
    qreal width = (m_size - 1) * cellSize + cellSize * 1.2;
    qreal height = (m_size - 1) * cellSize + cellSize * 1.2;
    return QSizeF(width, height);
}

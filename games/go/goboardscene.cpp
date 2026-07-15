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
    , m_captures(3, 0)
    , m_koPoint(-1, -1)
    , m_koFlag(false)
    , m_consecutivePasses(0)
{
    m_board.resize(m_size);
    for (int col = 0; col < m_size; ++col) {
        m_board[col].resize(m_size, 0);
    }

    QSizeF sceneSize = calculateSceneSize();
    setSceneRect(0, 0, sceneSize.width(), sceneSize.height());
    drawBoard();
}

// ==================== 核心落子逻辑 ====================

bool GoBoardScene::placePiece(int col, int row)
{
    if (m_gameOver) return false;
    if (col < 0 || col >= m_size || row < 0 || row >= m_size) return false;
    if (m_board[col][row] != 0) return false;

    // 重置pass计数
    m_consecutivePasses = 0;

    // 检查劫争
    if (m_koFlag && col == m_koPoint.x() && row == m_koPoint.y()) {
        emit gameInfoChanged("⚡ 劫争！不能立即提回");
        return false;
    }

    // 检查是否自杀
    if (isSuicide(col, row, m_currentPlayer)) {
        emit gameInfoChanged("❌ 禁止自杀！");
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

    // 检查自己是否被吃
    if (!captured) {
        if (getLibertyCount(col, row) == 0) {
            m_board[col][row] = 0;
            removePieceFromScene(col, row);
            emit gameInfoChanged("❌ 自杀！撤回");
            return false;
        }
    }

    // 检查劫争
    if (captured) {
        // 检查是否形成劫争（单子被提）
        int captureCount = 0;
        for (int i = 0; i < 4; ++i) {
            int checkCol = col + directions[i][0];
            int checkRow = row + directions[i][1];
            if (checkCol >= 0 && checkCol < m_size && checkRow >= 0 && checkRow < m_size) {
                if (m_board[checkCol][checkRow] == m_currentPlayer) {
                    captureCount++;
                }
            }
        }
        // 如果只提了一个子，记录劫争
        if (captureCount == 1 && m_board[col][row] != 0) {
            m_koPoint = QPoint(col, row);
            m_koFlag = true;
            emit gameInfoChanged("⚠️ 劫争形成！");
        }
    }

    // 发送游戏信息
    QString info = QString("第 %1 手 | 黑提 %2 子 | 白提 %3 子")
                       .arg(m_blackStones.size() + m_whiteStones.size())
                       .arg(m_captures[1])
                       .arg(m_captures[2]);
    emit gameInfoChanged(info);

    // 切换玩家
    m_currentPlayer = opponent;
    emit currentPlayerChanged(m_currentPlayer);
    emit capturesChanged(m_captures[1], m_captures[2]);

    return true;
}

// ==================== Pass功能 ====================

bool GoBoardScene::passTurn()
{
    if (m_gameOver) return false;

    m_consecutivePasses++;
    emit gameInfoChanged(QString("⏸️ 停手 #%1").arg(m_consecutivePasses));

    // 如果双方都Pass，游戏结束
    if (m_consecutivePasses >= 2) {
        calculateAndEmitResult();
        return true;
    }

    // 切换玩家
    int opponent = (m_currentPlayer == 1) ? 2 : 1;
    m_currentPlayer = opponent;
    emit currentPlayerChanged(m_currentPlayer);

    return true;
}

// ==================== 提子逻辑 ====================

bool GoBoardScene::removeCapturedStones(int col, int row, int player)
{
    QVector<QPoint> group = getGroup(col, row);
    if (group.isEmpty()) return false;

    QVector<QPoint> liberties = getLiberties(group);
    if (liberties.isEmpty()) {
        for (const QPoint &p : group) {
            m_board[p.x()][p.y()] = 0;
            removePieceFromScene(p.x(), p.y());
            if (player == 1) {
                m_blackStones.remove(p);
                m_captures[2]++;
            } else {
                m_whiteStones.remove(p);
                m_captures[1]++;
            }
            emit pieceCaptured(p.x(), p.y(), player);
        }
        return true;
    }
    return false;
}

// ==================== 组和气的计算 ====================

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

// ==================== 自杀检测 ====================

bool GoBoardScene::isSuicide(int col, int row, int player)
{
    int oldValue = m_board[col][row];
    m_board[col][row] = player;

    int libertyCount = getLibertyCount(col, row);

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

    m_board[col][row] = oldValue;

    return (libertyCount == 0 && !willCapture);
}

// ==================== 死活判定 ====================

QSet<QPoint> GoBoardScene::getDeadStones(int player) const
{
    QSet<QPoint> deadStones;

    for (int col = 0; col < m_size; ++col) {
        for (int row = 0; row < m_size; ++row) {
            if (m_board[col][row] == player) {
                QVector<QPoint> group = getGroup(col, row);
                QVector<QPoint> liberties = getLiberties(group);
                if (liberties.isEmpty()) {
                    for (const QPoint &p : group) {
                        deadStones.insert(p);
                    }
                }
            }
        }
    }

    return deadStones;
}

int GoBoardScene::countDeadStones(int player) const
{
    return getDeadStones(player).size();
}

// ==================== 计分系统 ====================

int GoBoardScene::countTerritory(int player) const
{
    int territory = 0;

    QVector<QVector<bool>> visited(m_size, QVector<bool>(m_size, false));
    int directions[4][2] = {{1,0}, {-1,0}, {0,1}, {0,-1}};

    for (int col = 0; col < m_size; ++col) {
        for (int row = 0; row < m_size; ++row) {
            if (m_board[col][row] == 0 && !visited[col][row]) {
                QQueue<QPoint> queue;
                QSet<QPoint> region;
                queue.enqueue(QPoint(col, row));
                visited[col][row] = true;
                region.insert(QPoint(col, row));

                bool isPlayerTerritory = true;
                bool hasBlackBorder = false;
                bool hasWhiteBorder = false;

                while (!queue.isEmpty()) {
                    QPoint current = queue.dequeue();

                    for (int i = 0; i < 4; ++i) {
                        int newCol = current.x() + directions[i][0];
                        int newRow = current.y() + directions[i][1];
                        if (newCol >= 0 && newCol < m_size && newRow >= 0 && newRow < m_size) {
                            if (m_board[newCol][newRow] == 0 && !visited[newCol][newRow]) {
                                queue.enqueue(QPoint(newCol, newRow));
                                visited[newCol][newRow] = true;
                                region.insert(QPoint(newCol, newRow));
                            } else if (m_board[newCol][newRow] == 1) {
                                hasBlackBorder = true;
                            } else if (m_board[newCol][newRow] == 2) {
                                hasWhiteBorder = true;
                            }
                        }
                    }
                }

                // 只有被同一方包围的空地才算地盘
                if (hasBlackBorder && !hasWhiteBorder) {
                    territory += region.size();
                } else if (hasWhiteBorder && !hasBlackBorder) {
                    territory += region.size();
                }
            }
        }
    }

    return territory;
}

int GoBoardScene::countStones(int player) const
{
    int count = 0;
    for (int col = 0; col < m_size; ++col) {
        for (int row = 0; row < m_size; ++row) {
            if (m_board[col][row] == player) {
                count++;
            }
        }
    }
    return count;
}

double GoBoardScene::calculateScore(int player) const
{
    // 中国规则：棋子 + 地盘 - 贴目
    double komi = 3.75;  // 白棋贴3.75子
    int stones = countStones(player);
    int territory = countTerritory(player);
    double score = stones + territory;

    if (player == 2) {
        score += komi;  // 白棋加贴目
    }

    return score;
}

// ==================== 计算结果 ====================

void GoBoardScene::calculateAndEmitResult()
{
    if (m_gameOver) return;

    m_gameOver = true;

    // 移除死子
    QSet<QPoint> deadBlack = getDeadStones(1);
    QSet<QPoint> deadWhite = getDeadStones(2);

    for (const QPoint &p : deadBlack) {
        m_board[p.x()][p.y()] = 0;
        removePieceFromScene(p.x(), p.y());
        m_blackStones.remove(p);
        m_captures[2]++;
    }
    for (const QPoint &p : deadWhite) {
        m_board[p.x()][p.y()] = 0;
        removePieceFromScene(p.x(), p.y());
        m_whiteStones.remove(p);
        m_captures[1]++;
    }

    // 计算得分
    double blackScore = calculateScore(1);
    double whiteScore = calculateScore(2);

    // 详细结果
    int blackStones = countStones(1);
    int whiteStones = countStones(2);
    int blackTerritory = countTerritory(1);
    int whiteTerritory = countTerritory(2);
    int deadBlackCount = deadBlack.size();
    int deadWhiteCount = deadWhite.size();

    QString resultDetail;
    int winner;

    if (blackScore > whiteScore) {
        winner = 1;
        resultDetail = QString(
                           "🏆 黑棋胜！\n\n"
                           "━━━━━━━━━━━━━━━━━━━━━━━━━━\n"
                           "📊 详细计分（中国规则）\n"
                           "━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n"
                           "⚫ 黑棋：\n"
                           "  • 棋子：%1 子\n"
                           "  • 地盘：%2 目\n"
                           "  • 提子：%3 子\n"
                           "  • 小计：%4 子\n\n"
                           "⚪ 白棋：\n"
                           "  • 棋子：%5 子\n"
                           "  • 地盘：%6 目\n"
                           "  • 提子：%7 子\n"
                           "  • 贴目：+3.75 子\n"
                           "  • 小计：%8 子\n\n"
                           "━━━━━━━━━━━━━━━━━━━━━━━━━━\n"
                           "📌 结果：黑胜 %.1f 子"
                           )
                           .arg(blackStones)
                           .arg(blackTerritory)
                           .arg(m_captures[1])
                           .arg(blackStones + blackTerritory)
                           .arg(whiteStones)
                           .arg(whiteTerritory)
                           .arg(m_captures[2])
                           .arg(whiteStones + whiteTerritory + 3.75)
                           .arg(blackScore - whiteScore);
    } else if (whiteScore > blackScore) {
        winner = 2;
        resultDetail = QString(
                           "🏆 白棋胜！\n\n"
                           "━━━━━━━━━━━━━━━━━━━━━━━━━━\n"
                           "📊 详细计分（中国规则）\n"
                           "━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n"
                           "⚫ 黑棋：\n"
                           "  • 棋子：%1 子\n"
                           "  • 地盘：%2 目\n"
                           "  • 提子：%3 子\n"
                           "  • 小计：%4 子\n\n"
                           "⚪ 白棋：\n"
                           "  • 棋子：%5 子\n"
                           "  • 地盘：%6 目\n"
                           "  • 提子：%7 子\n"
                           "  • 贴目：+3.75 子\n"
                           "  • 小计：%8 子\n\n"
                           "━━━━━━━━━━━━━━━━━━━━━━━━━━\n"
                           "📌 结果：白胜 %.1f 子"
                           )
                           .arg(blackStones)
                           .arg(blackTerritory)
                           .arg(m_captures[1])
                           .arg(blackStones + blackTerritory)
                           .arg(whiteStones)
                           .arg(whiteTerritory)
                           .arg(m_captures[2])
                           .arg(whiteStones + whiteTerritory + 3.75)
                           .arg(whiteScore - blackScore);
    } else {
        winner = 0;
        resultDetail = "🤝 平局！\n\n双方实力相当！";
    }

    emit gameOver(winner, resultDetail);
    emit gameInfoChanged("🏁 游戏结束！");
}

// ==================== 获取结果 ====================

QString GoBoardScene::getGameResult() const
{
    if (!m_gameOver) return "游戏未结束";

    double blackScore = calculateScore(1);
    double whiteScore = calculateScore(2);

    if (blackScore > whiteScore) {
        return QString("黑胜 %.1f 子").arg(blackScore - whiteScore);
    } else if (whiteScore > blackScore) {
        return QString("白胜 %.1f 子").arg(whiteScore - blackScore);
    } else {
        return "平局";
    }
}

// ==================== 其他方法 ====================

bool GoBoardScene::checkKo(int col, int row) const
{
    return m_koFlag && (col == m_koPoint.x() && row == m_koPoint.y());
}

bool GoBoardScene::isEye(int col, int row, int player) const
{
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
    m_consecutivePasses = 0;
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

    emit gameInfoChanged("🔄 游戏已重置");
}

// ==================== 绘制方法 ====================

void GoBoardScene::drawBoard()
{
    clear();
    m_blackStones.clear();
    m_whiteStones.clear();

    QRectF sceneRect = this->sceneRect();
    qreal boardWidth = sceneRect.width();
    qreal boardHeight = sceneRect.height();
    qreal cellSize = qMin(boardWidth / m_size, boardHeight / m_size);

    qreal boardSize = qMin(boardWidth, boardHeight);
    qreal marginX = (boardWidth - cellSize * (m_size - 1)) / 2;
    qreal marginY = (boardHeight - cellSize * (m_size - 1)) / 2;

    QBrush bgBrush(QColor(222, 184, 135));
    QPen bgPen(Qt::NoPen);
    addRect(0, 0, boardWidth, boardHeight, bgPen, bgBrush);

    QPen gridPen(Qt::black, 1.5);

    for (int col = 0; col < m_size; ++col) {
        qreal x = marginX + col * cellSize;
        addLine(x, marginY, x, marginY + (m_size - 1) * cellSize, gridPen);
    }
    for (int row = 0; row < m_size; ++row) {
        qreal y = marginY + row * cellSize;
        addLine(marginX, y, marginX + (m_size - 1) * cellSize, y, gridPen);
    }

    drawStarPoints();

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

    QVector<QPoint> starPositions;

    if (m_size == 19) {
        starPositions = {{3, 3}, {15, 3}, {9, 9}, {3, 15}, {15, 15}};
    } else if (m_size == 13) {
        starPositions = {{3, 3}, {9, 3}, {6, 6}, {3, 9}, {9, 9}};
    } else if (m_size == 9) {
        starPositions = {{2, 2}, {6, 2}, {4, 4}, {2, 6}, {6, 6}};
    } else if (m_size >= 9) {
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

int GoBoardScene::getCell(int col, int row) const
{
    if (col >= 0 && col < m_size && row >= 0 && row < m_size) {
        return m_board[col][row];
    }
    return -1;
}

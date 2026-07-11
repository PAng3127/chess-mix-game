#include "gomokuboardview.h"
#include <QGraphicsScene>

GomokuBoardView::GomokuBoardView(QWidget *parent)
    : QGraphicsView(parent)
    , m_boardScene(nullptr)
    , m_cellSize(60)
{
    setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::SmoothPixmapTransform);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFrameStyle(QFrame::NoFrame);
    setBackgroundBrush(QColor(240, 230, 210));
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorViewCenter);
}

GomokuBoardView::~GomokuBoardView() {}

void GomokuBoardView::setBoardScene(GomokuBoardScene *scene)
{
    m_boardScene = scene;
    setScene(scene);
    resetView();
}

void GomokuBoardView::resetView()
{
    if (!m_boardScene) return;
    QRectF sceneRect = m_boardScene->sceneRect();
    if (sceneRect.isEmpty()) {
        sceneRect = QRectF(0, 0, 500, 500);
    }
    fitInView(sceneRect, Qt::KeepAspectRatio);
    if (sceneRect.width() > 0 && m_boardScene->getCols() > 0) {
        m_cellSize = qRound(sceneRect.width() / m_boardScene->getCols());
    }
}

void GomokuBoardView::updateView()
{
    resetView();
    update();
}

void GomokuBoardView::mousePressEvent(QMouseEvent *event)
{
    if (!m_boardScene) {
        QGraphicsView::mousePressEvent(event);
        return;
    }
    QPointF scenePos = mapToScene(event->pos());
    QPoint gridPos = getGridFromPos(scenePos);
    if (gridPos.x() >= 0 && gridPos.y() >= 0) {
        m_boardScene->placePiece(gridPos.x(), gridPos.y());
    }
    QGraphicsView::mousePressEvent(event);
}

QPoint GomokuBoardView::getGridFromPos(const QPointF &pos) const
{
    if (!m_boardScene) return QPoint(-1, -1);

    int cols = m_boardScene->getCols();
    int rows = m_boardScene->getRows();
    QRectF sceneRect = m_boardScene->sceneRect();
    qreal cellSize = qMin(sceneRect.width() / cols, sceneRect.height() / rows);
    qreal marginX = (sceneRect.width() - cellSize * (cols - 1)) / 2;
    qreal marginY = (sceneRect.height() - cellSize * (rows - 1)) / 2;

    // 计算点击位置对应的行列
    int col = qRound((pos.x() - marginX) / cellSize);
    int row = qRound((pos.y() - marginY) / cellSize);

    // 检查是否在有效范围内（使用更精确的点击检测）
    qreal tolerance = cellSize * 0.45;
    qreal targetX = marginX + col * cellSize;
    qreal targetY = marginY + row * cellSize;

    if (col >= 0 && col < cols && row >= 0 && row < rows) {
        if (qAbs(pos.x() - targetX) <= tolerance && qAbs(pos.y() - targetY) <= tolerance) {
            return QPoint(col, row);
        }
    }

    return QPoint(-1, -1);
}

void GomokuBoardView::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
    resetView();
}

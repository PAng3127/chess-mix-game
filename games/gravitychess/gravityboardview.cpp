#include "gravityboardview.h"
#include <QGraphicsScene>

GravityBoardView::GravityBoardView(QWidget *parent)
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

GravityBoardView::~GravityBoardView() {}

void GravityBoardView::setBoardScene(GravityBoardScene *scene)
{
    m_boardScene = scene;
    setScene(scene);
    resetView();
}

void GravityBoardView::resetView()
{
    if (!m_boardScene) return;
    QRectF sceneRect = m_boardScene->sceneRect();
    if (sceneRect.isEmpty()) {
        sceneRect = QRectF(0, 0, 500, 400);
    }
    fitInView(sceneRect, Qt::KeepAspectRatio);
    if (sceneRect.width() > 0 && m_boardScene->getCols() > 0) {
        m_cellSize = qRound(sceneRect.width() / m_boardScene->getCols());
    }
}

void GravityBoardView::updateView()
{
    resetView();
    update();
}

void GravityBoardView::mousePressEvent(QMouseEvent *event)
{
    if (!m_boardScene) {
        QGraphicsView::mousePressEvent(event);
        return;
    }
    QPointF scenePos = mapToScene(event->pos());
    int col = getColumnFromPos(scenePos);
    if (col >= 0) {
        m_boardScene->placePiece(col);
    }
    QGraphicsView::mousePressEvent(event);
}

int GravityBoardView::getColumnFromPos(const QPointF &pos) const
{
    if (!m_boardScene) return -1;
    int cols = m_boardScene->getCols();
    QRectF sceneRect = m_boardScene->sceneRect();
    qreal cellWidth = sceneRect.width() / cols;
    int col = static_cast<int>(pos.x() / cellWidth);
    if (col >= 0 && col < cols) return col;
    return -1;
}

void GravityBoardView::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
    resetView();
}
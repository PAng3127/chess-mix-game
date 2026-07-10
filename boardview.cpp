#include "boardview.h"
#include <QGraphicsScene>
#include <QTransform>

BoardView::BoardView(QWidget *parent)
    : QGraphicsView(parent)
    , m_boardScene(nullptr)
    , m_cellSize(60)
{
    // 设置视图属性
    setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::SmoothPixmapTransform);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFrameStyle(QFrame::NoFrame);

    // 设置背景色
    setBackgroundBrush(QColor(240, 230, 210));

    // 设置缩放模式为保持宽高比
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorViewCenter);
}

BoardView::~BoardView()
{
    // 不需要手动删除 scene，Qt 会自动管理
}

void BoardView::setBoardScene(BoardScene *scene)
{
    m_boardScene = scene;
    setScene(scene);

    // 调整视图大小
    resetView();
}

void BoardView::resetView()
{
    if (!m_boardScene) return;

    // 计算场景的矩形区域
    QRectF sceneRect = m_boardScene->sceneRect();

    // 如果场景为空或大小为0，则使用默认大小
    if (sceneRect.isEmpty()) {
        sceneRect = QRectF(0, 0, 500, 400);
    }

    // 在视图中居中显示整个场景，保持宽高比
    fitInView(sceneRect, Qt::KeepAspectRatio);

    // 更新单元格大小（用于后续的坐标计算）
    if (sceneRect.width() > 0 && m_boardScene->getCols() > 0) {
        m_cellSize = qRound(sceneRect.width() / m_boardScene->getCols());
    }
}

void BoardView::updateView()
{
    // 强制更新视图
    resetView();
    update();
}

void BoardView::mousePressEvent(QMouseEvent *event)
{
    if (!m_boardScene) {
        QGraphicsView::mousePressEvent(event);
        return;
    }

    // 将鼠标位置转换为场景坐标
    QPointF scenePos = mapToScene(event->pos());

    // 获取点击的列
    int col = getColumnFromPos(scenePos);
    if (col >= 0) {
        m_boardScene->placePiece(col);
    }

    QGraphicsView::mousePressEvent(event);
}

int BoardView::getColumnFromPos(const QPointF &pos) const
{
    if (!m_boardScene) return -1;

    int cols = m_boardScene->getCols();
    int rows = m_boardScene->getRows();

    // 计算每个格子的大小
    QRectF sceneRect = m_boardScene->sceneRect();
    qreal cellWidth = sceneRect.width() / cols;
    qreal cellHeight = sceneRect.height() / rows;

    // 计算点击位置对应的列
    int col = static_cast<int>(pos.x() / cellWidth);

    // 检查是否在有效范围内
    if (col >= 0 && col < cols) {
        return col;
    }

    return -1;
}

void BoardView::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
    resetView();
}

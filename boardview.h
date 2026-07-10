#ifndef BOARDVIEW_H
#define BOARDVIEW_H

#include <QGraphicsView>
#include <QMouseEvent>
#include <QResizeEvent>
#include "boardscene.h"

class BoardView : public QGraphicsView
{
    Q_OBJECT

public:
    explicit BoardView(QWidget *parent = nullptr);
    ~BoardView();

    // 设置棋盘场景
    void setBoardScene(BoardScene *scene);

    // 获取当前棋盘场景
    BoardScene* getBoardScene() const { return m_boardScene; }

    // 重置视图（自适应大小）
    void resetView();

    // 强制更新视图
    void updateView();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    BoardScene *m_boardScene;
    int m_cellSize;

    // 将鼠标坐标转换为列索引
    int getColumnFromPos(const QPointF &pos) const;
};

#endif // BOARDVIEW_H

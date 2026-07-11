#ifndef GOBOARDVIEW_H
#define GOBOARDVIEW_H

#include <QGraphicsView>
#include <QMouseEvent>
#include <QResizeEvent>
#include "goboardscene.h"

class GoBoardView : public QGraphicsView
{
    Q_OBJECT

public:
    explicit GoBoardView(QWidget *parent = nullptr);
    ~GoBoardView();

    void setBoardScene(GoBoardScene *scene);
    GoBoardScene* getBoardScene() const { return m_boardScene; }
    void resetView();
    void updateView();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    GoBoardScene *m_boardScene;
    int m_cellSize;
    QPoint getGridFromPos(const QPointF &pos) const;
};

#endif // GOBOARDVIEW_H

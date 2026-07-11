#ifndef GOMOKUBOARDVIEW_H
#define GOMOKUBOARDVIEW_H

#include <QGraphicsView>
#include <QMouseEvent>
#include <QResizeEvent>
#include "gomokuboardscene.h"

class GomokuBoardView : public QGraphicsView
{
    Q_OBJECT

public:
    explicit GomokuBoardView(QWidget *parent = nullptr);
    ~GomokuBoardView();

    void setBoardScene(GomokuBoardScene *scene);
    GomokuBoardScene* getBoardScene() const { return m_boardScene; }
    void resetView();
    void updateView();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    GomokuBoardScene *m_boardScene;
    int m_cellSize;
    QPoint getGridFromPos(const QPointF &pos) const;
};

#endif // GOMOKUBOARDVIEW_H

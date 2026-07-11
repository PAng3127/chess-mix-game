#ifndef GRAVITYBOARDVIEW_H
#define GRAVITYBOARDVIEW_H

#include <QGraphicsView>
#include <QMouseEvent>
#include <QResizeEvent>
#include "gravityboardscene.h"

class GravityBoardView : public QGraphicsView
{
    Q_OBJECT

public:
    explicit GravityBoardView(QWidget *parent = nullptr);
    ~GravityBoardView();
    
    void setBoardScene(GravityBoardScene *scene);
    GravityBoardScene* getBoardScene() const { return m_boardScene; }
    void resetView();
    void updateView();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    GravityBoardScene *m_boardScene;
    int m_cellSize;
    int getColumnFromPos(const QPointF &pos) const;
};

#endif // GRAVITYBOARDVIEW_H
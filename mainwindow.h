#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QToolBar>
#include <QAction>
#include "boardview.h"
#include "boardscene.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // 重新开始游戏
    void onResetClicked();

    // 更新状态显示
    void updateStatus(int player);

    // 游戏结束处理
    void onGameOver(int winner);

    // 打开设置对话框
    void onSettingsClicked();

private:
    void setupUI();
    void initGame();
    void createMenuBar();
    void createToolBar();
    void createStatusBar();
    void updateToolBarInfo();  // 添加这个声明

    BoardView *m_boardView;
    BoardScene *m_boardScene;
    QLabel *m_statusLabel;

    // 当前设置
    int m_currentCols;
    int m_currentRows;
    int m_currentWinCount;
};

#endif // MAINWINDOW_H

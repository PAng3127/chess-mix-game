#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QLabel>
#include "games/gravitychess/gravityboardscene.h"
#include "games/gravitychess/gravityboardview.h"
#include "games/gomoku/gomokuboardscene.h"
#include "games/gomoku/gomokuboardview.h"
#include "games/go/goboardscene.h"
#include "games/go/goboardview.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onGameSelected(const QString &gameId);
    void onBackToMenu();
    void onResetGame();
    void onSettingsClicked();

private:
    void setupUI();
    void createMenuPage();
    void createGamePage();
    void initGravityChess();
    void initGomoku();
    void initGo();
    void updateToolBarInfo();
    void updateStatus(int player);
    void onGameOver(int winner);

    QStackedWidget *m_stackedWidget;

    // 菜单页
    QWidget *m_menuPage;

    // 游戏页
    QWidget *m_gamePage;
    QWidget *m_boardContainer;

    // 三个视图
    GravityBoardView *m_gravityView;
    GomokuBoardView *m_gomokuView;
    GoBoardView *m_goView;

    // 当前场景
    QGraphicsScene *m_currentScene;

    QLabel *m_statusLabel;
    QLabel *m_captureLabel;  // 显示提子数

    // 游戏设置
    int m_currentCols;
    int m_currentRows;
    int m_currentWinCount;
    QString m_currentGameId;
    QString m_gameType;
};

#endif // MAINWINDOW_H

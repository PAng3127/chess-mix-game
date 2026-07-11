#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QLabel>
#include "games/gravitychess/gravityboardscene.h"
#include "games/gravitychess/gravityboardview.h"

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
    void updateToolBarInfo();
    void updateStatus(int player);
    void onGameOver(int winner);
    
    QStackedWidget *m_stackedWidget;
    
    // 菜单页
    QWidget *m_menuPage;
    
    // 游戏页
    QWidget *m_gamePage;
    GravityBoardView *m_boardView;
    GravityBoardScene *m_boardScene;
    QLabel *m_statusLabel;
    
    // 游戏设置
    int m_currentCols;
    int m_currentRows;
    int m_currentWinCount;
    QString m_currentGameId;
};

#endif // MAINWINDOW_H
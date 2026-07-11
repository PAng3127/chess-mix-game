#include "mainwindow.h"
#include "widgets/gamebutton.h"
#include "widgets/settingsdialog.h"
#include "games/gomoku/gomokuboardscene.h"
#include "games/gomoku/gomokuboardview.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QStatusBar>
#include <QMessageBox>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_stackedWidget(nullptr)      // 添加这些成员变量的初始化
    , m_menuPage(nullptr)
    , m_gamePage(nullptr)
    , m_boardContainer(nullptr)
    , m_gravityView(nullptr)
    , m_gomokuView(nullptr)
    , m_currentScene(nullptr)
    , m_statusLabel(nullptr)
    , m_currentCols(8)
    , m_currentRows(8)
    , m_currentWinCount(4)
    , m_currentGameId("")
    , m_gameType("重力棋")
{
    setupUI();
}

MainWindow::~MainWindow() {}

void MainWindow::setupUI()
{
    setWindowTitle("棋类游戏合集");
    setMinimumSize(800, 600);

    // 创建菜单栏
    QMenuBar *menuBar = new QMenuBar(this);
    QMenu *gameMenu = menuBar->addMenu("游戏");
    QAction *backAction = new QAction("返回主菜单", this);
    connect(backAction, &QAction::triggered, this, &MainWindow::onBackToMenu);
    gameMenu->addAction(backAction);
    gameMenu->addSeparator();
    QAction *exitAction = new QAction("退出", this);
    connect(exitAction, &QAction::triggered, this, &QWidget::close);
    gameMenu->addAction(exitAction);
    setMenuBar(menuBar);

    // 创建堆叠窗口
    m_stackedWidget = new QStackedWidget(this);
    setCentralWidget(m_stackedWidget);

    // 创建页面
    createMenuPage();
    createGamePage();

    // 显示菜单页
    m_stackedWidget->setCurrentWidget(m_menuPage);
}

void MainWindow::createMenuPage()
{
    m_menuPage = new QWidget(this);

    QVBoxLayout *mainLayout = new QVBoxLayout(m_menuPage);
    mainLayout->setAlignment(Qt::AlignCenter);
    mainLayout->setSpacing(30);

    // 标题
    QLabel *titleLabel = new QLabel("🎮 棋类游戏合集", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(32);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setStyleSheet("color: #2c3e50; margin: 20px;");
    mainLayout->addWidget(titleLabel);

    // 游戏按钮容器
    QWidget *buttonContainer = new QWidget(this);
    QHBoxLayout *buttonLayout = new QHBoxLayout(buttonContainer);
    buttonLayout->setAlignment(Qt::AlignCenter);
    buttonLayout->setSpacing(40);

    // 重力棋按钮
    GameButton *gravityBtn = new GameButton(
        "重力棋",
        "经典四子棋玩法\n棋子受重力影响",
        "",
        this
        );
    connect(gravityBtn, &GameButton::gameSelected, this, &MainWindow::onGameSelected);
    buttonLayout->addWidget(gravityBtn);

    // 五子棋按钮
    GameButton *gomokuBtn = new GameButton(
        "五子棋",
        "15×15棋盘\n五子连珠获胜",
        "",
        this
        );
    connect(gomokuBtn, &GameButton::gameSelected, this, &MainWindow::onGameSelected);
    buttonLayout->addWidget(gomokuBtn);

    // 围棋按钮（预留）
    GameButton *goBtn = new GameButton(
        "围棋",
        "经典围棋玩法\n气、提子、劫争\n(开发中...)",
        "",
        this
        );
    goBtn->setEnabled(false);
    goBtn->setStyleSheet(goBtn->styleSheet() + " opacity: 0.6;");
    connect(goBtn, &GameButton::gameSelected, this, &MainWindow::onGameSelected);
    buttonLayout->addWidget(goBtn);

    mainLayout->addWidget(buttonContainer);

    // 版本信息
    QLabel *versionLabel = new QLabel("v1.0.0 | 使用Qt开发", this);
    versionLabel->setAlignment(Qt::AlignCenter);
    versionLabel->setStyleSheet("color: #999; font-size: 11px; margin-top: 30px;");
    mainLayout->addWidget(versionLabel);

    m_stackedWidget->addWidget(m_menuPage);
}

void MainWindow::createGamePage()
{
    m_gamePage = new QWidget(this);

    QVBoxLayout *mainLayout = new QVBoxLayout(m_gamePage);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(10);

    // 工具栏
    QHBoxLayout *toolbarLayout = new QHBoxLayout();

    // 返回按钮
    QPushButton *backBtn = new QPushButton("← 返回", this);
    connect(backBtn, &QPushButton::clicked, this, &MainWindow::onBackToMenu);
    toolbarLayout->addWidget(backBtn);

    toolbarLayout->addSpacing(20);

    // 重置按钮
    QPushButton *resetBtn = new QPushButton("重新开始", this);
    connect(resetBtn, &QPushButton::clicked, this, &MainWindow::onResetGame);
    toolbarLayout->addWidget(resetBtn);

    // 设置按钮
    QPushButton *settingsBtn = new QPushButton("设置", this);
    connect(settingsBtn, &QPushButton::clicked, this, &MainWindow::onSettingsClicked);
    toolbarLayout->addWidget(settingsBtn);

    toolbarLayout->addStretch();

    // 状态标签
    m_statusLabel = new QLabel("当前回合: 黑棋", this);
    m_statusLabel->setAlignment(Qt::AlignRight);
    QFont statusFont = m_statusLabel->font();
    statusFont.setPointSize(12);
    statusFont.setBold(true);
    m_statusLabel->setFont(statusFont);
    m_statusLabel->setStyleSheet("padding: 5px 15px; background-color: #f0f0f0; border-radius: 5px;");
    toolbarLayout->addWidget(m_statusLabel);

    mainLayout->addLayout(toolbarLayout);

    // 棋盘视图容器（用于动态切换）
    m_boardContainer = new QWidget(this);
    QVBoxLayout *containerLayout = new QVBoxLayout(m_boardContainer);
    containerLayout->setContentsMargins(0, 0, 0, 0);

    // 创建两个视图，但只显示一个
    m_gravityView = new GravityBoardView(this);
    m_gravityView->setMinimumSize(400, 400);
    m_gravityView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    containerLayout->addWidget(m_gravityView);

    m_gomokuView = new GomokuBoardView(this);
    m_gomokuView->setMinimumSize(400, 400);
    m_gomokuView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_gomokuView->hide();
    containerLayout->addWidget(m_gomokuView);

    mainLayout->addWidget(m_boardContainer);

    m_stackedWidget->addWidget(m_gamePage);
}

void MainWindow::onGameSelected(const QString &gameId)
{
    m_currentGameId = gameId;

    // 隐藏所有视图
    m_gravityView->hide();
    m_gomokuView->hide();

    if (gameId == "重力棋") {
        m_gameType = "重力棋";
        m_currentCols = 8;
        m_currentRows = 8;
        m_currentWinCount = 4;
        initGravityChess();
        m_gravityView->show();
        setWindowTitle("重力棋 - 棋类游戏合集");
        statusBar()->showMessage("游戏已启动：重力棋");
    } else if (gameId == "五子棋") {
        m_gameType = "五子棋";
        m_currentCols = 15;
        m_currentRows = 15;
        m_currentWinCount = 5;
        initGomoku();
        m_gomokuView->show();
        setWindowTitle("五子棋 - 棋类游戏合集");
        statusBar()->showMessage("游戏已启动：五子棋");
    }

    m_stackedWidget->setCurrentWidget(m_gamePage);
}

void MainWindow::initGravityChess()
{
    // 清理旧的场景
    if (m_gravityView->getBoardScene()) {
        delete m_gravityView->getBoardScene();
        m_gravityView->setBoardScene(nullptr);
    }

    // 创建新场景
    GravityBoardScene *scene = new GravityBoardScene(m_currentCols, m_currentRows, m_currentWinCount, this);
    connect(scene, &GravityBoardScene::currentPlayerChanged,
            this, &MainWindow::updateStatus);
    connect(scene, &GravityBoardScene::gameOver,
            this, &MainWindow::onGameOver);

    m_gravityView->setBoardScene(scene);
    m_currentScene = scene;
    updateStatus(scene->getCurrentPlayer());
    updateToolBarInfo();
}

void MainWindow::initGomoku()
{
    // 清理旧的场景
    if (m_gomokuView->getBoardScene()) {
        delete m_gomokuView->getBoardScene();
        m_gomokuView->setBoardScene(nullptr);
    }

    // 创建新场景
    GomokuBoardScene *scene = new GomokuBoardScene(m_currentCols, m_currentRows, m_currentWinCount, this);
    connect(scene, &GomokuBoardScene::currentPlayerChanged,
            this, &MainWindow::updateStatus);
    connect(scene, &GomokuBoardScene::gameOver,
            this, &MainWindow::onGameOver);

    m_gomokuView->setBoardScene(scene);
    m_currentScene = scene;
    updateStatus(scene->getCurrentPlayer());
    updateToolBarInfo();
}

void MainWindow::updateToolBarInfo()
{
    if (m_currentScene) {
        QString info = QString("%1 | 棋盘: %2×%3 | 连子: %4")
                           .arg(m_gameType).arg(m_currentCols).arg(m_currentRows).arg(m_currentWinCount);
        statusBar()->showMessage(info, 3000);
    }
}

void MainWindow::updateStatus(int player)
{
    // 注意：这里需要将 m_currentScene 转换为具体的类型来调用 isGameOver()
    // 但由于我们使用的是 QGraphicsScene 基类，需要判断实际类型
    bool isGameOver = false;
    if (m_gameType == "重力棋") {
        GravityBoardScene *scene = qobject_cast<GravityBoardScene*>(m_currentScene);
        if (scene) isGameOver = scene->isGameOver();
    } else if (m_gameType == "五子棋") {
        GomokuBoardScene *scene = qobject_cast<GomokuBoardScene*>(m_currentScene);
        if (scene) isGameOver = scene->isGameOver();
    }

    if (isGameOver) {
        return;
    }

    if (player == 1) {
        m_statusLabel->setText("⚫ 当前回合: 黑棋");
        m_statusLabel->setStyleSheet("padding: 5px 15px; background-color: #f0f0f0; border-radius: 5px; color: black;");
    } else {
        m_statusLabel->setText("⚪ 当前回合: 白棋");
        m_statusLabel->setStyleSheet("padding: 5px 15px; background-color: #f0f0f0; border-radius: 5px; color: #666;");
    }
}

void MainWindow::onGameOver(int winner)
{
    QString message;
    QString style;

    if (winner == 1) {
        message = "⚫ 黑棋胜利！";
        style = "padding: 5px 15px; background-color: #4CAF50; border-radius: 5px; color: white;";
        statusBar()->showMessage("黑棋胜利！");
    } else if (winner == 2) {
        message = "⚪ 白棋胜利！";
        style = "padding: 5px 15px; background-color: #4CAF50; border-radius: 5px; color: white;";
        statusBar()->showMessage("白棋胜利！");
    } else {
        message = "平局！";
        style = "padding: 5px 15px; background-color: #FF9800; border-radius: 5px; color: white;";
        statusBar()->showMessage("平局！");
    }

    m_statusLabel->setText(message);
    m_statusLabel->setStyleSheet(style);

    QMessageBox::information(this, "游戏结束", message);
}

void MainWindow::onBackToMenu()
{
    // 清理场景
    if (m_gravityView->getBoardScene()) {
        delete m_gravityView->getBoardScene();
        m_gravityView->setBoardScene(nullptr);
    }
    if (m_gomokuView->getBoardScene()) {
        delete m_gomokuView->getBoardScene();
        m_gomokuView->setBoardScene(nullptr);
    }
    m_currentScene = nullptr;

    setWindowTitle("棋类游戏合集");
    m_stackedWidget->setCurrentWidget(m_menuPage);
    statusBar()->showMessage("返回主菜单");
}

void MainWindow::onResetGame()
{
    if (m_currentScene) {
        // 根据游戏类型调用对应的 reset 方法
        if (m_gameType == "重力棋") {
            GravityBoardScene *scene = qobject_cast<GravityBoardScene*>(m_currentScene);
            if (scene) {
                scene->reset(m_currentCols, m_currentRows, m_currentWinCount);
                m_gravityView->updateView();
                updateStatus(scene->getCurrentPlayer());
            }
        } else if (m_gameType == "五子棋") {
            GomokuBoardScene *scene = qobject_cast<GomokuBoardScene*>(m_currentScene);
            if (scene) {
                scene->reset(m_currentCols, m_currentRows, m_currentWinCount);
                m_gomokuView->updateView();
                updateStatus(scene->getCurrentPlayer());
            }
        }
        updateToolBarInfo();
        statusBar()->showMessage("游戏已重置");
    }
}

void MainWindow::onSettingsClicked()
{
    SettingsDialog dialog(m_gameType, m_currentCols, m_currentRows, m_currentWinCount, this);
    if (dialog.exec() == QDialog::Accepted) {
        int newCols = dialog.getCols();
        int newRows = dialog.getRows();
        int newWinCount = dialog.getWinCount();

        if (newCols != m_currentCols || newRows != m_currentRows || newWinCount != m_currentWinCount) {
            m_currentCols = newCols;
            m_currentRows = newRows;
            m_currentWinCount = newWinCount;

            if (m_currentScene) {
                // 根据游戏类型调用对应的 reset 方法
                if (m_gameType == "重力棋") {
                    GravityBoardScene *scene = qobject_cast<GravityBoardScene*>(m_currentScene);
                    if (scene) {
                        scene->reset(m_currentCols, m_currentRows, m_currentWinCount);
                        m_gravityView->updateView();
                        updateStatus(scene->getCurrentPlayer());
                    }
                } else if (m_gameType == "五子棋") {
                    GomokuBoardScene *scene = qobject_cast<GomokuBoardScene*>(m_currentScene);
                    if (scene) {
                        scene->reset(m_currentCols, m_currentRows, m_currentWinCount);
                        m_gomokuView->updateView();
                        updateStatus(scene->getCurrentPlayer());
                    }
                }
                updateToolBarInfo();
                statusBar()->showMessage("设置已应用");
            }
        }
    }
}

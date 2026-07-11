#include "mainwindow.h"
#include "widgets/gamebutton.h"
#include "widgets/settingsdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QMessageBox>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_boardView(nullptr)
    , m_boardScene(nullptr)
    , m_currentCols(8)      // 默认改为 8
    , m_currentRows(8)      // 默认改为 8
    , m_currentWinCount(4)
    , m_currentGameId("")
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

    // 五子棋按钮（预留）
    GameButton *gomokuBtn = new GameButton(
        "五子棋",
        "15×15棋盘\n五子连珠获胜\n(开发中...)",
        "",
        this
        );
    gomokuBtn->setEnabled(false);
    gomokuBtn->setStyleSheet(gomokuBtn->styleSheet() + " opacity: 0.6;");
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

    // 棋盘视图
    m_boardView = new GravityBoardView(this);
    m_boardView->setMinimumSize(400, 400);
    m_boardView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mainLayout->addWidget(m_boardView);

    m_stackedWidget->addWidget(m_gamePage);
}

void MainWindow::onGameSelected(const QString &gameId)
{
    m_currentGameId = gameId;

    if (gameId == "重力棋") {
        // 重置为重力棋默认设置
        m_currentCols = 8;
        m_currentRows = 8;
        m_currentWinCount = 4;
        initGravityChess();
        setWindowTitle("重力棋 - 棋类游戏合集");
        m_stackedWidget->setCurrentWidget(m_gamePage);
        statusBar()->showMessage("游戏已启动：重力棋");
    }
}

void MainWindow::initGravityChess()
{
    // 清理旧的场景
    if (m_boardScene) {
        delete m_boardScene;
        m_boardScene = nullptr;
    }

    // 创建新场景
    m_boardScene = new GravityBoardScene(m_currentCols, m_currentRows, m_currentWinCount, this);
    connect(m_boardScene, &GravityBoardScene::currentPlayerChanged,
            this, &MainWindow::updateStatus);
    connect(m_boardScene, &GravityBoardScene::gameOver,
            this, &MainWindow::onGameOver);

    m_boardView->setBoardScene(m_boardScene);
    updateStatus(m_boardScene->getCurrentPlayer());
    updateToolBarInfo();
}

void MainWindow::updateToolBarInfo()
{
    if (m_boardScene) {
        QString info = QString("棋盘: %1×%2 | 连子: %3")
                           .arg(m_currentCols).arg(m_currentRows).arg(m_currentWinCount);
        statusBar()->showMessage(info, 3000);
    }
}

void MainWindow::updateStatus(int player)
{
    if (m_boardScene && m_boardScene->isGameOver()) {
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
    if (m_boardScene) {
        delete m_boardScene;
        m_boardScene = nullptr;
    }
    setWindowTitle("棋类游戏合集");
    m_stackedWidget->setCurrentWidget(m_menuPage);
    statusBar()->showMessage("返回主菜单");
}

void MainWindow::onResetGame()
{
    if (m_boardScene) {
        m_boardScene->reset(m_currentCols, m_currentRows, m_currentWinCount);
        m_boardView->updateView();
        updateStatus(m_boardScene->getCurrentPlayer());
        updateToolBarInfo();
        statusBar()->showMessage("游戏已重置");
    }
}

void MainWindow::onSettingsClicked()
{
    // 获取当前游戏类型
    QString gameType = m_currentGameId;
    if (gameType.isEmpty()) {
        gameType = "重力棋";
    }

    SettingsDialog dialog(gameType, m_currentCols, m_currentRows, m_currentWinCount, this);
    if (dialog.exec() == QDialog::Accepted) {
        int newCols = dialog.getCols();
        int newRows = dialog.getRows();
        int newWinCount = dialog.getWinCount();

        if (newCols != m_currentCols || newRows != m_currentRows || newWinCount != m_currentWinCount) {
            m_currentCols = newCols;
            m_currentRows = newRows;
            m_currentWinCount = newWinCount;

            if (m_boardScene) {
                m_boardScene->reset(m_currentCols, m_currentRows, m_currentWinCount);
                m_boardView->updateView();
                updateStatus(m_boardScene->getCurrentPlayer());
                updateToolBarInfo();
                statusBar()->showMessage("设置已应用");
            }
        }
    }
}

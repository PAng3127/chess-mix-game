#include "mainwindow.h"
#include "settingsdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_boardView(nullptr)
    , m_boardScene(nullptr)
    , m_currentCols(7)
    , m_currentRows(6)
    , m_currentWinCount(4)
{
    setupUI();
    initGame();
}

MainWindow::~MainWindow()
{
    // Qt 会自动清理子对象
}

void MainWindow::setupUI()
{
    // 设置窗口标题和大小
    setWindowTitle("棋类游戏 - 重力棋");
    setMinimumSize(600, 700);

    // 创建菜单栏
    createMenuBar();

    // 创建工具栏
    createToolBar();

    // 创建状态栏
    createStatusBar();

    // 创建中心部件
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // 创建主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    // 创建状态标签（放在棋盘上方）
    m_statusLabel = new QLabel("当前回合: 黑棋", this);
    m_statusLabel->setAlignment(Qt::AlignCenter);
    QFont statusFont = m_statusLabel->font();
    statusFont.setPointSize(14);
    statusFont.setBold(true);
    m_statusLabel->setFont(statusFont);
    m_statusLabel->setStyleSheet("padding: 10px; background-color: #f0f0f0; border-radius: 5px;");
    mainLayout->addWidget(m_statusLabel);

    // 创建棋盘视图
    m_boardView = new BoardView(this);
    m_boardView->setMinimumSize(400, 400);
    mainLayout->addWidget(m_boardView);
}

void MainWindow::createMenuBar()
{
    // 创建菜单栏
    QMenuBar *menuBar = new QMenuBar(this);

    // 游戏菜单
    QMenu *gameMenu = menuBar->addMenu("游戏");

    QAction *newGameAction = new QAction("重新开始", this);
    newGameAction->setShortcut(QKeySequence::New);
    connect(newGameAction, &QAction::triggered, this, &MainWindow::onResetClicked);
    gameMenu->addAction(newGameAction);

    gameMenu->addSeparator();

    QAction *settingsAction = new QAction("设置...", this);
    settingsAction->setShortcut(QKeySequence::Preferences);
    connect(settingsAction, &QAction::triggered, this, &MainWindow::onSettingsClicked);
    gameMenu->addAction(settingsAction);

    gameMenu->addSeparator();

    QAction *exitAction = new QAction("退出", this);
    exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, &QAction::triggered, this, &QWidget::close);
    gameMenu->addAction(exitAction);

    // 帮助菜单
    QMenu *helpMenu = menuBar->addMenu("帮助");

    QAction *aboutAction = new QAction("关于", this);
    connect(aboutAction, &QAction::triggered, [this]() {
        QMessageBox::about(this, "关于游戏",
                           "<h2>重力棋</h2>"
                           "<p>一个基于Qt开发的重力棋游戏</p>"
                           "<p><b>规则：</b></p>"
                           "<ul>"
                           "<li>棋子受重力影响，只能落到最低空位</li>"
                           "<li>率先连成指定数量的棋子获胜</li>"
                           "<li>支持自定义棋盘大小和连子数</li>"
                           "</ul>"
                           "<p><b>操作：</b></p>"
                           "<ul>"
                           "<li>点击列落子</li>"
                           "<li>菜单栏调整设置</li>"
                           "</ul>");
    });
    helpMenu->addAction(aboutAction);

    setMenuBar(menuBar);
}

void MainWindow::createToolBar()
{
    // 创建工具栏
    QToolBar *toolBar = new QToolBar("工具栏", this);
    toolBar->setMovable(false);

    // 重新开始按钮
    QAction *newGameAction = new QAction("重新开始", this);
    newGameAction->setToolTip("重新开始游戏");
    connect(newGameAction, &QAction::triggered, this, &MainWindow::onResetClicked);
    toolBar->addAction(newGameAction);

    toolBar->addSeparator();

    // 设置按钮
    QAction *settingsAction = new QAction("设置", this);
    settingsAction->setToolTip("打开游戏设置");
    connect(settingsAction, &QAction::triggered, this, &MainWindow::onSettingsClicked);
    toolBar->addAction(settingsAction);

    toolBar->addSeparator();

    // 当前设置信息
    QLabel *infoLabel = new QLabel(" 棋盘: 7×6 | 连子: 4 ", this);
    infoLabel->setStyleSheet("color: #666; font-size: 11px;");
    infoLabel->setObjectName("infoLabel");
    toolBar->addWidget(infoLabel);

    // 添加弹性空间，让工具栏元素靠左
    QWidget *spacer = new QWidget(this);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    toolBar->addWidget(spacer);

    // 添加当前回合状态到工具栏
    QLabel *turnLabel = new QLabel("⚫ 黑棋回合", this);
    turnLabel->setStyleSheet("font-weight: bold; font-size: 12px;");
    turnLabel->setObjectName("turnLabel");
    toolBar->addWidget(turnLabel);

    addToolBar(toolBar);
}

void MainWindow::createStatusBar()
{
    // 创建状态栏
    statusBar()->showMessage("黑棋先下");
}

void MainWindow::initGame()
{
    // 创建棋盘场景
    m_boardScene = new BoardScene(m_currentCols, m_currentRows, m_currentWinCount, this);

    // 连接信号槽
    connect(m_boardScene, &BoardScene::currentPlayerChanged,
            this, &MainWindow::updateStatus);
    connect(m_boardScene, &BoardScene::gameOver,
            this, &MainWindow::onGameOver);

    // 设置到视图
    m_boardView->setBoardScene(m_boardScene);

    // 更新状态
    updateStatus(m_boardScene->getCurrentPlayer());

    // 更新工具栏显示
    updateToolBarInfo();
}

void MainWindow::updateToolBarInfo()
{
    // 更新工具栏中的信息标签
    QList<QToolBar*> toolBars = findChildren<QToolBar*>();
    for (QToolBar *toolBar : toolBars) {
        QList<QWidget*> widgets = toolBar->findChildren<QWidget*>();
        for (QWidget *widget : widgets) {
            if (widget->objectName() == "infoLabel") {
                QLabel *label = qobject_cast<QLabel*>(widget);
                if (label) {
                    label->setText(QString(" 棋盘: %1×%2 | 连子: %3 ")
                                       .arg(m_currentCols).arg(m_currentRows).arg(m_currentWinCount));
                }
            } else if (widget->objectName() == "turnLabel") {
                QLabel *label = qobject_cast<QLabel*>(widget);
                if (label && m_boardScene) {
                    int player = m_boardScene->getCurrentPlayer();
                    if (player == 1) {
                        label->setText("⚫ 黑棋回合");
                    } else {
                        label->setText("⚪ 白棋回合");
                    }
                }
            }
        }
    }
}

void MainWindow::onResetClicked()
{
    if (m_boardScene) {
        m_boardScene->reset(m_currentCols, m_currentRows, m_currentWinCount);
        // 强制更新视图以适应新棋盘大小
        m_boardView->updateView();
        updateStatus(m_boardScene->getCurrentPlayer());
        updateToolBarInfo();
        statusBar()->showMessage("游戏重新开始");
    }
}

void MainWindow::onSettingsClicked()
{
    // 创建并显示设置对话框
    SettingsDialog dialog(m_currentCols, m_currentRows, m_currentWinCount, this);

    if (dialog.exec() == QDialog::Accepted) {
        // 获取新设置
        int newCols = dialog.getCols();
        int newRows = dialog.getRows();
        int newWinCount = dialog.getWinCount();

        // 检查是否有变化
        if (newCols != m_currentCols || newRows != m_currentRows || newWinCount != m_currentWinCount) {
            // 更新设置
            m_currentCols = newCols;
            m_currentRows = newRows;
            m_currentWinCount = newWinCount;

            // 重置游戏
            if (m_boardScene) {
                m_boardScene->reset(m_currentCols, m_currentRows, m_currentWinCount);
                // 强制更新视图以适应新棋盘大小
                m_boardView->updateView();
                updateStatus(m_boardScene->getCurrentPlayer());
                updateToolBarInfo();
                statusBar()->showMessage("设置已应用");
            }
        }
    }
}

void MainWindow::updateStatus(int player)
{
    if (m_boardScene && m_boardScene->isGameOver()) {
        // 如果游戏已结束，不更新状态
        return;
    }

    QString text;
    QString style;

    if (player == 1) {
        text = "⚫ 当前回合: 黑棋";
        style = "padding: 10px; background-color: #f0f0f0; border-radius: 5px; color: black;";
    } else {
        text = "⚪ 当前回合: 白棋";
        style = "padding: 10px; background-color: #f0f0f0; border-radius: 5px; color: #666;";
    }

    m_statusLabel->setText(text);
    m_statusLabel->setStyleSheet(style);

    // 更新工具栏
    updateToolBarInfo();
}

void MainWindow::onGameOver(int winner)
{
    QString message;
    QString title = "游戏结束";
    QString statusMessage;
    QString style;

    if (winner == 1) {
        message = "⚫ 黑棋胜利！";
        statusMessage = "黑棋胜利！";
        style = "padding: 10px; background-color: #4CAF50; border-radius: 5px; color: white;";
    } else if (winner == 2) {
        message = "⚪ 白棋胜利！";
        statusMessage = "白棋胜利！";
        style = "padding: 10px; background-color: #4CAF50; border-radius: 5px; color: white;";
    } else {
        message = "平局！";
        statusMessage = "平局！";
        style = "padding: 10px; background-color: #FF9800; border-radius: 5px; color: white;";
    }

    // 更新状态标签
    m_statusLabel->setText(message);
    m_statusLabel->setStyleSheet(style);

    // 更新状态栏
    statusBar()->showMessage(statusMessage);

    // 更新工具栏
    updateToolBarInfo();

    // 弹窗提示
    QMessageBox::information(this, title, message);
}

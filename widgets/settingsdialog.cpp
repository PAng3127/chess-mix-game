#include "settingsdialog.h"

SettingsDialog::SettingsDialog(const QString &gameType, int cols, int rows, int winCount, QWidget *parent)
    : QDialog(parent)
    , m_currentGameType(gameType)
{
    setWindowTitle("游戏设置");
    setModal(true);
    setMinimumWidth(350);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QFormLayout *formLayout = new QFormLayout();

    // 游戏类型选择（只读，显示当前游戏）
    m_gameTypeCombo = new QComboBox(this);
    m_gameTypeCombo->addItem("重力棋");
    m_gameTypeCombo->addItem("五子棋");
    m_gameTypeCombo->addItem("围棋");

    // 根据传入的游戏类型设置当前选中
    int index = m_gameTypeCombo->findText(gameType);
    if (index >= 0) {
        m_gameTypeCombo->setCurrentIndex(index);
    }

    // 对于围棋，禁用设置（围棋没有自定义设置）
    if (gameType == "围棋") {
        m_gameTypeCombo->setEnabled(false);
    }

    connect(m_gameTypeCombo, &QComboBox::currentTextChanged,
            this, &SettingsDialog::onGameTypeChanged);
    formLayout->addRow("游戏类型:", m_gameTypeCombo);

    // 列数设置
    m_colsSpinBox = new QSpinBox(this);
    m_colsSpinBox->setRange(4, 20);
    m_colsSpinBox->setValue(cols);
    formLayout->addRow("列数:", m_colsSpinBox);

    // 行数设置
    m_rowsSpinBox = new QSpinBox(this);
    m_rowsSpinBox->setRange(4, 20);
    m_rowsSpinBox->setValue(rows);
    formLayout->addRow("行数:", m_rowsSpinBox);

    // 连子数设置
    m_winCountSpinBox = new QSpinBox(this);
    m_winCountSpinBox->setRange(2, 10);
    m_winCountSpinBox->setValue(winCount);
    formLayout->addRow("连子数:", m_winCountSpinBox);

    // 添加说明
    QLabel *infoLabel = new QLabel("提示：修改设置后立刻重新开始游戏", this);
    infoLabel->setStyleSheet("color: gray; font-size: 10px;");
    formLayout->addRow(infoLabel);

    mainLayout->addLayout(formLayout);

    // 重置按钮和确定取消按钮
    QHBoxLayout *buttonLayout = new QHBoxLayout();

    // 重置到默认按钮
    m_resetButton = new QPushButton("重置到默认", this);
    m_resetButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #f0f0f0;"
        "   border: 1px solid #ccc;"
        "   border-radius: 5px;"
        "   padding: 6px 12px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #e0e0e0;"
        "}"
        );
    connect(m_resetButton, &QPushButton::clicked, this, &SettingsDialog::onResetToDefault);
    buttonLayout->addWidget(m_resetButton);

    buttonLayout->addStretch();

    // 确定取消按钮
    QDialogButtonBox *buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
        Qt::Horizontal, this
        );
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    buttonLayout->addWidget(buttonBox);

    mainLayout->addLayout(buttonLayout);

    // 根据当前游戏类型更新默认值显示
    updateDefaultValues(gameType);
}

void SettingsDialog::onGameTypeChanged(const QString &gameType)
{
    m_currentGameType = gameType;

    // 如果是围棋，禁用设置控件
    if (gameType == "围棋") {
        m_colsSpinBox->setEnabled(false);
        m_rowsSpinBox->setEnabled(false);
        m_winCountSpinBox->setEnabled(false);
        m_resetButton->setEnabled(false);
        // 围棋显示固定值
        m_colsSpinBox->setValue(19);
        m_rowsSpinBox->setValue(19);
        m_winCountSpinBox->setValue(0);  // 围棋没有连子数概念
        m_winCountSpinBox->setSuffix(" (不适用)");
    } else {
        m_colsSpinBox->setEnabled(true);
        m_rowsSpinBox->setEnabled(true);
        m_winCountSpinBox->setEnabled(true);
        m_resetButton->setEnabled(true);
        m_winCountSpinBox->setSuffix("");

        // 更新为当前游戏类型的默认值
        updateDefaultValues(gameType);
    }
}

void SettingsDialog::updateDefaultValues(const QString &gameType)
{
    // 根据游戏类型设置默认值
    if (gameType == "重力棋") {
        m_colsSpinBox->setValue(8);
        m_rowsSpinBox->setValue(8);
        m_winCountSpinBox->setValue(4);
    } else if (gameType == "五子棋") {
        m_colsSpinBox->setValue(9);
        m_rowsSpinBox->setValue(9);
        m_winCountSpinBox->setValue(5);
    } else if (gameType == "围棋") {
        m_colsSpinBox->setValue(19);
        m_rowsSpinBox->setValue(19);
        m_winCountSpinBox->setValue(0);
        m_winCountSpinBox->setSuffix(" (不适用)");
    }
}

void SettingsDialog::setDefaultValues(const QString &gameType)
{
    // 设置默认值（与 updateDefaultValues 相同，但用于重置按钮）
    if (gameType == "重力棋") {
        m_colsSpinBox->setValue(8);
        m_rowsSpinBox->setValue(8);
        m_winCountSpinBox->setValue(4);
    } else if (gameType == "五子棋") {
        m_colsSpinBox->setValue(9);
        m_rowsSpinBox->setValue(9);
        m_winCountSpinBox->setValue(5);
    } else if (gameType == "围棋") {
        m_colsSpinBox->setValue(19);
        m_rowsSpinBox->setValue(19);
        m_winCountSpinBox->setValue(0);
        m_winCountSpinBox->setSuffix(" (不适用)");
    }
}

void SettingsDialog::onResetToDefault()
{
    // 重置到当前游戏类型的默认值
    setDefaultValues(m_currentGameType);
}

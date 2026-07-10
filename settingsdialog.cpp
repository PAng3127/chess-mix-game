#include "settingsdialog.h"

SettingsDialog::SettingsDialog(int cols, int rows, int winCount, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("游戏设置");
    setModal(true);
    setMinimumWidth(300);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QFormLayout *formLayout = new QFormLayout();

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
    QLabel *infoLabel = new QLabel("提示：修改设置后需要重新开始游戏", this);
    infoLabel->setStyleSheet("color: gray; font-size: 10px;");
    formLayout->addRow(infoLabel);

    mainLayout->addLayout(formLayout);

    // 按钮
    QDialogButtonBox *buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
        Qt::Horizontal, this
        );
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(buttonBox);
}

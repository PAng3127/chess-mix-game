#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QSpinBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QComboBox>

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    SettingsDialog(const QString &gameType, int cols, int rows, int winCount, QWidget *parent = nullptr);

    int getCols() const { return m_colsSpinBox->value(); }
    int getRows() const { return m_rowsSpinBox->value(); }
    int getWinCount() const { return m_winCountSpinBox->value(); }
    QString getGameType() const { return m_gameTypeCombo->currentText(); }

private slots:
    void onGameTypeChanged(const QString &gameType);
    void onResetToDefault();

private:
    void updateDefaultValues(const QString &gameType);
    void setDefaultValues(const QString &gameType);

    QComboBox *m_gameTypeCombo;
    QSpinBox *m_colsSpinBox;
    QSpinBox *m_rowsSpinBox;
    QSpinBox *m_winCountSpinBox;
    QPushButton *m_resetButton;

    // 存储当前游戏类型
    QString m_currentGameType;
};

#endif // SETTINGSDIALOG_H

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QSpinBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QDialogButtonBox>

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    SettingsDialog(int cols, int rows, int winCount, QWidget *parent = nullptr);

    int getCols() const { return m_colsSpinBox->value(); }
    int getRows() const { return m_rowsSpinBox->value(); }
    int getWinCount() const { return m_winCountSpinBox->value(); }

private:
    QSpinBox *m_colsSpinBox;
    QSpinBox *m_rowsSpinBox;
    QSpinBox *m_winCountSpinBox;
};

#endif // SETTINGSDIALOG_H

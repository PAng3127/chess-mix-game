#ifndef GAMEBUTTON_H
#define GAMEBUTTON_H

#include <QPushButton>
#include <QString>

class GameButton : public QPushButton
{
    Q_OBJECT

public:
    explicit GameButton(const QString &title, const QString &description, 
                        const QString &iconPath = "", QWidget *parent = nullptr);

signals:
    void gameSelected(const QString &gameId);

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    QString m_gameId;
};

#endif // GAMEBUTTON_H
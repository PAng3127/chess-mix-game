#include "gamebutton.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QGraphicsDropShadowEffect>

GameButton::GameButton(const QString &title, const QString &description,
                       const QString &iconPath, QWidget *parent)
    : QPushButton(parent)
{
    setFixedSize(250, 180);
    setStyleSheet(
        "QPushButton {"
        "   background-color: #f0f0f0;"
        "   border: 2px solid #ccc;"
        "   border-radius: 15px;"
        "   padding: 10px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #e0e0e0;"
        "   border-color: #4CAF50;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #d0d0d0;"
        "}"
        );

    // 添加阴影效果，替代 transform
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(10);
    shadow->setOffset(0, 4);
    shadow->setColor(QColor(0, 0, 0, 30));
    setGraphicsEffect(shadow);

    // 设置光标为手型
    setCursor(Qt::PointingHandCursor);

    // 创建布局
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignCenter);

    // 图标（如果有）
    if (!iconPath.isEmpty()) {
        QLabel *iconLabel = new QLabel(this);
        iconLabel->setPixmap(QPixmap(iconPath).scaled(48, 48, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        iconLabel->setAlignment(Qt::AlignCenter);
        layout->addWidget(iconLabel);
    }

    // 标题
    QLabel *titleLabel = new QLabel(title, this);
    titleLabel->setAlignment(Qt::AlignCenter);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    layout->addWidget(titleLabel);

    // 描述
    QLabel *descLabel = new QLabel(description, this);
    descLabel->setAlignment(Qt::AlignCenter);
    descLabel->setWordWrap(true);
    descLabel->setStyleSheet("color: #666; font-size: 11px;");
    layout->addWidget(descLabel);

    // 设置游戏ID
    m_gameId = title;
}

void GameButton::mousePressEvent(QMouseEvent *event)
{
    QPushButton::mousePressEvent(event);
    if (event->button() == Qt::LeftButton) {
        emit gameSelected(m_gameId);
    }
}

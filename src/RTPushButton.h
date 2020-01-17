#pragma once

#include <QPushButton>
#include <QTextDocument>
#include <QUrl>

class RTPushButton:public QPushButton {
	Q_OBJECT
public:
	RTPushButton(QUrl const &url, QWidget *parent = nullptr);
	RTPushButton(QString const &text, QWidget *parent = nullptr);
	QSize sizeHint() const override;
protected:
	void resizeEvent(QResizeEvent *) override;
	void paintEvent(QPaintEvent *) override;
protected:
	QTextDocument		_text;
};

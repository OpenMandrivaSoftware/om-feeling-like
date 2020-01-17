#include "RTPushButton.h"
#include <QFile>
#include <QPainter>
#include <QResizeEvent>

RTPushButton::RTPushButton(QUrl const &url, QWidget *parent):QPushButton(QString(), parent),_text(this) {
	QFile f(url.toLocalFile());
	if(f.open(QFile::ReadOnly)) {
		_text.setBaseUrl(url);
		_text.setHtml(f.readAll());
		f.close();
	} else
		_text.setHtml(url.toString());
}

RTPushButton::RTPushButton(QString const &text, QWidget *parent):QPushButton(QString(), parent),_text(text, this) {
}

QSize RTPushButton::sizeHint() const {
	return QSize(_text.size().width(), _text.size().height());
}

void RTPushButton::resizeEvent(QResizeEvent *e) {
	QPushButton::resizeEvent(e);
	_text.setTextWidth(e->size().width());
}

void RTPushButton::paintEvent(QPaintEvent *e) {
	QPushButton::paintEvent(e);
	QPainter p(this);
	_text.drawContents(&p);
}

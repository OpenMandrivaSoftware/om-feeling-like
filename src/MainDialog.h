#pragma once
#include <QDialog>
#include <QLabel>
#include <QMap>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QGridLayout>
#include "RTPushButton.h"

class MainDialog:public QDialog {
	Q_OBJECT
public:
	MainDialog(QWidget *parent=nullptr);
	QString selectedPreset() const;
protected:
	// FIXME this really doesn't belong to the
	// dialog, just kept here for now for brevity
	bool switchPreset(QString const &preset);
protected slots:
	void presetSelected();
	void buttonClicked(QAbstractButton*);
protected:
	QStringList			_presets;
	QGridLayout			_layout;
	QLabel *			_label;
	QMap<QString,RTPushButton*>	_presetButtons;
	QCheckBox *			_preferDark;
	QDialogButtonBox *		_buttons;
	RTPushButton *			_current;
};

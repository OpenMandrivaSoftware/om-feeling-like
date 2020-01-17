#include "MainDialog.h"
#include "Rpm.h"
#include <QDir>
#include <QMessageBox>
#include <QProcess>
#include <cmath>
#include <iostream>

MainDialog::MainDialog(QWidget *parent):QDialog(parent),_layout(this) {
	QDir presetDir(PREFIX "/share/om-feeling-like/presets");
	QStringList presets=presetDir.entryList(QDir::Dirs|QDir::NoDot|QDir::NoDotDot, QDir::Name);
	// First things first...
	if(presets.contains("OpenMandriva")) {
		presets.removeAll("OpenMandriva");
		presets.prepend("OpenMandriva");
	}
	int width = sqrt(presets.count());

	_label = new QLabel(tr("Old habits are hard to break - fortunately with OpenMandriva you don't have to: You can configure your desktop to look and feel similar to other systems you may be used to."), this);
	_layout.addWidget(_label, 0, 0, 1, width);


	for(int y=0; y<width; y++) {
		for(int x=0; x<width; x++) {
			RTPushButton *b;
			QString const &p = presets.at(y*width+x);
			_presets.insert(p, b = new RTPushButton(QUrl::fromLocalFile(PREFIX "/share/om-feeling-like/presets/" + p + "/description.html"), this));
			if(x==0 && y==0) {
				_current = b;
				b->setDown(true);
			}
			connect(b, &RTPushButton::clicked, this, &MainDialog::presetSelected);
			_layout.addWidget(b, y+1, x);
		}
	}

	_buttons = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Apply|QDialogButtonBox::Cancel, this);
	connect(_buttons, &QDialogButtonBox::clicked, this, &MainDialog::buttonClicked);
	_layout.addWidget(_buttons, width+1, 0, 1, width);
}

void MainDialog::presetSelected() {
	_current->setDown(false);
	_current = static_cast<RTPushButton*>(sender());
	_current->setDown(true);
}

void MainDialog::buttonClicked(QAbstractButton *button) {
	QDialogButtonBox::ButtonRole role = _buttons->buttonRole(button);
	if(role == QDialogButtonBox::RejectRole)
		reject();
	else {
		bool ok = switchPreset(selectedPreset());
		if(ok) {
			// Let's restart plasmashell to reload icons in kicker...
			if(QProcess::execute("/usr/bin/kquitapp5", QStringList() << "plasmashell") == 0)
				QProcess::execute("/usr/bin/kstart5", QStringList() << "plasmashell");
			// But logging out and back in is a good idea anyway
			QMessageBox::information(this, tr("Preset switched"), tr("The preset has been switched to %1. Since not all applications automatically reload all settings, you may have to log out and back in to see all effects.").arg(selectedPreset()));
		} else {
			QMessageBox::warning(this, tr("Something went wrong..."), tr("Something went wrong while trying to switch to the %1 preset.\nPlease try installing dependencies (if any) manually.\nIf that doesn't help, please report a bug or find us on #openmandriva-cooker on irc.freenode.net.").arg(selectedPreset()));
		}
		if(ok && role == QDialogButtonBox::AcceptRole)
			accept();
		_current->setDown(true);
	}
}

QString MainDialog::selectedPreset() const {
	for(QMap<QString,RTPushButton*>::const_iterator it=_presets.begin(); it!=_presets.end(); ++it) {
		if(_current == it.value())
			return it.key();
	}
	return QString();
}

bool MainDialog::switchPreset(QString const &preset) {
	QFile depFile(PREFIX "/share/om-feeling-like/presets/" + preset + "/depends");
	if(depFile.open(QFile::ReadOnly)) {
		QStringList deps;
		while(!depFile.atEnd()) {
			QString dep = depFile.readLine().trimmed();
			if(dep.isEmpty() || dep.startsWith('#'))
				continue;
			if(!Rpm::installed(dep))
				deps.append(dep);
		}
		if(deps.count()) {
			QMessageBox::information(this, tr("Additional packages needed"), tr("This preset depends on additional packages to be installed: %1.\nPlease allow installing them (root privileges needed) in the dialog that follows.").arg(deps.join(", ")));
			if(QProcess::execute("/usr/bin/kdesu", QStringList() << "-t" << "-c" << ("dnf --refresh -y install " + deps.join(" "))) != 0)
				return false;
		}
		depFile.close();
	}
	return QProcess::execute(PREFIX "/bin/om-feeling-like", QStringList() << preset) == 0;
}

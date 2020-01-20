#include "MainDialog.h"
#include "Rpm.h"
#include <QDir>
#include <QMessageBox>
#include <QGuiApplication>
#include <QProcess>
#include <QCursor>
#include <cmath>
#include <iostream>

MainDialog::MainDialog(QWidget *parent):QDialog(parent),_layout(this) {
	QDir presetDir(PREFIX "/share/om-feeling-like/presets");
	for(QString const &p : presetDir.entryList(QDir::Dirs|QDir::NoDot|QDir::NoDotDot, QDir::Name)) {
		if(p.endsWith("_Dark"))
			continue;
		// First things first...
		if(p == "OpenMandriva")
			_presets.prepend(p);
		else
			_presets.append(p);
	}
	int width = sqrtf(_presets.count());
	int height = ceilf(static_cast<float>(_presets.count())/width);

	_label = new QLabel(tr("Old habits are hard to break - fortunately with OpenMandriva you don't have to: You can configure your desktop to look and feel similar to other systems you may be used to."), this);
	_layout.addWidget(_label, 0, 0, 1, width);


	for(int y=0; y<height; y++) {
		for(int x=0; x<width; x++) {
			if(_presets.length() <= y*width+x) // Can happen if the number of presets isn't a square number...
				break;
			RTPushButton *b;
			QString const &p = _presets.at(y*width+x);
			_presetButtons.insert(p, b = new RTPushButton(QUrl::fromLocalFile(PREFIX "/share/om-feeling-like/presets/" + p + "/description.html"), this));
			if(x==0 && y==0) {
				_current = b;
				b->setDown(true);
			}
			connect(b, &RTPushButton::clicked, this, &MainDialog::presetSelected);
			_layout.addWidget(b, y+1, x);
		}
	}

	_preferDark = new QCheckBox(tr("Use the dark version of the preset if available"), this);
	_layout.addWidget(_preferDark, height+1, 0, 1, width);

	_buttons = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Apply|QDialogButtonBox::Cancel, this);
	connect(_buttons, &QDialogButtonBox::clicked, this, &MainDialog::buttonClicked);
	_layout.addWidget(_buttons, height+2, 0, 1, width);
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
			if(QMessageBox::information(this, tr("Preset switched"), tr("The preset has been switched to %1. Since not all applications automatically reload all settings, you may have to log out and back in to see all effects.\nDo you wish to log out now?").arg(selectedPreset()), QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes)
				QProcess::execute("/usr/bin/qdbus", QStringList() << "org.kde.ksmserver" << "/KSMServer" << "org.kde.KSMServerInterface.logout" << "0" << "0" << "0");
		} else {
			QMessageBox::warning(this, tr("Something went wrong..."), tr("Something went wrong while trying to switch to the %1 preset.\nPlease try installing dependencies (if any) manually.\nIf that doesn't help, please report a bug or find us on #openmandriva-cooker on irc.freenode.net.").arg(selectedPreset()));
		}
		if(ok && role == QDialogButtonBox::AcceptRole)
			accept();
		_current->setDown(true);
	}
}

QString MainDialog::selectedPreset() const {
	for(QMap<QString,RTPushButton*>::const_iterator it=_presetButtons.begin(); it!=_presetButtons.end(); ++it) {
		if(_current == it.value()) {
			QString p = it.key();
			if(_preferDark->isChecked()) {
				if(QFile::exists(PREFIX "/share/om-feeling-like/presets/" + p + "_Dark"))
					return p + "_Dark";
			}
			return p;
		}
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
			QMessageBox::information(this, tr("Additional packages needed"), tr("This preset depends on additional packages to be installed: %1.\nDepending on your system configuration, you may now be asked to enter the root (admin) password to allow installing these packages.\nAfter confirming the password, that dialog will seem to hang for a while (while packages are being downloaded and installed). Give it a bit of time.").arg(deps.join(", ")));
			QGuiApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
			bool installed = (QProcess::execute("/usr/bin/pkexec", QStringList() << "dnf" << "--refresh" << "-y" << "install" << deps) == 0);
			QGuiApplication::restoreOverrideCursor();
			if(!installed)
				return false;
		}
		depFile.close();
	}
	QGuiApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	bool ok = (QProcess::execute(PREFIX "/bin/om-feeling-like", QStringList() << preset) == 0);
	QGuiApplication::restoreOverrideCursor();
	return ok;
}

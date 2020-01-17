#include "Rpm.h"
#include <QProcess>

bool Rpm::installed(QString const &name) {
	return QProcess::execute("/bin/rpm", QStringList() << "-q" << name) == 0;
}

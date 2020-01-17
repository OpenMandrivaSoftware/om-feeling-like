#pragma once
#include <QString>

class Rpm {
public:
	static bool installed(QString const &name);
};

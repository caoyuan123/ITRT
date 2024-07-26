#pragma once

#include <QString>

#define U8s(a) (QString::fromLocal8Bit(a))

#define U8c(a) (QString::fromLocal8Bit(a).toUtf8())



#ifndef TKEYS_H
#define TKEYS_H

#include <QObject>
#include <QtTest/QtTest>

#include "mcr/libmacro.h"

class TKeys : public QObject
{
    Q_OBJECT
public:
    explicit TKeys(QObject *parent = nullptr);

private slots:
    void initTestCase();
    void cleanupTestCase();

    void inputAnything();

private:
    mcr::Libmacro *libmacro_pt;
};

#endif // TKEYS_H

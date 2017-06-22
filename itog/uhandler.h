#ifndef UHANDLER_H
#define UHANDLER_H
#include "QObject"

class UHandler : public QObject
{
    Q_OBJECT

public:
    UHandler() {}
    ~UHandler() {}

public slots:
    void callFunc(const QString in);

signals:
    void needSend(const QString &s);

};

class CDrawUI : public QObject
{
    Q_OBJECT

public:
    CDrawUI();

public slots:
    void draw(QString in);

};
class TryDrawUI : public QObject
{
    Q_OBJECT
public:
    TryDrawUI() {}
    void try_draw(QString in);

signals:
    void needDraw(QString in);

public slots:
    void send(QString in);
};

#endif // UHANDLER_H

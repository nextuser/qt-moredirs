#ifndef THISTORY_H
#define THISTORY_H
#include <QStringList>
class THistory
{
    const int MAX_COUNT = 100;
public:
    THistory();
    bool nextable();
    bool prevable();
    QString prev();
    QString next();
    QString cur();
    void addItem(QString url);
    void clear();
    bool isIndexValid(int index);
private:
    QStringList historys;
    int cursor;

};

#endif // THISTORY_H

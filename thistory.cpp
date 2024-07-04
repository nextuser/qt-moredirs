#include "thistory.h"

THistory::THistory():cursor(0) {

}


bool THistory::nextable()
{
    return isIndexValid(cursor + 1);
}

bool THistory::prevable()
{
    return isIndexValid(cursor -1);
}

QString THistory::prev()
{
    if(prevable() ){
        -- cursor;
    }
    return cur();
}

QString THistory::next()
{
    if(nextable()) {
        ++ cursor;
    }
    return cur();
}

QString THistory::cur()
{
    if(isIndexValid(cursor)){
        return historys.at(cursor);
    }
    return QString();
}

void THistory::addItem(QString url){
    if(historys.size() > MAX_COUNT)  historys.pop_front();

    if(isIndexValid(cursor)){
        historys = historys.sliced(0,cursor + 1);
    }

    historys.append(url);
    cursor = (int)historys.size() - 1;
}
void THistory::clear(){
    historys.clear();
}

bool THistory::isIndexValid(int index)
{
    return  index >= 0 && index < historys.size();
}

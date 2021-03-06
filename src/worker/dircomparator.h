#ifndef DIRCOMPARATOR_H
#define DIRCOMPARATOR_H
#include <QObject>
#include <QLinkedList>
#include <QVector>
#include <QHash>
#include <QMutableListIterator>
#include <QLinkedListIterator>
#include <QListIterator>
#include <QList>
#include <QMutableVectorIterator>
#include <QVectorIterator>
#include <QFileInfo>
#include <QCryptographicHash>
#include "../hashfileinfostruct.h"
#include "hashdirwalker.h"
#ifdef MYPREFIX_DEBUG
#include <QDebug>
#endif
#include <QThread>
#include "../constants.h"

#define SizeForInnerVector 0

class DirComparator : public HashDirWalker
{
     Q_OBJECT
public:
    DirComparator(QCryptographicHash::Algorithm hash, QObject *parent = nullptr);
    virtual ~DirComparator();
    void processFile(const QString &fileName) Q_DECL_OVERRIDE;

protected:

private:
    QMultiHash<QString, HashFileInfoStruct> hashByHash;
    int resultCount;
    void clearNoDuplicatedSize();
    void makeHashByHashes();
    void clearNoDuplicatedHashes();
    void reduceToResult();
    QSharedPtrListHFIS result;

signals:
    void finishedWData(QSharedPtrListHFIS itemsPtr);

public slots:
    void process();
};

#endif // DIRCOMPARATOR_H

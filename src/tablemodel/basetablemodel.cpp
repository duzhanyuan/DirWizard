#include "basetablemodel.h"

Qt::ItemFlags BaseTableModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags theFlags = QAbstractTableModel::flags(index);
    // |Qt::ItemIsEditable
    if (index.isValid())
        theFlags |= Qt::ItemIsSelectable|
                    Qt::ItemIsEnabled|Qt::ItemIsUserCheckable;
    return theFlags;
}

bool BaseTableModel::removeRow(int row, const QModelIndex&)
{
    beginRemoveRows(QModelIndex(), row, row);
    items->removeAt(row);
    endRemoveRows();
    return true;
}

bool BaseTableModel::removeRows(int row, int count, const QModelIndex&)
{
    beginRemoveRows(QModelIndex(), row, row + count - 1);
    if(row==0 && count==items->count()) {
        items->clear();
    } else {
    for (int i = 0; i < count; ++i)
        items->removeAt(row);
    }
    endRemoveRows();
    return true;
}


void BaseTableModel::saveToFileFunc(const QString &fileName) const
{
    if(items.isNull())
        return;
    QFile file(fileName);
    if(file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&file);
        out.setCodec("UTF-8");
        out.setGenerateByteOrderMark(false);
        QString delimeter = "\t";
        QString endOfLine = "\n";
        out<<items.data()->count() << endOfLine;

        out << tr("Group ID") << delimeter;
        out << tr("Size") << delimeter;
        out << tr("Hash") << delimeter << tr("FileName");
        out << endOfLine;

        QListIterator<HashFileInfoStruct> it(*items);
        while(it.hasNext())
        {
            const HashFileInfoStruct &s = it.next();
            out << QString("%1").arg(s.groupID) << delimeter;
            out << s.size << delimeter << s.hash << delimeter << s.fileName << endOfLine;
        }
        file.close();
    }
}

void BaseTableModel::loadFromFileFunc(const QString &fileName)
{
    const QString delimeter = "\t";
    QFile file(fileName);
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        if(file.size() == 0) {
            file.close();
            return;
        }
        QTextStream in(&file);
        in.setCodec("UTF-8");
        if(items.isNull()) {
            items = QSharedPtrListHFIS(new QList<HashFileInfoStruct>());
        } else {
            const int count = items->count();
            if(count != 0) {
                removeRows(0, count, QModelIndex());
            }
        }
        const int newItemsCount = in.readLine().toInt();
        if(newItemsCount == 0) {
            file.close();
            return;
        }
        in.readLine(); //Pass header string
        items.data()->reserve(newItemsCount);
        QList<HashFileInfoStruct> * const list = items.data();
        QStringList lineList;
        while(!in.atEnd()) {
            lineList = in.readLine().split(delimeter);
            if(lineList.size() < 4) {
                file.close();
                return;
            }
            HashFileInfoStruct strct;
            strct.groupID = lineList[0].toUInt();
            strct.size = lineList[1].toLong();
            strct.hash = lineList[2];
            strct.fileName = lineList[3];
            strct.checked = false;
            list->append(strct);
        }
        file.close();
        beginInsertRows(QModelIndex(),0,newItemsCount);
        insertRows(0, newItemsCount);
        endInsertRows();
    }
}

void BaseTableModel::removeCheckedFunc()
{
    QListIterator<HashFileInfoStruct> it(*items);
    int removed = 0;
    int row = 0;
    QModelIndex index;
    while(it.hasNext())
    {
        const HashFileInfoStruct &strct = it.next();
        if(strct.checked)
        {
            if(QDir(strct.fileName).remove(strct.fileName)) {
                ++removed;
                removeRow(row, index);
            } else {
                QMessageBox msgBox;
                msgBox.setText(QObject::tr("Can't delete file: %1").arg(strct.fileName));
                msgBox.setIcon(QMessageBox::Warning);
                msgBox.exec();
            }
        }
        ++row;
    }
}

//Slots
void BaseTableModel::saveToFile(QString fileName)
{
#ifdef MYPREFIX_DEBUG
qDebug() << "BaseTableModel::saveToFile";
#endif
    saveToFileFunc(fileName);
}

void BaseTableModel::removeChecked(bool checked)
{
    Q_UNUSED(checked)
    removeChecked();
}

QString BaseTableModel::getFileName(const QModelIndex &index) const {
    Q_UNUSED(index)
    return "";
}

void BaseTableModel::unselectAll()
{
    if(items.isNull())
        return;

    QList<HashFileInfoStruct> * const list = items.data();
    const int count = list->count();
    QMutableListIterator<HashFileInfoStruct> it(*list);
    while(it.hasNext()){
        HashFileInfoStruct &strct = it.next();
        if(strct.checked){
            strct.checked = false;
        }
    }
    emit dataChanged(createIndex(0,0), createIndex(count,0));
}

void BaseTableModel::selectFilesInFolder(const QString &dirName)
{
#ifdef MYPREFIX_DEBUG
qDebug() << "BaseTableModel::selectFilesInFolder: "<<dirName;
#endif
    if(items.isNull()) {
        return;
    }
    QList<HashFileInfoStruct> * const list = items.data();
    const int count = list->count();
    QMutableListIterator<HashFileInfoStruct> it(*list);
    while(it.hasNext()){
        HashFileInfoStruct &strct = it.next();
        if(strct.fileName.startsWith(dirName)){
            strct.checked = true;
            qDebug()<<strct.fileName;
        }
    }
    emit dataChanged(createIndex(0,0), createIndex(count,0));
}


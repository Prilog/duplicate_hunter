#ifndef SCANNER_H
#define SCANNER_H

#include <QCommonStyle>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QCoreApplication>
#include <QDirIterator>
#include <QMap>
#include <QVector>
#include <QString>
#include <QThread>
#include <QTextStream>
#include <QCryptographicHash>

class scanner: public QObject {
    Q_OBJECT

public:
    scanner(QString dir);
    ~scanner();

    void do_scanning();

private:
    void collect_files();
    void add_file(QFile& file);
    bool check_interruption();
    void change_progress();
    void finish();
    QByteArray count_hash(QFile& file);
    QMap <qint64, QVector <QString> > files;
    QString root_directory;
    qint64 total_size, cur_size, cur_percent;

public slots:

signals:
    void add_file_list(QVector <QString> name);
    void set_progress_bar(qint64 value);
    void finished();
};

#endif // SCANNER_H

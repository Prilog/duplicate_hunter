#include "scanner.h"

scanner::scanner(QString dir) {
    root_directory = dir;
    total_size = 0;
    cur_size = 0;
    cur_percent = 0;
}

scanner::~scanner() {

}

bool scanner::check_interruption() {
    return (QThread::currentThread()->isInterruptionRequested());
}

void scanner::do_scanning() {
    collect_files();
    for (auto file_list : files) {
        QMap <QByteArray, QVector <QString> > m;
        for (auto file_name : file_list) {
            if (check_interruption()) {
                break;
            }
            QFile file(file_name);
            cur_size += file.size();
            change_progress();
            QByteArray hash = count_hash(file);
            if (!hash.isEmpty()) {
                m[hash].push_back(file.fileName());
            }
        }
        for (auto file_list : m.toStdMap()) {
            if (file_list.second.size() > 1) {
                emit add_file_list(file_list.second);
            }
        }
    }
    emit finished();
}

QByteArray scanner::count_hash(QFile& file) {
    if (!file.open(QIODevice::ReadOnly)) {
        return QByteArray();
    }
    QCryptographicHash::Algorithm hashAlgorithm = QCryptographicHash::Md5;
    QCryptographicHash hash(hashAlgorithm);
    if (hash.addData(&file)) {
        return hash.result();
    }
    return QByteArray();
}

void scanner::collect_files() {
    QDirIterator scanner(root_directory, QDirIterator::Subdirectories);
    while (scanner.hasNext()) {
        if (check_interruption()) {
            break;
        }
        QFile f(scanner.next());
        if (!scanner.fileInfo().isDir()) {
            total_size += f.size();
            add_file(f);
        }
    }
}

void scanner::change_progress() {
     qint64 new_percent = qint64(double(100) * (double(cur_size) / double(total_size)));
     if (new_percent != cur_percent) {
         cur_percent = new_percent;
         emit set_progress_bar(cur_percent);
     }
}

void scanner::add_file(QFile& file) {
    files[file.size()].push_back(file.fileName());
}

void scanner::stop() {

}

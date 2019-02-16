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
                QVector <bool> vis(file_list.second.size());
                for (int i = 0; i < file_list.second.size(); i++) {
                    if (!vis[i]) {
                        QFile f(file_list.second[i]);
                        if (f.open(QIODevice::ReadOnly)) {
                            QVector <QString> cur;
                            cur.push_back(file_list.second[i]);
                            for (int j = i + 1; j < file_list.second.size(); j++) {
                                QFile cur_f(file_list.second[j]);
                                if (cur_f.open(QIODevice::ReadOnly) && f.size() == cur_f.size()) {
                                    char buffer[1024];
                                    char buffer2[1024];
                                    bool equal = true;
                                    while (!f.atEnd()) {
                                        qint64 buffer_size = f.read(buffer, sizeof(buffer));
                                        cur_f.read(buffer2, sizeof(buffer2));
                                        for (int i = 0; i < buffer_size; i++) {
                                            if (buffer[i] != buffer2[i]) {
                                                equal = false;
                                                break;
                                            }
                                        }
                                        if (!equal) {
                                            break;
                                        }
                                    }
                                    if (equal) {
                                        cur.push_back(file_list.second[j]);
                                        vis[j] = true;
                                    }
                                }
                            }
                            if (cur.size() > 0) {
                                emit add_file_list(cur);
                            }
                        }
                    }
                }
            }
        }
    }
    finish();
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
        if (!scanner.fileInfo().isDir() && f.open(QIODevice::ReadOnly)) {
            total_size += f.size();
            add_file(f);
        }
    }
}

void scanner::change_progress() {
    if (total_size == 0) {
        emit set_progress_bar(100);
        return;
    }
     qint64 new_percent = qint64(double(100) * (double(cur_size) / double(total_size)));
     if (new_percent != cur_percent) {
         cur_percent = new_percent;
         emit set_progress_bar(cur_percent);
     }
}

void scanner::add_file(QFile& file) {
    files[file.size()].push_back(file.fileName());
}

void scanner::finish() {
    change_progress();
    emit finished();
}

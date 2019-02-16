#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QCommonStyle>
#include <QDesktopWidget>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QCoreApplication>
#include <QDirIterator>
#include <QStandardItemModel>
#include <QThread>
#include <QVector>
#include <QTreeWidget>
#include <QFileSystemModel>

#include "scanner.h"

namespace Ui {
class main_window;
}

class main_window : public QMainWindow
{
    Q_OBJECT

public:
    explicit main_window(QWidget *parent = nullptr);
    ~main_window();

private slots:
    void select_directory();
    void full_cleaning();
    void run_scanning();
    void interrupt_scanning();

public slots:
    void set_progress_bar(qint64 value);
    void add_file_list(QVector <QString> file_list);
    void scanning_finished();

private:
    Ui::main_window *ui;
    const QString NO_DIR_MSG = QString("No dirrectory chosen");
    const QString HEADER_DIR_MSG = QString("Chosen dirrectory: ");
    const QString NOTHING_DOING_MSG = QString("Nothing");
    const QString STARTED_SCANNING_MSG = QString("Scanning");
    const QString FINISHED_SCANNING_MSG = QString("Finished");
    QStandardItemModel *model;
    QString start_dir;
    bool is_scanning;
    QThread *thread;

    void scan_directory(QString const& dir);
    void clear_table();
};

#endif // MAIN_WINDOW_H

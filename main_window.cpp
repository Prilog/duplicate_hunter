#include "main_window.h"
#include "ui_main_window.h"

main_window::main_window(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::main_window)
{
    ui->setupUi(this);
    QWidget::setWindowTitle(QString("Duplicate Hunter 2"));
    //Thread Settings
    thread = nullptr;

    //Flags Setting
    is_scanning = false;

    //Text Settings
    ui->label->setText(NO_DIR_MSG);
    ui->process_label->setText(NOTHING_DOING_MSG);

    //Button Settings
    ui->progress_bar->setValue(0);
    connect(ui->select_button, SIGNAL (released()), this, SLOT (select_directory()));
    connect(ui->clear_button, SIGNAL (released()), this, SLOT (full_cleaning()));
    connect(ui->run_button, SIGNAL (released()), this, SLOT (run_scanning()));
    connect(ui->interrupt_button, SIGNAL (released()), this, SLOT (interrupt_scanning()));

    //Tree Settings
    QTreeWidgetItem* item = new QTreeWidgetItem();
    item->setText(0, QString("Result:"));
    ui->tree_widget->setHeaderItem(item);

    //Progress bar Settings
    ui->progress_bar->setMinimum(0);
    ui->progress_bar->setMaximum(100);
    ui->progress_bar->setValue(0);
}

void main_window::select_directory()
{
    if (thread == nullptr) {
        full_cleaning();
        QString dir = QFileDialog::getExistingDirectory(this, "Select Directory for THE BIG HUNT", QString(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        if (dir.size() != 0) {
            start_dir = dir;
            ui->label->setText(HEADER_DIR_MSG + start_dir);
        }
    }
}

void main_window::clear_table()
{
    ui->tree_widget->clear();
}

void main_window::full_cleaning() {
    if (thread == nullptr) {
        start_dir.clear();
        ui->label->setText(NO_DIR_MSG);
        ui->progress_bar->setValue(0);
        ui->process_label->setText(NOTHING_DOING_MSG);
        clear_table();
    }
}

void main_window::interrupt_scanning() {
    if (thread !=  nullptr) {
        thread->requestInterruption();
    }
}

void main_window::run_scanning()
{
    if (thread == nullptr) {
        if (start_dir.size() != 0) {
            clear_table();
            scan_directory(start_dir);
        }
    }
}

void main_window::set_progress_bar(qint64 value) {
    ui->progress_bar->setValue(int(value));
}

void main_window::add_file_list(QVector <QString> file_list) {
    QTreeWidgetItem* item = new QTreeWidgetItem();
    item->setText(0, file_list.first() + " is equal to " + QString::number(file_list.size()) + " files");
    for (auto file : file_list) {
        QTreeWidgetItem* child_item = new QTreeWidgetItem();
        child_item->setText(0, file);
        item->addChild(child_item);
    }
    ui->tree_widget->addTopLevelItem(item);
}

void main_window::scanning_finished() {
    ui->process_label->setText(FINISHED_SCANNING_MSG);
    thread = nullptr;
}

void main_window::scan_directory(QString const& dir)
{
    thread = new QThread();
    scanner *cur_scanner = new scanner(dir);
    qRegisterMetaType<QVector<QList<QString>>>("QVector <QString>");
    //
    connect(thread, &QThread::started, cur_scanner, &scanner::do_scanning);
    ui->process_label->setText(STARTED_SCANNING_MSG);

    connect(thread, &QThread::finished, cur_scanner, &scanner::deleteLater);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);

    connect(cur_scanner, &scanner::finished, thread, &QThread::quit);
    //connect(cur_scanner, &scanner::finished, thread, &QThread::deleteLater);
    connect(cur_scanner, &scanner::finished, this, &main_window::scanning_finished);

    connect(cur_scanner, &scanner::add_file_list, this, &main_window::add_file_list);
    connect(cur_scanner, &scanner::set_progress_bar, this, &main_window::set_progress_bar);
    //
    cur_scanner->moveToThread(thread);
    thread->start();
}

main_window::~main_window()
{
    delete ui;
}

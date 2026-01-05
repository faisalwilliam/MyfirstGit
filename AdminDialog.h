#ifndef ADMINDIALOG_H
#define ADMINDIALOG_H

#include <QDialog>
#include <QTableView>
#include <QSqlTableModel>
#include <QLineEdit>

class AdminDialog : public QDialog {
    Q_OBJECT
public:
    explicit AdminDialog(QWidget *parent = nullptr);

private:
    QTableView *tableView;
    QSqlTableModel *model;
    QLineEdit *searchEdit;
    void exportToCSV();
    void deleteUser();
    void importFromCSV();
    void unlockUser();
    void resetDatabase();
    void logAction(const QString &message);
    void backupDatabase();
    void compactDatabase();
    void dumpToConsole();
    void viewAsText();
    void viewAllReports();
};

#endif // ADMINDIALOG_H
#ifndef MYREPORTSDIALOG_H
#define MYREPORTSDIALOG_H

#include <QDialog>
#include <QTableView>
#include <QSqlTableModel>

class MyReportsDialog : public QDialog {
    Q_OBJECT
public:
    explicit MyReportsDialog(const QString &employeeId, QWidget *parent = nullptr);

private:
    QTableView *tableView;
    QSqlTableModel *model;
    QString currentEmployeeId;
    void deleteReport();
    void selectAllReports();
};

#endif // MYREPORTSDIALOG_H
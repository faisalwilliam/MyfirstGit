#ifndef ADMINREPORTSDIALOG_H
#define ADMINREPORTSDIALOG_H

#include <QDialog>
#include <QTableView>
#include <QSqlTableModel>

class AdminReportsDialog : public QDialog {
    Q_OBJECT
public:
    explicit AdminReportsDialog(QWidget *parent = nullptr);

private:
    QTableView *tableView;
    QSqlTableModel *model;
};

#endif // ADMINREPORTSDIALOG_H
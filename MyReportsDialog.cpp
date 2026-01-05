#include "MyReportsDialog.h"
#include <QVBoxLayout>
#include <QHeaderView>
#include <QSqlTableModel>
#include <QPushButton>
#include <QMessageBox>
#include <QInputDialog>
#include <QSqlQuery>
#include <QCryptographicHash>

MyReportsDialog::MyReportsDialog(const QString &employeeId, QWidget *parent) : QDialog(parent), currentEmployeeId(employeeId) {
    setWindowTitle("My Saved Reports");
    resize(800, 400);

    QVBoxLayout *layout = new QVBoxLayout(this);

    tableView = new QTableView(this);
    model = new QSqlTableModel(this);
    model->setTable("reports");
    model->setFilter(QString("employee_id = '%1'").arg(employeeId));
    model->setSort(model->fieldIndex("report_date"), Qt::DescendingOrder); // Sort by date
    model->select();
    
    model->setHeaderData(model->fieldIndex("report_date"), Qt::Horizontal, "Report Date");
    model->setHeaderData(model->fieldIndex("total_hours"), Qt::Horizontal, "Total Hours");
    model->setHeaderData(model->fieldIndex("gross_wage"), Qt::Horizontal, "Gross Wage");
    model->setHeaderData(model->fieldIndex("tax"), Qt::Horizontal, "Tax");
    model->setHeaderData(model->fieldIndex("net_wage"), Qt::Horizontal, "Net Wage");

    tableView->setModel(model);
    
    tableView->hideColumn(model->fieldIndex("id"));
    tableView->hideColumn(model->fieldIndex("employee_id"));
    tableView->hideColumn(model->fieldIndex("name"));

    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    
    layout->addWidget(tableView);

    QPushButton *selectAllBtn = new QPushButton("Select All", this);
    layout->addWidget(selectAllBtn);
    connect(selectAllBtn, &QPushButton::clicked, this, &MyReportsDialog::selectAllReports);

    QPushButton *deleteBtn = new QPushButton("Delete Report", this);
    layout->addWidget(deleteBtn);
    connect(deleteBtn, &QPushButton::clicked, this, &MyReportsDialog::deleteReport);
}

void MyReportsDialog::deleteReport() {
    QModelIndexList selected = tableView->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        QMessageBox::warning(this, "Selection Error", "Please select a report to delete.");
        return;
    }

    bool ok;
    QString password = QInputDialog::getText(this, "Confirm Delete", "Enter password to confirm deletion:", QLineEdit::Password, "", &ok);
    if (ok && !password.isEmpty()) {
        QSqlQuery query;
        query.prepare("SELECT password FROM users WHERE employee_id = :id");
        query.bindValue(":id", currentEmployeeId);
        if (query.exec() && query.next()) {
            QString storedHash = query.value(0).toString();
            QString inputHash = QString(QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex());
            if (storedHash == inputHash) {
                for (const QModelIndex &index : selected) {
                    model->removeRow(index.row());
                }
                model->select();
                QMessageBox::information(this, "Success", "Report deleted.");
            } else {
                QMessageBox::warning(this, "Error", "Incorrect password.");
            }
        }
    }
}

void MyReportsDialog::selectAllReports() {
    tableView->selectAll();
}
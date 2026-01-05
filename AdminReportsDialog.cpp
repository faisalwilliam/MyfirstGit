#include "AdminReportsDialog.h"
#include <QVBoxLayout>
#include <QHeaderView>
#include <QSqlTableModel>

AdminReportsDialog::AdminReportsDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("All Saved Reports (Admin View)");
    resize(900, 500);

    QVBoxLayout *layout = new QVBoxLayout(this);

    tableView = new QTableView(this);
    model = new QSqlTableModel(this);
    model->setTable("reports");
    model->setSort(model->fieldIndex("report_date"), Qt::DescendingOrder); // Sort by date
    model->select();
    
    model->setHeaderData(model->fieldIndex("employee_id"), Qt::Horizontal, "Employee ID");
    model->setHeaderData(model->fieldIndex("name"), Qt::Horizontal, "Name");
    model->setHeaderData(model->fieldIndex("report_date"), Qt::Horizontal, "Report Date");
    model->setHeaderData(model->fieldIndex("total_hours"), Qt::Horizontal, "Total Hours");
    model->setHeaderData(model->fieldIndex("gross_wage"), Qt::Horizontal, "Gross Wage");
    model->setHeaderData(model->fieldIndex("tax"), Qt::Horizontal, "Tax");
    model->setHeaderData(model->fieldIndex("net_wage"), Qt::Horizontal, "Net Wage");

    tableView->setModel(model);
    tableView->hideColumn(model->fieldIndex("id")); // Hide internal ID
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    
    layout->addWidget(tableView);
}
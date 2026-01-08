#include "MyReportsDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QHeaderView>
#include <QSqlTableModel>
#include <QPushButton>
#include <QMessageBox>
#include <QInputDialog>
#include <QSqlQuery>
#include <QCryptographicHash>
#include <QDateEdit>
#include <QCheckBox>
#include <QDate>
#include <QSet>
#include <algorithm>
#include <QPainter>
#include <QMouseEvent>
#include <QStyleOptionButton>
#include <QStyle>
#include <QFileDialog>
#include <QTextStream>

CheckBoxHeader::CheckBoxHeader(Qt::Orientation orientation, QWidget* parent)
    : QHeaderView(orientation, parent), checked(false) {}

bool CheckBoxHeader::isChecked() const { return checked; }

void CheckBoxHeader::setChecked(bool c) {
    if (checked != c) {
        checked = c;
        update(); // Trigger repaint
        emit checkBoxToggled(checked);
    }
}

void CheckBoxHeader::paintSection(QPainter* painter, const QRect& rect, int logicalIndex) const {
    painter->save();
    QHeaderView::paintSection(painter, rect, logicalIndex);
    painter->restore();

    if (visualIndex(logicalIndex) == 0) { // Draw checkbox in the first visible column
        QStyleOptionButton option;
        // Position the checkbox on the left side of the header section
        option.rect = QRect(rect.left() + 4, rect.top() + (rect.height() - 16) / 2, 16, 16);
        option.state = QStyle::State_Enabled | (checked ? QStyle::State_On : QStyle::State_Off);
        style()->drawPrimitive(QStyle::PE_IndicatorCheckBox, &option, painter);
    }
}

void CheckBoxHeader::mousePressEvent(QMouseEvent* event) {
    int logicalIndex = logicalIndexAt(event->pos());
    if (visualIndex(logicalIndex) == 0) {
        int sectionX = sectionViewportPosition(logicalIndex);
        if (event->pos().x() >= sectionX + 4 && event->pos().x() <= sectionX + 20) {
            setChecked(!checked);
            return;
        }
    }
    QHeaderView::mousePressEvent(event);
}

MyReportsDialog::MyReportsDialog(const QString &employeeId, QWidget *parent) : QDialog(parent), currentEmployeeId(employeeId) {
    setWindowTitle("My Saved Reports");
    resize(800, 400);

    QVBoxLayout *layout = new QVBoxLayout(this);

    QHBoxLayout *searchLayout = new QHBoxLayout();
    layout->addLayout(searchLayout);

    searchLayout->addWidget(new QLabel("Search Name:", this));
    searchEdit = new QLineEdit(this);
    searchEdit->setPlaceholderText("Filter by Name...");
    searchLayout->addWidget(searchEdit);

    QHBoxLayout *dateLayout = new QHBoxLayout();
    layout->addLayout(dateLayout);

    dateFilterCheck = new QCheckBox("Filter by Date:", this);
    dateLayout->addWidget(dateFilterCheck);

    startDateEdit = new QDateEdit(QDate::currentDate().addDays(-30), this);
    startDateEdit->setCalendarPopup(true);
    startDateEdit->setDisplayFormat("yyyy-MM-dd");
    startDateEdit->setEnabled(false);
    dateLayout->addWidget(startDateEdit);

    dateLayout->addWidget(new QLabel("to", this));

    endDateEdit = new QDateEdit(QDate::currentDate(), this);
    endDateEdit->setCalendarPopup(true);
    endDateEdit->setDisplayFormat("yyyy-MM-dd");
    endDateEdit->setEnabled(false);
    dateLayout->addWidget(endDateEdit);
    dateLayout->addStretch();

    tableView = new QTableView(this);
    model = new QSqlTableModel(this);
    model->setTable("reports");
    model->setFilter(QString("employee_id = '%1'").arg(employeeId));
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
    
    tableView->hideColumn(model->fieldIndex("id"));

    // Set custom header with checkbox
    CheckBoxHeader* header = new CheckBoxHeader(Qt::Horizontal, tableView);
    tableView->setHorizontalHeader(header);
    connect(header, &CheckBoxHeader::checkBoxToggled, this, [this](bool checked){
        if (checked) selectAllReports();
        else clearSelection();
    });

    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setSelectionMode(QAbstractItemView::MultiSelection); // Allows "marking" multiple rows by clicking

    
    layout->addWidget(tableView);

    summaryLabel = new QLabel("Total Net Earnings: 0.00", this);
    layout->addWidget(summaryLabel);

    QPushButton *clearSelectBtn = new QPushButton("Clear Selection", this);
    layout->addWidget(clearSelectBtn);
    connect(clearSelectBtn, &QPushButton::clicked, this, &MyReportsDialog::clearSelection);

    QPushButton *exportBtn = new QPushButton("Export Selected to CSV", this);
    layout->addWidget(exportBtn);
    connect(exportBtn, &QPushButton::clicked, this, &MyReportsDialog::exportSelectedToCSV);

    QPushButton *deleteBtn = new QPushButton("Delete Report", this);
    layout->addWidget(deleteBtn);
    connect(deleteBtn, &QPushButton::clicked, this, &MyReportsDialog::deleteReport);

    connect(dateFilterCheck, &QCheckBox::toggled, [this](bool checked){
        startDateEdit->setEnabled(checked);
        endDateEdit->setEnabled(checked);
        applyFilter();
    });
    connect(startDateEdit, &QDateEdit::dateChanged, [this](){ applyFilter(); });
    connect(endDateEdit, &QDateEdit::dateChanged, [this](){ applyFilter(); });
    connect(searchEdit, &QLineEdit::textChanged, [this](){ applyFilter(); });

    updateSummary();
}

void MyReportsDialog::deleteReport() {
    QModelIndexList selected = tableView->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        QMessageBox::warning(this, "Selection Error", "Please select (mark) reports to delete.");
        return;
    }

    int count = selected.count();
    auto reply = QMessageBox::question(this, "Confirm Deletion", 
                                       QString("Are you sure you want to delete %1 selected report(s)?").arg(count),
                                       QMessageBox::Yes | QMessageBox::No);
    if (reply != QMessageBox::Yes) return;

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
                // Collect rows to delete
                QList<int> rowsToDelete;
                for (const QModelIndex &index : selected) {
                    rowsToDelete.append(index.row());
                }

                // Sort in descending order to delete from bottom up (avoids index shifting)
                std::sort(rowsToDelete.begin(), rowsToDelete.end(), std::greater<int>());

                for (int row : rowsToDelete) {
                    model->removeRow(row);
                }
                model->select();
                updateSummary();
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

void MyReportsDialog::clearSelection() {
    tableView->clearSelection();
}

void MyReportsDialog::exportSelectedToCSV() {
    QModelIndexList selected = tableView->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        QMessageBox::warning(this, "Selection Error", "Please select reports to export.");
        return;
    }

#ifdef Q_OS_WASM
    QString content;
    QTextStream out(&content);

    // Headers
    for (int i = 0; i < model->columnCount(); ++i) {
        if (!tableView->isColumnHidden(i)) {
            out << model->headerData(i, Qt::Horizontal).toString();
            if (i < model->columnCount() - 1) out << ",";
        }
    }
    out << "\n";

    // Data
    for (const QModelIndex &index : selected) {
        for (int j = 0; j < model->columnCount(); ++j) {
            if (!tableView->isColumnHidden(j)) {
                out << model->data(model->index(index.row(), j)).toString();
                if (j < model->columnCount() - 1) out << ",";
            }
        }
        out << "\n";
    }
    QFileDialog::saveFileContent(content.toUtf8(), "my_reports.csv");
    return;
#endif

    QString fileName = QFileDialog::getSaveFileName(this, "Export CSV", "", "CSV Files (*.csv)");
    if (fileName.isEmpty()) return;

    if (!fileName.endsWith(".csv", Qt::CaseInsensitive)) fileName += ".csv";

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);

        // Headers
        for (int i = 0; i < model->columnCount(); ++i) {
            if (!tableView->isColumnHidden(i)) {
                out << model->headerData(i, Qt::Horizontal).toString();
                if (i < model->columnCount() - 1) out << ",";
            }
        }
        out << "\n";

        // Data
        for (const QModelIndex &index : selected) {
            for (int j = 0; j < model->columnCount(); ++j) {
                if (!tableView->isColumnHidden(j)) {
                    out << model->data(model->index(index.row(), j)).toString();
                    if (j < model->columnCount() - 1) out << ",";
                }
            }
            out << "\n";
        }
        file.close();
        QMessageBox::information(this, "Success", "Selected reports exported successfully.");
    } else {
        QMessageBox::critical(this, "Error", "Could not save file.");
    }
}

void MyReportsDialog::applyFilter() {
    QString filter = QString("employee_id = '%1'").arg(currentEmployeeId);
    
    QString nameText = searchEdit->text();
    if (!nameText.isEmpty()) {
        filter += QString(" AND name LIKE '%%1%'").arg(nameText);
    }

    if (dateFilterCheck->isChecked()) {
        QString start = startDateEdit->date().toString("yyyy-MM-dd");
        QString end = endDateEdit->date().toString("yyyy-MM-dd");
        filter += QString(" AND report_date >= '%1' AND report_date <= '%2 23:59:59'").arg(start, end);
    }

    model->setFilter(filter);
    if(model->select()) {
         updateSummary();
    }
}

void MyReportsDialog::updateSummary() {
    while (model->canFetchMore())
        model->fetchMore();

    double totalGrossWage = 0.0;
    double totalNetWage = 0.0;
    int grossWageCol = model->fieldIndex("gross_wage");
    int netWageCol = model->fieldIndex("net_wage");
    
    // Iterate through all rows currently in the model (which respects the filter)
    for (int i = 0; i < model->rowCount(); ++i) {
        totalGrossWage += model->data(model->index(i, grossWageCol)).toDouble();
        totalNetWage += model->data(model->index(i, netWageCol)).toDouble();
    }
    
    summaryLabel->setText(QString("Total Gross: %1  |  Total Net: %2")
                          .arg(totalGrossWage, 0, 'f', 2)
                          .arg(totalNetWage, 0, 'f', 2));
}
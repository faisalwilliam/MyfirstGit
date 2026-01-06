#ifndef MYREPORTSDIALOG_H
#define MYREPORTSDIALOG_H

#include <QDialog>
#include <QTableView>
#include <QSqlTableModel>
#include <QLineEdit>
#include <QLabel>
#include <QDateEdit>
#include <QCheckBox>
#include <QHeaderView>

class CheckBoxHeader : public QHeaderView {
    Q_OBJECT
public:
    CheckBoxHeader(Qt::Orientation orientation, QWidget* parent = nullptr);
    bool isChecked() const;
    void setChecked(bool checked);
signals:
    void checkBoxToggled(bool checked);
protected:
    void paintSection(QPainter* painter, const QRect& rect, int logicalIndex) const override;
    void mousePressEvent(QMouseEvent* event) override;
private:
    bool checked;
};

class MyReportsDialog : public QDialog {
    Q_OBJECT
public:
    explicit MyReportsDialog(const QString &employeeId, QWidget *parent = nullptr);

private:
    QTableView *tableView;
    QSqlTableModel *model;
    QLineEdit *searchEdit;
    QLabel *summaryLabel;
    QDateEdit *startDateEdit;
    QDateEdit *endDateEdit;
    QCheckBox *dateFilterCheck;
    QString currentEmployeeId;
    void deleteReport();
    void selectAllReports();
    void clearSelection();
    void exportSelectedToCSV();
    void applyFilter();
    void updateSummary();
};

#endif // MYREPORTSDIALOG_H
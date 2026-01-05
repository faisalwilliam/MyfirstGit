#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QTextEdit>
#include <QPushButton>
#include <vector>
#include <utility>
#include <QCloseEvent>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QString employeeId, QWidget *parent = nullptr);

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void calculateReport();
    void saveReport();
    void loadReport();
    void clearInputs();
    void about();
    void openSettings();
    void openProfile();
    void logout();
    void emailReport();
    void openAdminPanel();
    void restoreDatabase();
    void openMyReports();
    void saveToDatabase();

private:
    void setupUi();
    void applyTheme();

    QLineEdit *idEdit;
    QLineEdit *nameEdit;
    QDoubleSpinBox *wageSpinBox;
    QDoubleSpinBox *taxSpinBox;
    std::vector<std::pair<QSpinBox*, QSpinBox*>> dailyInputs; // Pairs of (Hours, Minutes)
    QTextEdit *reportDisplay;

    QString currencySymbol;
    double defaultTaxRate;
    int themeMode;
    QString currentEmployeeId;
    bool createPdfReport(const QString &fileName);
    void backupDatabase();
};

#endif // MAINWINDOW_H
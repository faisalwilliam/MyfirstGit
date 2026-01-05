#include "MainWindow.h"
#include "LoginDialog.h"
#include "SettingsDialog.h"
#include "ProfileDialog.h"
#include "AdminDialog.h"
#include "MyReportsDialog.h"
#include "Employee.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QGroupBox>
#include <QFileDialog>
#include <QMessageBox>
#include <fstream>
#include <sstream>
#include <QIcon>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QSettings>
#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QStyle>
#include <QDesktopServices>
#include <QUrl>
#include <QPrinter>
#include <QtCharts>
#include <QBuffer>
#include <QProcess>
#include <QStandardPaths>
#include <QDateTime>
#include <QDir>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

MainWindow::MainWindow(QString employeeId, QWidget *parent)
    : QMainWindow(parent), currentEmployeeId(employeeId)
{
    QSettings settings;
    currencySymbol = settings.value("currencySymbol", "kr ").toString();
    defaultTaxRate = settings.value("defaultTaxRate", 0.0).toDouble();
    themeMode = settings.value("themeMode", 0).toInt();
    setupUi();
    setWindowTitle("Employee Management System");
    resize(600, 500);
    setWindowIcon(QIcon(":/icon.png"));
    applyTheme();
}

void MainWindow::setupUi() {
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    // --- Employee Information Section ---
    QGroupBox *infoGroup = new QGroupBox("Employee Information");
    QGridLayout *infoLayout = new QGridLayout(infoGroup);

    idEdit = new QLineEdit(currentEmployeeId);
    idEdit->setReadOnly(true);
    nameEdit = new QLineEdit();
    wageSpinBox = new QDoubleSpinBox();
    wageSpinBox->setRange(0, 10000);
    wageSpinBox->setPrefix(currencySymbol);
    taxSpinBox = new QDoubleSpinBox();
    taxSpinBox->setRange(0, 100);
    taxSpinBox->setValue(defaultTaxRate);
    taxSpinBox->setSuffix("%");

    infoLayout->addWidget(new QLabel("Employee ID:"), 0, 0);
    infoLayout->addWidget(idEdit, 0, 1);
    infoLayout->addWidget(new QLabel("Name:"), 1, 0);
    infoLayout->addWidget(nameEdit, 1, 1);
    infoLayout->addWidget(new QLabel("Hourly Wage:"), 2, 0);
    infoLayout->addWidget(wageSpinBox, 2, 1);
    infoLayout->addWidget(new QLabel("Tax Rate:"), 3, 0);
    infoLayout->addWidget(taxSpinBox, 3, 1);

    mainLayout->addWidget(infoGroup);

    // --- Daily Hours Section ---
    QGroupBox *hoursGroup = new QGroupBox("Weekly Hours");
    QGridLayout *hoursLayout = new QGridLayout(hoursGroup);
    hoursLayout->addWidget(new QLabel("Day"), 0, 0);
    hoursLayout->addWidget(new QLabel("Hours"), 0, 1);
    hoursLayout->addWidget(new QLabel("Minutes"), 0, 2);

    for(int i = 0; i < 5; ++i) {
        hoursLayout->addWidget(new QLabel(QString("Day %1").arg(i + 1)), i + 1, 0);
        
        QSpinBox *h = new QSpinBox();
        h->setRange(0, 23);
        
        QSpinBox *m = new QSpinBox();
        m->setRange(0, 59);
        
        dailyInputs.push_back({h, m});
        hoursLayout->addWidget(h, i + 1, 1);
        hoursLayout->addWidget(m, i + 1, 2);
    }
    mainLayout->addWidget(hoursGroup);

    // --- Buttons Section ---
    QHBoxLayout *btnLayout = new QHBoxLayout();
    QPushButton *calcBtn = new QPushButton("Calculate Report");
    QPushButton *saveBtn = new QPushButton("Save Report");
    QPushButton *loadBtn = new QPushButton("Load Report");
    QPushButton *clearBtn = new QPushButton("Clear");
    QPushButton *logoutBtn = new QPushButton("Logout");
    QPushButton *emailBtn = new QPushButton("Email Report");
    QPushButton *saveDbBtn = new QPushButton("Save to DB");

    connect(calcBtn, &QPushButton::clicked, this, &MainWindow::calculateReport);
    connect(saveBtn, &QPushButton::clicked, this, &MainWindow::saveReport);
    connect(loadBtn, &QPushButton::clicked, this, &MainWindow::loadReport);
    connect(clearBtn, &QPushButton::clicked, this, &MainWindow::clearInputs);
    connect(logoutBtn, &QPushButton::clicked, this, &MainWindow::logout);
    connect(emailBtn, &QPushButton::clicked, this, &MainWindow::emailReport);
    connect(saveDbBtn, &QPushButton::clicked, this, &MainWindow::saveToDatabase);

    btnLayout->addWidget(calcBtn);
    btnLayout->addWidget(saveBtn);
    btnLayout->addWidget(loadBtn);
    btnLayout->addWidget(saveDbBtn);
    btnLayout->addWidget(emailBtn);
    btnLayout->addWidget(clearBtn);
    btnLayout->addWidget(logoutBtn);
    mainLayout->addLayout(btnLayout);

    // --- Output Section ---
    reportDisplay = new QTextEdit();
    reportDisplay->setReadOnly(true);
    mainLayout->addWidget(reportDisplay);

    // --- Menu Bar ---
    QMenuBar *menuBar = new QMenuBar(this);
    setMenuBar(menuBar);

    QMenu *fileMenu = menuBar->addMenu("File");
    QAction *settingsAction = fileMenu->addAction("Settings");
    connect(settingsAction, &QAction::triggered, this, &MainWindow::openSettings);
    QAction *myReportsAction = fileMenu->addAction("My Reports");
    connect(myReportsAction, &QAction::triggered, this, &MainWindow::openMyReports);
    QAction *profileAction = fileMenu->addAction("Profile");
    connect(profileAction, &QAction::triggered, this, &MainWindow::openProfile);
    QAction *logoutAction = fileMenu->addAction("Logout");
    connect(logoutAction, &QAction::triggered, this, &MainWindow::logout);
    QAction *exitAction = fileMenu->addAction("Exit");
    connect(exitAction, &QAction::triggered, this, &MainWindow::close);

    if (currentEmployeeId == "EMP001") {
        QMenu *adminMenu = menuBar->addMenu("Admin");
        QAction *viewAllAction = adminMenu->addAction("View All Employees");
        connect(viewAllAction, &QAction::triggered, this, &MainWindow::openAdminPanel);
        QAction *restoreAction = adminMenu->addAction("Restore Database");
        connect(restoreAction, &QAction::triggered, this, &MainWindow::restoreDatabase);
    }

    QMenu *helpMenu = menuBar->addMenu("Help");
    QAction *updateAction = helpMenu->addAction("Check for Updates");
    connect(updateAction, &QAction::triggered, [](){
        QDesktopServices::openUrl(QUrl("https://github.com/"));
    });
    QAction *aboutAction = helpMenu->addAction("About");
    connect(aboutAction, &QAction::triggered, this, &MainWindow::about);
}

void MainWindow::calculateReport() {
    QString name = nameEdit->text();
    if (name.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Please enter a name.");
        return;
    }

    Employee emp(currentEmployeeId.toStdString(), name.toStdString(), wageSpinBox->value(), taxSpinBox->value());

    for (size_t i = 0; i < dailyInputs.size(); ++i) {
        emp.setDailyWorkTime(i, dailyInputs[i].first->value(), dailyInputs[i].second->value());
    }

    reportDisplay->setText(QString::fromStdString(emp.output()));
}

void MainWindow::saveReport() {
    QString content = reportDisplay->toPlainText();
    if (content.isEmpty()) {
        QMessageBox::warning(this, "Save Error", "No report to save. Please calculate first.");
        return;
    }

    QString selectedFilter;
    QString fileName = QFileDialog::getSaveFileName(this, "Save Report", "", "Text Files (*.txt);;PDF Files (*.pdf)", &selectedFilter);
    if (fileName.isEmpty()) return;

    if (selectedFilter.contains("PDF") || fileName.endsWith(".pdf", Qt::CaseInsensitive)) {
        if (!fileName.endsWith(".pdf", Qt::CaseInsensitive)) fileName += ".pdf";
        if (createPdfReport(fileName)) {
            QMessageBox::information(this, "Success", "Report saved to PDF successfully.");
        }
    } else {
        if (!fileName.endsWith(".txt", Qt::CaseInsensitive)) fileName += ".txt";
        std::ofstream out(fileName.toStdString());
        if (out.is_open()) {
            out << content.toStdString();
            out.close();
            QMessageBox::information(this, "Success", "Report saved successfully.");
        } else {
            QMessageBox::critical(this, "Error", "Could not save file.");
        }
    }
}

void MainWindow::saveToDatabase() {
    QString name = nameEdit->text();
    if (name.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Please enter a name.");
        return;
    }

    Employee emp(currentEmployeeId.toStdString(), name.toStdString(), wageSpinBox->value(), taxSpinBox->value());

    for (size_t i = 0; i < dailyInputs.size(); ++i) {
        emp.setDailyWorkTime(i, dailyInputs[i].first->value(), dailyInputs[i].second->value());
    }
    emp.salary();

    QSqlQuery query;
    query.prepare("INSERT INTO reports (employee_id, name, report_date, total_hours, gross_wage, tax, net_wage) VALUES (:id, :name, :date, :hours, :gross, :tax, :net)");
    query.bindValue(":id", currentEmployeeId);
    query.bindValue(":name", name);
    query.bindValue(":date", QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
    query.bindValue(":hours", emp.getTotalHours());
    query.bindValue(":gross", emp.getWeeklyWage());
    query.bindValue(":tax", emp.getTax());
    query.bindValue(":net", emp.getNetWage());

    if (query.exec()) {
        QMessageBox::information(this, "Success", "Report saved to database.");
    } else {
        QMessageBox::critical(this, "Database Error", query.lastError().text());
    }
}

bool MainWindow::createPdfReport(const QString &fileName) {
    // --- 1. Generate Chart ---
    QtCharts::QBarSet *set = new QtCharts::QBarSet("Hours Worked");
    for (const auto& pair : dailyInputs) {
        double hours = pair.first->value() + (pair.second->value() / 60.0);
        set->append(hours);
    }

    QtCharts::QBarSeries *series = new QtCharts::QBarSeries();
    series->append(set);

    QtCharts::QChart *chart = new QtCharts::QChart();
    chart->addSeries(series);
    chart->setTitle("Weekly Work Hours");
    chart->setAnimationOptions(QtCharts::QChart::SeriesAnimations);

    QStringList categories;
    for (size_t i = 0; i < dailyInputs.size(); ++i) {
        categories << QString("Day %1").arg(i + 1);
    }

    QtCharts::QBarCategoryAxis *axisX = new QtCharts::QBarCategoryAxis();
    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QtCharts::QValueAxis *axisY = new QtCharts::QValueAxis();
    axisY->setRange(0, 24); // Max 24 hours in a day
    axisY->setTitleText("Hours");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    // --- 2. Render Chart to Image ---
    QtCharts::QChartView chartView(chart);
    chartView.resize(800, 500);
    chartView.setRenderHint(QPainter::Antialiasing);

    QImage image(800, 500, QImage::Format_ARGB32);
    image.fill(Qt::white);
    QPainter painter(&image);
    chartView.render(&painter);

    // --- 3. Convert to Base64 for HTML Embedding ---
    QByteArray bytes;
    QBuffer buffer(&bytes);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, "PNG");
    QString base64Image = bytes.toBase64();

    // --- 4. Create PDF with Text and Image ---
    QTextDocument doc;
    QString html = reportDisplay->toHtml();
    html += "<br><hr><br>";
    html += QString("<div align='center'><img src='data:image/png;base64,%1'></div>").arg(base64Image);
    doc.setHtml(html);

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);
    doc.print(&printer);

    return true;
}

void MainWindow::emailReport() {
    if (reportDisplay->toPlainText().isEmpty()) {
        QMessageBox::warning(this, "Email Error", "No report to email. Please calculate first.");
        return;
    }

    QString tempPath = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/EmployeeReport.pdf";
    if (createPdfReport(tempPath)) {
        // Try to use xdg-email (Linux standard) to attach the file
        QStringList args;
        args << "--attach" << tempPath;
        args << "--subject" << "Employee Report";
        args << "--body" << "Please find the attached employee report.";

        if (!QProcess::startDetached("xdg-email", args)) {
            // Fallback if xdg-email is missing
            QDesktopServices::openUrl(QUrl("mailto:?subject=Employee Report&body=Please find the attached employee report."));
            QMessageBox::information(this, "Email", "Default mail client opened. Please attach the report manually from: " + tempPath);
        }
    }
}

void MainWindow::openProfile() {
    ProfileDialog dlg(currentEmployeeId, this);
    if (dlg.exec() == 2) { // Code 2 indicates account deletion
        logout();
    }
}

void MainWindow::closeEvent(QCloseEvent *event) {
    backupDatabase();
    QMainWindow::closeEvent(event);
}

void MainWindow::backupDatabase() {
    QString dbPath = "employees.db";
    if (QFile::exists(dbPath)) {
        QString backupDir = "backups";
        if (!QDir(backupDir).exists()) {
            QDir().mkpath(backupDir);
        }
        QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd_HH-mm-ss");
        QString backupPath = QString("%1/employees_backup_%2.db").arg(backupDir, timestamp);
        QFile::copy(dbPath, backupPath);
    }
}

void MainWindow::logout() {
    this->hide();
    LoginDialog login;
    if (login.exec() == QDialog::Accepted) {
        currentEmployeeId = login.getEmployeeID();
        idEdit->setText(currentEmployeeId);
        
        nameEdit->clear();
        wageSpinBox->setValue(0.0);
        taxSpinBox->setValue(defaultTaxRate);
        for (auto& pair : dailyInputs) {
            pair.first->setValue(0);
            pair.second->setValue(0);
        }
        reportDisplay->clear();
        
        this->show();
    } else {
        this->close();
    }
}

void MainWindow::loadReport() {
    QString fileName = QFileDialog::getOpenFileName(this, "Open Report", "", "Text Files (*.txt)");
    if (fileName.isEmpty()) return;

    std::ifstream in(fileName.toStdString());
    if (in.is_open()) {
        std::stringstream buffer;
        buffer << in.rdbuf();
        reportDisplay->setText(QString::fromStdString(buffer.str()));
    } else {
        QMessageBox::critical(this, "Error", "Could not open file.");
    }
}

void MainWindow::clearInputs() {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirm Clear", "Are you sure you want to clear all inputs?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        nameEdit->clear();
        wageSpinBox->setValue(0.0);
        taxSpinBox->setValue(defaultTaxRate);
        for (auto& pair : dailyInputs) {
            pair.first->setValue(0);
            pair.second->setValue(0);
        }
        reportDisplay->clear();
    }
}

void MainWindow::about() {
    QMessageBox::about(this, "About Employee Management System",
                       "Employee Management System v1.0\n\n"
                       "A simple tool to calculate weekly wages and tax.\n"
                       "Built with C++ and Qt.");
}

void MainWindow::openSettings() {
    SettingsDialog dlg(currencySymbol, defaultTaxRate, themeMode, this);
    if (dlg.exec() == QDialog::Accepted) {
        currencySymbol = dlg.getCurrencySymbol();
        defaultTaxRate = dlg.getDefaultTaxRate();
        themeMode = dlg.getThemeMode();
        wageSpinBox->setPrefix(currencySymbol);

        QSettings settings;
        settings.setValue("currencySymbol", currencySymbol);
        settings.setValue("defaultTaxRate", defaultTaxRate);
        settings.setValue("themeMode", themeMode);
        applyTheme();
    }
}

void MainWindow::openAdminPanel() {
    AdminDialog dlg(this);
    dlg.exec();
}

void MainWindow::openMyReports() {
    MyReportsDialog dlg(currentEmployeeId, this);
    dlg.exec();
}

void MainWindow::restoreDatabase() {
    QString fileName = QFileDialog::getOpenFileName(this, "Select Backup File", "backups", "Database Files (*.db)");
    if (fileName.isEmpty()) return;

    auto reply = QMessageBox::warning(this, "Confirm Restore", 
        "Restoring the database will overwrite all current data.\nThe application will restart.\nAre you sure?",
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        // Close database connection
        {
            QSqlDatabase db = QSqlDatabase::database();
            db.close();
        }
        QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);

        QString dbFile = "employees.db";
        QString tempFile = "employees_temp.db";
        
        // Backup current DB to temp just in case copy fails
        if (QFile::exists(dbFile)) {
            QFile::rename(dbFile, tempFile);
        }

        if (QFile::copy(fileName, dbFile)) {
            QFile::remove(tempFile); // Restore successful, delete temp
            QMessageBox::information(this, "Success", "Database restored successfully. Application will restart.");
            qApp->quit();
            QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
        } else {
            if (QFile::exists(tempFile)) QFile::rename(tempFile, dbFile); // Restore failed, revert
            QMessageBox::critical(this, "Error", "Could not restore database file.");
            // Re-open database
            QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
            db.setDatabaseName(dbFile);
            db.open();
        }
    }
}

void MainWindow::applyTheme() {
    bool useDark = false;
    if (themeMode == 2) {
        useDark = true;
    } else if (themeMode == 1) {
        useDark = false;
    } else {
        // System Theme: Check standard palette brightness
        useDark = QApplication::style()->standardPalette().color(QPalette::Window).value() < 128;
    }

    if (useDark) {
        qApp->setStyleSheet(
            "QMainWindow { background-color: #2b2b2b; color: #ffffff; }"
            "QGroupBox { font-weight: bold; border: 1px solid #555; border-radius: 5px; margin-top: 10px; background-color: #323232; color: #ffffff; }"
            "QGroupBox::title { subcontrol-origin: margin; subcontrol-position: top left; padding: 0 3px; left: 10px; color: #dddddd; }"
            "QLabel { color: #dddddd; font-size: 13px; }"
            "QLineEdit, QSpinBox, QDoubleSpinBox { padding: 5px; border: 1px solid #555; border-radius: 3px; background-color: #404040; color: #ffffff; }"
            "QLineEdit:focus, QSpinBox:focus, QDoubleSpinBox:focus { border: 1px solid #3498db; background-color: #505050; }"
            "QPushButton { background-color: #3498db; color: white; border: none; padding: 8px 15px; border-radius: 4px; font-weight: bold; }"
            "QPushButton:hover { background-color: #2980b9; }"
            "QPushButton:pressed { background-color: #1f618d; }"
            "QTextEdit { border: 1px solid #555; border-radius: 3px; background-color: #404040; color: #ffffff; font-family: monospace; }"
            "QMenuBar { background-color: #2b2b2b; color: #ffffff; }"
            "QMenuBar::item:selected { background-color: #3a3a3a; }"
            "QMenu { background-color: #2b2b2b; color: #ffffff; border: 1px solid #555; }"
            "QMenu::item:selected { background-color: #3498db; }"
        );
    } else {
        QFile file(":/style.qss");
        if (file.open(QFile::ReadOnly | QFile::Text)) {
            QTextStream stream(&file);
            qApp->setStyleSheet(stream.readAll());
            file.close();
        }
    }
}
#include "AdminDialog.h"
#include "AdminReportsDialog.h"
#include <QVBoxLayout>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QFile>
#include <QSqlQuery>
#include <QSqlError>
#include <QCryptographicHash>
#include <QDateTime>
#include <iostream>
#include <QTextEdit>

AdminDialog::AdminDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Admin Panel - All Employees");
    resize(600, 400);

    QVBoxLayout *layout = new QVBoxLayout(this);

    QHBoxLayout *searchLayout = new QHBoxLayout();
    layout->addLayout(searchLayout);

    searchLayout->addWidget(new QLabel("Search ID:", this));
    searchEdit = new QLineEdit(this);
    searchEdit->setPlaceholderText("Filter by Employee ID...");
    searchLayout->addWidget(searchEdit);

    QHBoxLayout *actionLayout = new QHBoxLayout();
    layout->addLayout(actionLayout);

    QPushButton *exportBtn = new QPushButton("Export to CSV", this);
    actionLayout->addWidget(exportBtn);
    connect(exportBtn, &QPushButton::clicked, this, &AdminDialog::exportToCSV);

    QPushButton *importBtn = new QPushButton("Import from CSV", this);
    actionLayout->addWidget(importBtn);
    connect(importBtn, &QPushButton::clicked, this, &AdminDialog::importFromCSV);

    QPushButton *unlockBtn = new QPushButton("Unlock User", this);
    actionLayout->addWidget(unlockBtn);
    connect(unlockBtn, &QPushButton::clicked, this, &AdminDialog::unlockUser);

    QPushButton *deleteBtn = new QPushButton("Delete Selected User", this);
    deleteBtn->setStyleSheet("background-color: #e74c3c; color: white; font-weight: bold;");
    actionLayout->addWidget(deleteBtn);
    connect(deleteBtn, &QPushButton::clicked, this, &AdminDialog::deleteUser);

    QPushButton *resetBtn = new QPushButton("Reset DB", this);
    resetBtn->setStyleSheet("background-color: #c0392b; color: white; font-weight: bold;");
    actionLayout->addWidget(resetBtn);
    connect(resetBtn, &QPushButton::clicked, this, &AdminDialog::resetDatabase);

    QPushButton *backupBtn = new QPushButton("Backup DB", this);
    actionLayout->addWidget(backupBtn);
    connect(backupBtn, &QPushButton::clicked, this, &AdminDialog::backupDatabase);

    QPushButton *compactBtn = new QPushButton("Compact DB", this);
    actionLayout->addWidget(compactBtn);
    connect(compactBtn, &QPushButton::clicked, this, &AdminDialog::compactDatabase);

    QPushButton *debugBtn = new QPushButton("Print to Console", this);
    actionLayout->addWidget(debugBtn);
    connect(debugBtn, &QPushButton::clicked, this, &AdminDialog::dumpToConsole);

    QPushButton *viewTextBtn = new QPushButton("View as Text", this);
    actionLayout->addWidget(viewTextBtn);
    connect(viewTextBtn, &QPushButton::clicked, this, &AdminDialog::viewAsText);

    QPushButton *viewReportsBtn = new QPushButton("View All Reports", this);
    actionLayout->addWidget(viewReportsBtn);
    connect(viewReportsBtn, &QPushButton::clicked, this, &AdminDialog::viewAllReports);

    tableView = new QTableView(this);
    model = new QSqlTableModel(this);
    model->setTable("users");
    model->setEditStrategy(QSqlTableModel::OnFieldChange);
    model->select();
    
    model->setHeaderData(0, Qt::Horizontal, "Employee ID");
    model->setHeaderData(1, Qt::Horizontal, "Password (Hash)");
    model->setHeaderData(2, Qt::Horizontal, "Last Login");
    model->setHeaderData(3, Qt::Horizontal, "Failed Attempts");
    model->setHeaderData(4, Qt::Horizontal, "Locked Until");

    tableView->setModel(model);
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    
    layout->addWidget(tableView);

    connect(searchEdit, &QLineEdit::textChanged, [this](const QString &text){
        model->setFilter(QString("employee_id LIKE '%%1%'").arg(text));
        model->select();
    });
}

void AdminDialog::exportToCSV() {
#ifdef Q_OS_WASM
    QString content;
    QTextStream out(&content);
    
    while (model->canFetchMore())
        model->fetchMore();

    // Headers
    for (int i = 0; i < model->columnCount(); ++i) {
        out << model->headerData(i, Qt::Horizontal).toString();
        if (i < model->columnCount() - 1) out << ",";
    }
    out << "\n";

    // Data
    for (int i = 0; i < model->rowCount(); ++i) {
        for (int j = 0; j < model->columnCount(); ++j) {
            out << model->data(model->index(i, j)).toString();
            if (j < model->columnCount() - 1) out << ",";
        }
        out << "\n";
    }
    QFileDialog::saveFileContent(content.toUtf8(), "all_employees.csv");
    return;
#endif

    QString fileName = QFileDialog::getSaveFileName(this, "Export CSV", "", "CSV Files (*.csv)");
    if (fileName.isEmpty()) return;

    if (!fileName.endsWith(".csv", Qt::CaseInsensitive)) fileName += ".csv";

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        
        while (model->canFetchMore())
            model->fetchMore();

        // Headers
        for (int i = 0; i < model->columnCount(); ++i) {
            out << model->headerData(i, Qt::Horizontal).toString();
            if (i < model->columnCount() - 1) out << ",";
        }
        out << "\n";

        // Data
        for (int i = 0; i < model->rowCount(); ++i) {
            for (int j = 0; j < model->columnCount(); ++j) {
                out << model->data(model->index(i, j)).toString();
                if (j < model->columnCount() - 1) out << ",";
            }
            out << "\n";
        }
        file.close();
        QMessageBox::information(this, "Success", "Data exported successfully.");
    } else {
        QMessageBox::critical(this, "Error", "Could not save file.");
    }
}

void AdminDialog::deleteUser() {
    QModelIndexList selected = tableView->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        QMessageBox::warning(this, "Selection Error", "Please select a user to delete.");
        return;
    }

    int row = selected.first().row();
    QString empId = model->data(model->index(row, 0)).toString();

    if (empId == "EMP001") {
        QMessageBox::warning(this, "Action Denied", "Cannot delete the main admin account.");
        return;
    }

    auto reply = QMessageBox::question(this, "Confirm Delete", "Are you sure you want to delete user " + empId + "?", QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        model->removeRow(row);
        model->select();
        logAction("Deleted user: " + empId);
    }
}

void AdminDialog::importFromCSV() {
    QString fileName = QFileDialog::getOpenFileName(this, "Import CSV", "", "CSV Files (*.csv)");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        
        // Check for header and skip if present
        if (!in.atEnd()) {
            qint64 pos = in.pos();
            QString line = in.readLine();
            if (!line.contains("Employee ID", Qt::CaseInsensitive)) {
                in.seek(pos);
            }
        }

        int count = 0;
        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList parts = line.split(",");
            if (parts.size() >= 2) {
                QString id = parts[0].trimmed();
                QString pass = parts[1].trimmed();
                if (id.isEmpty() || pass.isEmpty()) continue;

                QString hash = QString(QCryptographicHash::hash(pass.toUtf8(), QCryptographicHash::Sha256).toHex());
                QSqlQuery query;
                query.prepare("INSERT OR IGNORE INTO users (employee_id, password) VALUES (:id, :pass)");
                query.bindValue(":id", id);
                query.bindValue(":pass", hash);
                if (query.exec() && query.numRowsAffected() > 0) count++;
            }
        }
        file.close();
        model->select();
        QMessageBox::information(this, "Success", QString("Imported %1 users.").arg(count));
        logAction(QString("Imported %1 users from CSV").arg(count));
    } else {
        QMessageBox::critical(this, "Error", "Could not open file.");
    }
}

void AdminDialog::unlockUser() {
    QModelIndexList selected = tableView->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        QMessageBox::warning(this, "Selection Error", "Please select a user to unlock.");
        return;
    }

    int row = selected.first().row();
    QString empId = model->data(model->index(row, 0)).toString();

    QSqlQuery query;
    query.prepare("UPDATE users SET failed_attempts = 0, locked_until = NULL WHERE employee_id = :id");
    query.bindValue(":id", empId);
    
    if (query.exec()) {
        QMessageBox::information(this, "Success", "User account unlocked.");
        model->select();
        logAction("Unlocked user: " + empId);
    } else {
        QMessageBox::critical(this, "Database Error", query.lastError().text());
    }
}

void AdminDialog::dumpToConsole() {
    QSqlQuery query("SELECT * FROM users");
    std::cout << "\n=== DATABASE DUMP ===" << std::endl;
    std::cout << "ID              | Last Login              | Failed | Locked Until" << std::endl;
    std::cout << "------------------------------------------------------------------------" << std::endl;
    
    while (query.next()) {
        QString id = query.value("employee_id").toString();
        QString lastLogin = query.value("last_login").toString();
        QString failed = query.value("failed_attempts").toString();
        QString locked = query.value("locked_until").toString();
        std::cout << id.leftJustified(15).toStdString() << " | " 
                  << lastLogin.leftJustified(23).toStdString() << " | " 
                  << failed.leftJustified(6).toStdString() << " | " 
                  << locked.toStdString() << std::endl;
    }
    std::cout << "========================================================================\n" << std::endl;
}

void AdminDialog::viewAsText() {
    QSqlQuery query("SELECT * FROM users");
    QString textData;
    textData += "ID             | Last Login             | Failed | Locked Until           | Password Hash\n";
    textData += "----------------------------------------------------------------------------------------------------\n";
    
    while (query.next()) {
        QString id = query.value("employee_id").toString();
        QString lastLogin = query.value("last_login").toString();
        QString failed = query.value("failed_attempts").toString();
        QString locked = query.value("locked_until").toString();
        QString passHash = query.value("password").toString();
        textData += QString("%1 | %2 | %3 | %4 | %5\n")
                        .arg(id.leftJustified(14))
                        .arg(lastLogin.leftJustified(22))
                        .arg(failed.leftJustified(6))
                        .arg(locked.leftJustified(22))
                        .arg(passHash);
    }

    QDialog *textDialog = new QDialog(this);
    textDialog->setWindowTitle("Database Content (Text View)");    textDialog->resize(800, 400);
    QVBoxLayout *layout = new QVBoxLayout(textDialog);
    
    QTextEdit *textEdit = new QTextEdit(textDialog);
    textEdit->setReadOnly(true);
    textEdit->setFontFamily("Monospace");
    textEdit->setText(textData);
    layout->addWidget(textEdit);
    
    QPushButton *closeBtn = new QPushButton("Close", textDialog);
    connect(closeBtn, &QPushButton::clicked, textDialog, &QDialog::accept);
    layout->addWidget(closeBtn);
    
    textDialog->exec();
}

void AdminDialog::viewAllReports() {
    AdminReportsDialog dlg(this);
    dlg.exec();
}

void AdminDialog::resetDatabase() {
    auto reply = QMessageBox::question(this, "Reset Database", 
        "Are you sure you want to delete ALL users except the admin (EMP001)?\nThis action cannot be undone.",
        QMessageBox::Yes | QMessageBox::No);
        
    if (reply == QMessageBox::Yes) {
        QSqlQuery query;
        if (query.exec("DELETE FROM users WHERE employee_id != 'EMP001'")) {
            QMessageBox::information(this, "Success", "Database reset successfully.");
            model->select();
            logAction("Database reset (all users deleted except admin)");
        } else {
            QMessageBox::critical(this, "Database Error", query.lastError().text());
        }
    }
}

void AdminDialog::compactDatabase() {
    QSqlQuery query;
    if (query.exec("VACUUM")) {
        QMessageBox::information(this, "Success", "Database compacted successfully.");
        logAction("Database compacted (VACUUM)");
    } else {
        QMessageBox::critical(this, "Database Error", query.lastError().text());
    }
}

void AdminDialog::logAction(const QString &message) {
    QFile file("admin_log.txt");
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        out << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") << " - " << message << "\n";
        file.close();
    }
}

void AdminDialog::backupDatabase() {
    QString fileName = QFileDialog::getSaveFileName(this, "Backup Database", "", "Database Files (*.db)");
    if (fileName.isEmpty()) return;

    if (!fileName.endsWith(".db", Qt::CaseInsensitive)) fileName += ".db";

    QString dbPath = "employees.db";
    if (QFile::exists(dbPath)) {
        if (QFile::exists(fileName)) {
            QFile::remove(fileName);
        }
        if (QFile::copy(dbPath, fileName)) {
            QMessageBox::information(this, "Success", "Database backed up successfully.");
            logAction("Database backed up to: " + fileName);
        } else {
            QMessageBox::critical(this, "Error", "Could not backup database.");
        }
    }
}
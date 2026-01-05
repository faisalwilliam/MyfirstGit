#include "ProfileDialog.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QCryptographicHash>

ProfileDialog::ProfileDialog(const QString &employeeId, QWidget *parent) 
    : QDialog(parent), currentEmployeeId(employeeId) 
{
    setWindowTitle("User Profile");
    setFixedSize(300, 450);

    QVBoxLayout *layout = new QVBoxLayout(this);

    layout->addWidget(new QLabel("Employee ID: " + currentEmployeeId, this));

    layout->addWidget(new QLabel("New Username (ID):", this));
    newIdEdit = new QLineEdit(this);
    layout->addWidget(newIdEdit);

    QPushButton *changeUserBtn = new QPushButton("Change Username", this);
    layout->addWidget(changeUserBtn);
    connect(changeUserBtn, &QPushButton::clicked, this, &ProfileDialog::changeUsername);

    layout->addSpacing(10);

    layout->addWidget(new QLabel("Current Password:", this));
    currentPasswordEdit = new QLineEdit(this);
    currentPasswordEdit->setEchoMode(QLineEdit::Password);
    layout->addWidget(currentPasswordEdit);

    layout->addWidget(new QLabel("New Password:", this));
    newPasswordEdit = new QLineEdit(this);
    newPasswordEdit->setEchoMode(QLineEdit::Password);
    layout->addWidget(newPasswordEdit);

    layout->addWidget(new QLabel("Confirm New Password:", this));
    confirmPasswordEdit = new QLineEdit(this);
    confirmPasswordEdit->setEchoMode(QLineEdit::Password);
    layout->addWidget(confirmPasswordEdit);

    QPushButton *helpBtn = new QPushButton("Password Help", this);
    layout->addWidget(helpBtn);
    connect(helpBtn, &QPushButton::clicked, [this]() {
        QMessageBox::information(this, "Password Requirements",
            "Password must meet the following criteria:\n"
            "- At least 8 characters long\n"
            "- Contains at least one uppercase letter\n"
            "- Contains at least one digit\n"
            "- Contains at least one special character");
    });

    QPushButton *changeBtn = new QPushButton("Change Password", this);
    layout->addWidget(changeBtn);
    connect(changeBtn, &QPushButton::clicked, this, &ProfileDialog::changePassword);

    QPushButton *deleteBtn = new QPushButton("Delete Account", this);
    deleteBtn->setStyleSheet("background-color: #e74c3c; color: white; font-weight: bold;");
    layout->addWidget(deleteBtn);
    connect(deleteBtn, &QPushButton::clicked, this, &ProfileDialog::deleteAccount);

    QPushButton *closeBtn = new QPushButton("Close", this);
    layout->addWidget(closeBtn);
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::reject);
}

bool ProfileDialog::validatePassword(const QString &password) {
    if (password.length() < 8) return false;
    bool hasSpecial = false;
    bool hasDigit = false;
    bool hasUpper = false;
    for (const QChar &c : password) {
        if (!c.isLetterOrNumber()) hasSpecial = true;
        if (c.isDigit()) hasDigit = true;
        if (c.isUpper()) hasUpper = true;
    }
    return hasSpecial && hasDigit && hasUpper;
}

void ProfileDialog::changeUsername() {
    QString newId = newIdEdit->text().trimmed();
    if (newId.isEmpty()) {
        QMessageBox::warning(this, "Error", "New ID cannot be empty.");
        return;
    }

    if (newId == currentEmployeeId) {
        QMessageBox::warning(this, "Error", "New ID is the same as current ID.");
        return;
    }

    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT employee_id FROM users WHERE employee_id = :id");
    checkQuery.bindValue(":id", newId);
    if (checkQuery.exec() && checkQuery.next()) {
        QMessageBox::warning(this, "Error", "Employee ID already exists.");
        return;
    }

    QSqlQuery updateQuery;
    updateQuery.prepare("UPDATE users SET employee_id = :newId WHERE employee_id = :oldId");
    updateQuery.bindValue(":newId", newId);
    updateQuery.bindValue(":oldId", currentEmployeeId);

    if (updateQuery.exec()) {
        QMessageBox::information(this, "Success", "Username changed. Please login again.");
        done(2); // Force logout
    } else {
        QMessageBox::critical(this, "Database Error", updateQuery.lastError().text());
    }
}

void ProfileDialog::changePassword() {
    QString currentPass = currentPasswordEdit->text();
    QString newPass = newPasswordEdit->text();
    QString confirmPass = confirmPasswordEdit->text();

    if (currentPass.isEmpty() || newPass.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please fill in all fields.");
        return;
    }

    if (newPass != confirmPass) {
        QMessageBox::warning(this, "Error", "New passwords do not match.");
        return;
    }

    if (!validatePassword(newPass)) {
        QMessageBox::warning(this, "Error", "Password must be at least 8 characters long and contain:\n- One uppercase letter\n- One digit\n- One special character");
        return;
    }

    // Verify current password
    QSqlQuery query;
    query.prepare("SELECT password FROM users WHERE employee_id = :id");
    query.bindValue(":id", currentEmployeeId);
    
    if (query.exec() && query.next()) {
        QString storedHash = query.value(0).toString();
        QString inputHash = QString(QCryptographicHash::hash(currentPass.toUtf8(), QCryptographicHash::Sha256).toHex());
        
        if (storedHash != inputHash) {
            QMessageBox::warning(this, "Error", "Incorrect current password.");
            return;
        }

        // Update password
        QString newHash = QString(QCryptographicHash::hash(newPass.toUtf8(), QCryptographicHash::Sha256).toHex());
        QSqlQuery updateQuery;
        updateQuery.prepare("UPDATE users SET password = :pass WHERE employee_id = :id");
        updateQuery.bindValue(":pass", newHash);
        updateQuery.bindValue(":id", currentEmployeeId);
        
        if (updateQuery.exec()) {
            QMessageBox::information(this, "Success", "Password changed successfully.");
            accept();
        } else {
            QMessageBox::critical(this, "Database Error", updateQuery.lastError().text());
        }
    } else {
        QMessageBox::critical(this, "Error", "User not found.");
    }
}

void ProfileDialog::deleteAccount() {
    auto reply = QMessageBox::question(this, "Delete Account", 
        "Are you sure you want to delete your account?\nThis action cannot be undone.",
        QMessageBox::Yes | QMessageBox::No);
        
    if (reply == QMessageBox::Yes) {
        QSqlQuery query;
        query.prepare("DELETE FROM users WHERE employee_id = :id");
        query.bindValue(":id", currentEmployeeId);
        
        if (query.exec()) {
            QMessageBox::information(this, "Account Deleted", "Your account has been deleted.");
            done(2); // Return custom code 2 to signal deletion
        } else {
            QMessageBox::critical(this, "Error", "Could not delete account: " + query.lastError().text());
        }
    }
}
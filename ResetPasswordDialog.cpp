#include "ResetPasswordDialog.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QCryptographicHash>

ResetPasswordDialog::ResetPasswordDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Reset Password");
    setFixedSize(300, 350);

    QVBoxLayout *layout = new QVBoxLayout(this);

    layout->addWidget(new QLabel("Employee ID:", this));
    idEdit = new QLineEdit(this);
    layout->addWidget(idEdit);

    layout->addWidget(new QLabel("Recovery Key (admin123):", this));
    recoveryKeyEdit = new QLineEdit(this);
    recoveryKeyEdit->setEchoMode(QLineEdit::Password);
    layout->addWidget(recoveryKeyEdit);

    layout->addWidget(new QLabel("New Password:", this));
    newPasswordEdit = new QLineEdit(this);
    newPasswordEdit->setEchoMode(QLineEdit::Password);
    layout->addWidget(newPasswordEdit);

    layout->addWidget(new QLabel("Confirm Password:", this));
    confirmPasswordEdit = new QLineEdit(this);
    confirmPasswordEdit->setEchoMode(QLineEdit::Password);
    layout->addWidget(confirmPasswordEdit);

    QPushButton *resetBtn = new QPushButton("Reset Password", this);
    layout->addWidget(resetBtn);
    connect(resetBtn, &QPushButton::clicked, this, &ResetPasswordDialog::resetPassword);

    QPushButton *cancelBtn = new QPushButton("Cancel", this);
    layout->addWidget(cancelBtn);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
}

bool ResetPasswordDialog::validatePassword(const QString &password) {
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

void ResetPasswordDialog::resetPassword() {
    QString id = idEdit->text().trimmed();
    QString key = recoveryKeyEdit->text();
    QString pass = newPasswordEdit->text();
    QString confirmPass = confirmPasswordEdit->text();

    if (id.isEmpty() || key.isEmpty() || pass.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please fill in all fields.");
        return;
    }

    // Hardcoded recovery key for demonstration
    if (key != "admin123") {
        QMessageBox::warning(this, "Error", "Invalid Recovery Key.");
        return;
    }

    if (pass != confirmPass) {
        QMessageBox::warning(this, "Error", "Passwords do not match.");
        return;
    }

    if (!validatePassword(pass)) {
        QMessageBox::warning(this, "Error", "Password must be at least 8 characters long and contain:\n- One uppercase letter\n- One digit\n- One special character");
        return;
    }

    QString hashedPassword = QString(QCryptographicHash::hash(pass.toUtf8(), QCryptographicHash::Sha256).toHex());
    QSqlQuery updateQuery;
    updateQuery.prepare("UPDATE users SET password = :pass WHERE employee_id = :id");
    updateQuery.bindValue(":pass", hashedPassword);
    updateQuery.bindValue(":id", id);

    if (updateQuery.exec() && updateQuery.numRowsAffected() > 0) {
        QMessageBox::information(this, "Success", "Password reset successfully.");
        accept();
    } else {
        QMessageBox::warning(this, "Error", "Employee ID not found or database error.");
    }
}
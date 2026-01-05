#include "RegisterDialog.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QCryptographicHash>

RegisterDialog::RegisterDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Register New User");
    setFixedSize(300, 280);

    QVBoxLayout *layout = new QVBoxLayout(this);

    layout->addWidget(new QLabel("Employee ID:", this));
    idEdit = new QLineEdit(this);
    layout->addWidget(idEdit);

    layout->addWidget(new QLabel("Password:", this));
    passwordEdit = new QLineEdit(this);
    passwordEdit->setEchoMode(QLineEdit::Password);
    layout->addWidget(passwordEdit);

    layout->addWidget(new QLabel("Confirm Password:", this));
    confirmPasswordEdit = new QLineEdit(this);
    confirmPasswordEdit->setEchoMode(QLineEdit::Password);
    layout->addWidget(confirmPasswordEdit);

    QPushButton *registerBtn = new QPushButton("Register", this);
    layout->addWidget(registerBtn);
    connect(registerBtn, &QPushButton::clicked, this, &RegisterDialog::registerUser);

    QPushButton *cancelBtn = new QPushButton("Cancel", this);
    layout->addWidget(cancelBtn);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
}

bool RegisterDialog::validatePassword(const QString &password) {
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

void RegisterDialog::registerUser() {
    QString id = idEdit->text().trimmed();
    QString pass = passwordEdit->text();
    QString confirmPass = confirmPasswordEdit->text();

    if (id.isEmpty() || pass.isEmpty()) {
        QMessageBox::warning(this, "Registration Failed", "Please fill in all fields.");
        return;
    }

    if (pass != confirmPass) {
        QMessageBox::warning(this, "Registration Failed", "Passwords do not match.");
        return;
    }

    if (!validatePassword(pass)) {
        QMessageBox::warning(this, "Registration Failed", "Password must be at least 8 characters long and contain:\n- One uppercase letter\n- One digit\n- One special character");
        return;
    }

    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT employee_id FROM users WHERE employee_id = :id");
    checkQuery.bindValue(":id", id);
    if (checkQuery.exec() && checkQuery.next()) {
        QMessageBox::warning(this, "Registration Failed", "Employee ID already exists.");
        return;
    }

    QString hashedPassword = QString(QCryptographicHash::hash(pass.toUtf8(), QCryptographicHash::Sha256).toHex());
    QSqlQuery insertQuery;
    insertQuery.prepare("INSERT INTO users (employee_id, password) VALUES (:id, :pass)");
    insertQuery.bindValue(":id", id);
    insertQuery.bindValue(":pass", hashedPassword);

    if (insertQuery.exec()) {
        QMessageBox::information(this, "Success", "User registered successfully.");
        accept();
    } else {
        QMessageBox::critical(this, "Database Error", insertQuery.lastError().text());
    }
}
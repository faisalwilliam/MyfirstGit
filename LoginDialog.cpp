#include "LoginDialog.h"
#include "RegisterDialog.h"
#include "ResetPasswordDialog.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QCheckBox>
#include <QCryptographicHash>
#include <QSettings>
#include <QProcess>
#include <QStandardPaths>
#include <QDateTime>
#include <QTimer>
#include <QCoreApplication>
#include <QFile>
#include <QDebug>

LoginDialog::LoginDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Login");
    setFixedSize(300, 260);

    QVBoxLayout *layout = new QVBoxLayout(this);

    QLabel *idLabel = new QLabel("Employee ID:", this);
    layout->addWidget(idLabel);

    idEdit = new QLineEdit(this);
    idEdit->setPlaceholderText("e.g., EMP001");

    QSettings settings;
    if (settings.value("rememberMe", false).toBool()) {
        idEdit->setText(settings.value("savedEmployeeId", "").toString());
    }

    layout->addWidget(idEdit);

    QLabel *passLabel = new QLabel("Password:", this);
    layout->addWidget(passLabel);

    passwordEdit = new QLineEdit(this);
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setPlaceholderText("Enter Password");
    layout->addWidget(passwordEdit);

    QCheckBox *showPassCheck = new QCheckBox("Show Password", this);
    layout->addWidget(showPassCheck);

    connect(showPassCheck, &QCheckBox::toggled, [this](bool checked) {
        passwordEdit->setEchoMode(checked ? QLineEdit::Normal : QLineEdit::Password);
    });

    rememberMeCheck = new QCheckBox("Remember Me", this);
    rememberMeCheck->setChecked(settings.value("rememberMe", false).toBool());
    layout->addWidget(rememberMeCheck);

    loginBtn = new QPushButton("Login", this);
    layout->addWidget(loginBtn);

    registerBtn = new QPushButton("Register New User", this);
    layout->addWidget(registerBtn);

    forgotBtn = new QPushButton("Forgot Password?", this);
    layout->addWidget(forgotBtn);

    testConnBtn = new QPushButton("Test Connection", this);
    layout->addWidget(testConnBtn);
    connect(testConnBtn, &QPushButton::clicked, this, &LoginDialog::testConnection);

    connect(forgotBtn, &QPushButton::clicked, [this]() {
        ResetPasswordDialog resetDlg(this);
        resetDlg.exec();
    });

    connect(registerBtn, &QPushButton::clicked, [this]() {
        RegisterDialog regDlg(this);
        regDlg.exec();
    });

    connect(loginBtn, &QPushButton::clicked, [this]() {
        QString id = idEdit->text().trimmed();
        QString pass = passwordEdit->text();

        if (id.isEmpty()) {
            QMessageBox::warning(this, "Login Failed", "Employee ID cannot be empty.");
        } else {
            QSqlQuery query;
            query.prepare("SELECT password, failed_attempts, locked_until FROM users WHERE employee_id = :id");
            query.bindValue(":id", id);
            
            if (query.exec()) {
                if (query.next()) {
                    QString storedHash = query.value(0).toString();
                    int failedAttempts = query.value(1).toInt();
                    QString lockedUntilStr = query.value(2).toString();

                    if (!lockedUntilStr.isEmpty()) {
                        QDateTime lockedUntil = QDateTime::fromString(lockedUntilStr, Qt::ISODate);
                        if (lockedUntil.isValid() && lockedUntil > QDateTime::currentDateTime()) {
                            QMessageBox::warning(this, "Account Locked", "Account is locked due to too many failed attempts.\nTry again later.");
                            return;
                        }
                    }

                    QString hashedInput = QString(QCryptographicHash::hash(pass.toUtf8(), QCryptographicHash::Sha256).toHex());
                    if (storedHash == hashedInput) {
                        // Update Last Login Timestamp and reset failures
                        QSqlQuery updateQuery;
                        updateQuery.prepare("UPDATE users SET last_login = :time, failed_attempts = 0, locked_until = NULL WHERE employee_id = :id");
                        updateQuery.bindValue(":time", QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
                        updateQuery.bindValue(":id", id);
                        updateQuery.exec();

                    QSettings settings;
                    if (rememberMeCheck->isChecked()) {
                        settings.setValue("savedEmployeeId", id);
                        settings.setValue("rememberMe", true);
                    } else {
                        settings.remove("savedEmployeeId");
                        settings.setValue("rememberMe", false);
                    }
                    accept();
                    } else {
                        failedAttempts++;
                        QSqlQuery updateQuery;
                        if (failedAttempts >= 3) {
                            QDateTime lockTime = QDateTime::currentDateTime().addSecs(300); // Lock for 5 minutes
                            updateQuery.prepare("UPDATE users SET failed_attempts = :attempts, locked_until = :lock WHERE employee_id = :id");
                            updateQuery.bindValue(":attempts", failedAttempts);
                            updateQuery.bindValue(":lock", lockTime.toString(Qt::ISODate));
                            updateQuery.bindValue(":id", id);
                            updateQuery.exec();
                            sendLockNotificationEmail(id);
                            QMessageBox::warning(this, "Login Failed", "Incorrect password.\nAccount locked for 5 minutes.");
                        } else {
                            updateQuery.prepare("UPDATE users SET failed_attempts = :attempts WHERE employee_id = :id");
                            updateQuery.bindValue(":attempts", failedAttempts);
                            updateQuery.bindValue(":id", id);
                            updateQuery.exec();
                            QMessageBox::warning(this, "Login Failed", QString("Incorrect password.\nAttempts remaining: %1").arg(3 - failedAttempts));
                        }
                    }
                } else {
                    QMessageBox::warning(this, "Login Failed", "Incorrect ID or password.");
                }
            } else {
                QMessageBox::critical(this, "Database Error", query.lastError().text());
            }
        }
    });

    // Setup Progress Bar for Database Connection
    progressBar = new QProgressBar(this);
    progressBar->setRange(0, 0); // Indeterminate (Spinner effect)
    progressBar->setTextVisible(true);
    progressBar->setFormat("Connecting to Database...");
    progressBar->setVisible(false);
    layout->addWidget(progressBar);

    // Trigger connection after UI is shown
    QTimer::singleShot(100, this, &LoginDialog::connectToDatabase);
}

void LoginDialog::connectToDatabase() {
    // Disable inputs while connecting
    idEdit->setEnabled(false);
    passwordEdit->setEnabled(false);
    loginBtn->setEnabled(false);
    registerBtn->setEnabled(false);
    forgotBtn->setEnabled(false);
    testConnBtn->setEnabled(false);
    progressBar->setVisible(true);
    
    // Force UI update to show the spinner
    QCoreApplication::processEvents();

    // Initialize Database
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isValid()) {
        db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("employees.db");

        if (!db.open()) {
            QMessageBox::critical(this, "Database Error", "Could not open database: " + db.lastError().text());
        } else {
            QSqlQuery query;
            query.exec("CREATE TABLE IF NOT EXISTS users (employee_id TEXT PRIMARY KEY, password TEXT, last_login TEXT, failed_attempts INTEGER DEFAULT 0, locked_until TEXT)");
            query.exec("ALTER TABLE users ADD COLUMN last_login TEXT");
            query.exec("ALTER TABLE users ADD COLUMN failed_attempts INTEGER DEFAULT 0");
            query.exec("ALTER TABLE users ADD COLUMN locked_until TEXT");
            query.exec("CREATE TABLE IF NOT EXISTS reports (id INTEGER PRIMARY KEY AUTOINCREMENT, employee_id TEXT, name TEXT, report_date TEXT, total_hours REAL, gross_wage REAL, tax REAL, net_wage REAL)");

            // Insert default user for testing
            QString defaultPassHash = QString(QCryptographicHash::hash(QByteArray("admin123"), QCryptographicHash::Sha256).toHex());
            query.prepare("INSERT OR IGNORE INTO users (employee_id, password) VALUES ('EMP001', :pass)");
            query.bindValue(":pass", defaultPassHash);
            query.exec();
        }
    }

    progressBar->setVisible(false);
    idEdit->setEnabled(true);
    passwordEdit->setEnabled(true);
    loginBtn->setEnabled(true);
    registerBtn->setEnabled(true);
    forgotBtn->setEnabled(true);
    testConnBtn->setEnabled(true);
}

void LoginDialog::testConnection() {
    QSqlDatabase db = QSqlDatabase::database();
    if (db.isOpen()) {
        QMessageBox::information(this, "Test Connection", "Database is already connected.");
        return;
    }

    // If not open, try to open existing definition
    if (db.open()) {
        QMessageBox::information(this, "Test Connection", "Database connected successfully.");
    } else {
        QMessageBox::critical(this, "Test Connection", "Could not connect to database: " + db.lastError().text());
    }
}

QString LoginDialog::getEmployeeID() const {
    return idEdit->text().trimmed();
}

void LoginDialog::sendLockNotificationEmail(const QString &userId) {
    QString emailAddress = "admin@example.com"; // Replace with the admin's actual email
    QString subject = "Account Locked: " + userId;
    QString body = "The account " + userId + " has been locked due to too many failed login attempts.";

    QStringList args;
    args << "--to" << emailAddress;
    args << "--subject" << subject;
    args << "--body" << body;

    if (!QProcess::startDetached("xdg-email", args)) {
        qDebug() << "Failed to start xdg-email. Please ensure it is installed.";
    }
}
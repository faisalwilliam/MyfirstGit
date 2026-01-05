#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QCheckBox>
#include <QProgressBar>
#include <QPushButton>

class LoginDialog : public QDialog {
    Q_OBJECT
public:
    explicit LoginDialog(QWidget *parent = nullptr);
    QString getEmployeeID() const;

private slots:
    void connectToDatabase();
    void testConnection();

private:
    QLineEdit *idEdit;
    QLineEdit *passwordEdit;
    QCheckBox *rememberMeCheck;
    QProgressBar *progressBar;
    QPushButton *loginBtn;
    QPushButton *registerBtn;
    QPushButton *forgotBtn;
    QPushButton *testConnBtn;
    void sendLockNotificationEmail(const QString &userId);
};

#endif // LOGINDIALOG_H
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

private:
    QLineEdit *idEdit;
    QLineEdit *passwordEdit;
    QCheckBox *rememberMeCheck;
    QProgressBar *progressBar;
    QPushButton *loginBtn;
    QPushButton *registerBtn;
    QPushButton *forgotBtn;
    void sendLockNotificationEmail(const QString &userId);
};

#endif // LOGINDIALOG_H
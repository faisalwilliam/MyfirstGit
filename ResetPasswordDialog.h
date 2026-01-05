#ifndef RESETPASSWORDDIALOG_H
#define RESETPASSWORDDIALOG_H

#include <QDialog>
#include <QLineEdit>

class ResetPasswordDialog : public QDialog {
    Q_OBJECT
public:
    explicit ResetPasswordDialog(QWidget *parent = nullptr);

private:
    QLineEdit *idEdit;
    QLineEdit *recoveryKeyEdit;
    QLineEdit *newPasswordEdit;
    QLineEdit *confirmPasswordEdit;
    void resetPassword();
    bool validatePassword(const QString &password);
};

#endif // RESETPASSWORDDIALOG_H
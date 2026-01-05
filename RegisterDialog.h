#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include <QDialog>
#include <QLineEdit>

class RegisterDialog : public QDialog {
    Q_OBJECT
public:
    explicit RegisterDialog(QWidget *parent = nullptr);

private:
    QLineEdit *idEdit;
    QLineEdit *passwordEdit;
    QLineEdit *confirmPasswordEdit;
    void registerUser();
    bool validatePassword(const QString &password);
};

#endif // REGISTERDIALOG_H
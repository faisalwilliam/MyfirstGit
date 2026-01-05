#ifndef PROFILEDIALOG_H
#define PROFILEDIALOG_H

#include <QDialog>
#include <QLineEdit>

class ProfileDialog : public QDialog {
    Q_OBJECT
public:
    explicit ProfileDialog(const QString &employeeId, QWidget *parent = nullptr);

private:
    QString currentEmployeeId;
    QLineEdit *newIdEdit;
    QLineEdit *currentPasswordEdit;
    QLineEdit *newPasswordEdit;
    QLineEdit *confirmPasswordEdit;
    void changeUsername();
    void changePassword();
    void deleteAccount();
    bool validatePassword(const QString &password);
};

#endif // PROFILEDIALOG_H
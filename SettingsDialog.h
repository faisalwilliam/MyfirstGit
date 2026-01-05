#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QComboBox>

class SettingsDialog : public QDialog {
    Q_OBJECT
public:
    SettingsDialog(const QString &currentCurrency, double currentTaxRate, int themeMode, QWidget *parent = nullptr);
    QString getCurrencySymbol() const;
    double getDefaultTaxRate() const;
    int getThemeMode() const;
    
public slots:
    void accept() override;

private:
    QLineEdit *currencyEdit;
    QDoubleSpinBox *taxRateSpin;
    QComboBox *themeCombo;
};

#endif
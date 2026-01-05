#include "SettingsDialog.h"
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QGroupBox>
#include <QSettings>

SettingsDialog::SettingsDialog(const QString &currentCurrency, double currentTaxRate, int themeMode, QWidget *parent) 
    : QDialog(parent) 
{
    setWindowTitle("Settings");
    
    QFormLayout *layout = new QFormLayout(this);
    
    currencyEdit = new QLineEdit(currentCurrency);
    layout->addRow("Currency Symbol:", currencyEdit);
    
    taxRateSpin = new QDoubleSpinBox();
    taxRateSpin->setRange(0, 100);
    taxRateSpin->setValue(currentTaxRate);
    taxRateSpin->setSuffix("%");
    layout->addRow("Default Tax Rate:", taxRateSpin);
    
    themeCombo = new QComboBox();
    themeCombo->addItem("System Theme");
    themeCombo->addItem("Light Theme");
    themeCombo->addItem("Dark Theme");
    themeCombo->setCurrentIndex(themeMode);
    layout->addRow("Theme:", themeCombo);
    
    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Reset);
    
    QPushButton *okBtn = buttons->button(QDialogButtonBox::Ok);
    connect(currencyEdit, &QLineEdit::textChanged, [okBtn](const QString &text){
        okBtn->setEnabled(!text.trimmed().isEmpty());
    });
    okBtn->setEnabled(!currencyEdit->text().trimmed().isEmpty());

    QPushButton *resetBtn = buttons->button(QDialogButtonBox::Reset);
    resetBtn->setText("Reset to Defaults");
    connect(resetBtn, &QPushButton::clicked, [this](){
        currencyEdit->setText("kr ");
        taxRateSpin->setValue(0.0);
        themeCombo->setCurrentIndex(0);
    });

    connect(buttons, &QDialogButtonBox::accepted, this, &SettingsDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addRow(buttons);
}

QString SettingsDialog::getCurrencySymbol() const {
    return currencyEdit->text();
}

double SettingsDialog::getDefaultTaxRate() const {
    return taxRateSpin->value();
}

int SettingsDialog::getThemeMode() const {
    return themeCombo->currentIndex();
}

void SettingsDialog::accept() {
    QDialog::accept();
}
#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QSplashScreen>
#include <QTimer>
#include <QProgressBar>
#include <QLabel>
#include "MainWindow.h"
#include "LoginDialog.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationName("AmirSoft");
    a.setApplicationName("EmployeeManagementSystem");

    // Setup and show Splash Screen
    QPixmap pixmap(":/splash.png");
    QSplashScreen splash(pixmap);

    QProgressBar progressBar(&splash);
    progressBar.setGeometry(10, pixmap.height() - 30, pixmap.width() - 20, 20);
    progressBar.setStyleSheet("QProgressBar { border: 1px solid grey; border-radius: 5px; text-align: center; background: white; color: black; } QProgressBar::chunk { background-color: #3498db; }");
    progressBar.show();

    QLabel versionLabel(&splash);
    versionLabel.setText("v1.0");
    versionLabel.setStyleSheet("color: white; font-weight: bold; font-size: 16px; background-color: rgba(0, 0, 0, 100); padding: 4px; border-radius: 4px;");
    versionLabel.adjustSize();
    versionLabel.move(pixmap.width() - versionLabel.width() - 10, 10); // Position at top-right
    versionLabel.show();

    splash.show();

    QTimer timer;
    int progress = 0;
    QObject::connect(&timer, &QTimer::timeout, [&]() {
        progress++;
        progressBar.setValue(progress);
        if (progress >= 100) {
            timer.stop();
            
            LoginDialog login;
            if (login.exec() == QDialog::Accepted) {
                MainWindow *w = new MainWindow(login.getEmployeeID());
                w->setAttribute(Qt::WA_DeleteOnClose);
                w->show();
                splash.finish(w);
            } else {
                splash.close();
            }
        }
    });
    timer.start(20);

    return a.exec();
}
#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QScreen>


#define _(String) gettext(String)

extern "C" {
    #include <stdlib.h>
    #include <locale.h>
    #include <libintl.h>

    extern bool pam_auth(const char* user, const char* passwd);
}

class LockScreen : public QMainWindow {
public:
    QScreen *screen;
    QLabel *label;
    QLineEdit* passwordLineEdit;
    int fail = 0;
    LockScreen(){
        this->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
        this->setAttribute(Qt::WA_TranslucentBackground);

        screen = QGuiApplication::primaryScreen();
        this->setFixedSize(screen->size());

        float scale = screen->size().height() / 1080;
        if(scale < 1.0){
            scale = 1.0;
        }

        QWidget* area = new QWidget(this);
        area->setStyleSheet("background-color: black");
        area->setFixedSize(screen->size());

        QWidget* dialog = new QWidget(area);
        dialog->setStyleSheet("background-color: #ff313131;");
        dialog->setFixedSize(400*scale, 200*scale); // Fixed size for the dialog
        dialog->move((screen->size().width() - dialog->size().width()) / 2,
                 (screen->size().height() - dialog->size().height()) / 2);

        QVBoxLayout *mainLayout = new QVBoxLayout;
        dialog->setLayout(mainLayout);

        label = new QLabel("");
        label->setAlignment(Qt::AlignCenter);
        label->setStyleSheet(
            "color: red;"
            "font-size: "+QString::number(18*scale)+"px;"
        );
        mainLayout->addWidget(label);

        passwordLineEdit = new QLineEdit();
        passwordLineEdit->setEchoMode(QLineEdit::Password);
        passwordLineEdit->setPlaceholderText(_("Enter Password"));
        passwordLineEdit->setStyleSheet(
            "background-color: #232323;"
            "color: white;"
            "font-size: "+QString::number(18*scale)+"px;"
            "padding: "+QString::number(10*scale)+"px;"
        );
        connect(passwordLineEdit, &QLineEdit::returnPressed, this, &LockScreen::auth);
        if (strlen(OSK) > 0){
            passwordLineEdit->installEventFilter(this);
        }
        mainLayout->addWidget(passwordLineEdit);

        QPushButton *okButton = new QPushButton(_("Unlock"));
        okButton->setStyleSheet(
            "background-color: #4CAF50;"
            "color: black;"
            "border: none;"
            "font-size: "+QString::number(18*scale)+"px;"
            "border-radius: "+QString::number(5*scale)+"px;"
            "padding: "+QString::number(10*scale)+"px;"
        );
        mainLayout->addWidget(okButton);

        connect(okButton, &QPushButton::clicked, this, &LockScreen::auth);
    }

    bool eventFilter(QObject* object, QEvent* event){
        if(object == passwordLineEdit){
            if (event->type() == QEvent::TouchBegin ||
                event->type() == QEvent::TabletPress ||
                event->type() == QEvent::MouseButtonPress){
                (void)system(QString(QString(OSK)+" &").toStdString().c_str());
            }
        }
        return false;
    }

    void auth(){
        const char* username = getenv("USER");
        if(username == nullptr){
            return;
        }
        const char* password = passwordLineEdit->text().toStdString().c_str();
        if(pam_auth(username, password)){
            QCoreApplication::exit(0);
        } else  {
            label->setText(QString(_("Authentication Failed"))+" ("+QString::number(fail)+")");
            passwordLineEdit->setText("");
        }
    }
};

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    const char *systemLanguage = getenv("LANG");
    if (systemLanguage != nullptr) {
        bindtextdomain("qtlock", "/usr/share/locale");
        setlocale(LC_ALL, systemLanguage);
        textdomain("qtlock");
    } else {
        fprintf(stderr, "WARNING: LANG environment variable not set.\n");
    }
    LockScreen *lock = new LockScreen();
    lock->show();
    return a.exec();
}

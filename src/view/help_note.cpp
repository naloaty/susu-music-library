#include "help_note.h"
#include "ui_help_note.h"

HelpNote::HelpNote(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HelpNote)
{
    ui->setupUi(this);

    QFile file;
    QString data;

    file.setFileName(":/help/UserManual.html");

    if(!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Cannot open user manual.";
    } else {
        data = file.readAll();
    }

    file.close();

    ui->textBrowser->setOpenExternalLinks(true);
    ui->textBrowser->setHtml(data);
}

HelpNote::~HelpNote()
{
    delete ui;
}

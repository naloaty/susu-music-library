#ifndef HELP_NOTE_H
#define HELP_NOTE_H

#include <QDialog>
#include <QFile>

#undef TAG
#define TAG "[HelpNote]"

namespace Ui {
class HelpNote;
}

class HelpNote : public QDialog
{
    Q_OBJECT

public:
    explicit HelpNote(QWidget *parent = nullptr);
    ~HelpNote();

private:
    Ui::HelpNote *ui;
};

#endif // HELP_NOTE_H

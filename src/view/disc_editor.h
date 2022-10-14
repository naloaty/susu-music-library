#ifndef DISC_EDITOR_H
#define DISC_EDITOR_H

#include <QDialog>
#include <QItemSelection>
#include <QMessageBox>
#include <QDataWidgetMapper>

#include "src/database.h"
#include "src/model/simplecolumn.h"
#include "src/model/lookupcolumn.h"
#include "src/model/discsongcheckcolumn.h"

#undef TAG
#define TAG "[DiscEditor]"

namespace Ui {
class DiscEditor;
}

class DiscEditor : public QDialog
{
    Q_OBJECT

public:
    explicit DiscEditor(Database *db, QWidget *parent = nullptr);
    ~DiscEditor();

public slots:
    void updateDiscs();
    void updateSongs();
    void updateAuthors();
    void updateDiscSongs();
    void handleDiscSelection(const QItemSelection &selection);

private slots:
    void on_btn_newDisc_clicked();
    void on_btn_deleteDisc_clicked();
    void on_btn_applyDiscChanges_clicked();

private:
    Ui::DiscEditor *ui;

    Database *m_database;
    Filter m_filter = { -1, -1, -1, true };

    VariantMapTableModel *m_discs;
    VariantMapTableModel *m_discSongs;
    VariantMapTableModel *m_songs;
    VariantMapTableModel *m_authors;
    DiscSongCheckColumn *m_discSongColumn;
};

#endif // DISC_EDITOR_H

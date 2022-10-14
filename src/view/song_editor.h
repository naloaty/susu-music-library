#ifndef SONG_EDITOR_H
#define SONG_EDITOR_H

#include <QDialog>
#include <QCompleter>
#include <QItemSelection>
#include <QMessageBox>

#include "src/database.h"
#include "src/model/simplecolumn.h"
#include "src/model/lookupcolumn.h"

#undef TAG
#define TAG "[SongEditor]"

namespace Ui {
class SongEditor;
}

class SongEditor : public QDialog
{
    Q_OBJECT

public:
    explicit SongEditor(Database *db, QWidget *parent = nullptr);
    ~SongEditor();

public slots:
    void updateSongs();
    void updateAuthors();
    void handleSongSelection(const QItemSelection &selection);

private slots:
    void on_btn_deleteSong_clicked();
    void on_btn_newSong_clicked();
    void on_btn_applySongChanges_clicked();

private:
    Ui::SongEditor *ui;

    Database *m_database;
    QCompleter *m_authorCompleter;

    VariantMapTableModel *m_songs;
    VariantMapTableModel *m_authors;
};

#endif // SONG_EDITOR_H

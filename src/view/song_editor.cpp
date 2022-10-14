#include "song_editor.h"
#include "ui_song_editor.h"

SongEditor::SongEditor(Database *db, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SongEditor)
{
    ui->setupUi(this);
    m_database = db;

    m_authors = new VariantMapTableModel(parent);
    m_authors->registerColumn(new SimpleColumn("authorId"));
    m_authors->registerColumn(new SimpleColumn("displayName"));
    m_authors->setIdColumn("authorId");

    m_songs = new VariantMapTableModel(parent);
    m_songs->registerColumn(new SimpleColumn("songId"));
    m_songs->registerColumn(new SimpleColumn("name", "Song"));
    m_songs->registerColumn(new SimpleColumn("authorId"));
    m_songs->registerColumn(new LookupColumn("author", m_authors, "authorId", "displayName", "Author"));
    m_songs->setIdColumn("songId");

    ui->tbl_songs->setModel(m_songs);
    ui->tbl_songs->hideColumn(0);
    ui->tbl_songs->hideColumn(2);

    m_authorCompleter = new QCompleter(this);
    m_authorCompleter->setModel(m_authors);
    m_authorCompleter->setCompletionColumn(1);
    m_authorCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    m_authorCompleter->setFilterMode(Qt::MatchContains);

    ui->cb_songAuthor->setModel(m_authors);
    ui->cb_songAuthor->setModelColumn(1);
    ui->cb_songAuthor->setCompleter(m_authorCompleter);

    QHeaderView *songRow = ui->tbl_songs->verticalHeader();
    songRow->setSectionResizeMode(QHeaderView::Fixed);
    songRow->setMinimumSectionSize(22);
    songRow->setDefaultSectionSize(22);

    QObject::connect(m_database, &Database::songsChanged, this, &SongEditor::updateSongs);
    QObject::connect(m_database, &Database::authorsChanged, this, &SongEditor::updateAuthors);
    QObject::connect(ui->tbl_songs->selectionModel(), &QItemSelectionModel::selectionChanged,
        this, &SongEditor::handleSongSelection);
}

SongEditor::~SongEditor()
{
    delete ui;
}

void SongEditor::updateSongs()
{
    m_songs->replaceRows(m_database->selectSongs());
    qDebug() << TAG << "Updated songs.";
}

void SongEditor::updateAuthors()
{
    int currentRow = ui->cb_songAuthor->currentIndex();
    int currentId = m_authors->getIdByRow(currentRow);

    m_authors->replaceRows(m_database->selectAuthors());

    int authorRow = m_authors->getRowById(currentId);
    ui->cb_songAuthor->setCurrentIndex(authorRow);

    qDebug() << TAG << "Updated authors.";
}

void SongEditor::handleSongSelection(const QItemSelection &selection)
{
    if (selection.indexes().isEmpty()) {
        ui->edt_songName->clear();
        ui->cb_songAuthor->setCurrentIndex(-1);
        ui->edt_songName->setEnabled(false);
        ui->cb_songAuthor->setEnabled(false);
        ui->btn_deleteSong->setEnabled(false);
        ui->btn_applySongChanges->setEnabled(false);
    } else {
        int row = selection.indexes().first().row();
        int nameCol = m_songs->getColByKey("name");
        int authorCol = m_songs->getColByKey("authorId");

        QModelIndex nameIndex = m_songs->index(row, nameCol);
        QModelIndex authorIndex = m_songs->index(row, authorCol);

        int authorId = m_songs->data(authorIndex, Qt::DisplayRole).toInt();
        QString songName = m_songs->data(nameIndex, Qt::DisplayRole).toString();

        ui->edt_songName->setText(songName);
        ui->cb_songAuthor->setCurrentIndex(m_authors->getRowById(authorId));

        ui->edt_songName->setEnabled(true);
        ui->cb_songAuthor->setEnabled(true);
        ui->btn_deleteSong->setEnabled(true);
        ui->btn_applySongChanges->setEnabled(true);
    }
}

void SongEditor::on_btn_deleteSong_clicked()
{
    QItemSelection selection = ui->tbl_songs->selectionModel()->selection();

    if (selection.isEmpty())
        return;

    int row = selection.indexes().first().row();
    int songId = m_songs->getIdByRow(row);
    m_database->removeSong(songId);

    if (row < m_songs->rowCount(QModelIndex()))
        ui->tbl_songs->selectRow(row);
    else if (row > 0)
        ui->tbl_songs->selectRow(row-1);
    else
        handleSongSelection(QItemSelection());

    ui->tbl_songs->setFocus();
}


void SongEditor::on_btn_newSong_clicked()
{
    if (m_authors->rowCount(QModelIndex()) == 0) {
        QMessageBox msg;
        msg.setIcon(QMessageBox::Warning);
        msg.setText(tr("You must add at least 1 author before adding a new song."));
        msg.exec();
        return;
    }

    int songId = m_database->insertSong(tr("New Song"), m_authors->getIdByRow(0));
    int row = m_songs->getRowById(songId);
    ui->tbl_songs->selectRow(row);
    ui->tbl_songs->setFocus();
    ui->edt_songName->setText("");
    ui->cb_songAuthor->setCurrentIndex(-1);
}


void SongEditor::on_btn_applySongChanges_clicked()
{
    QItemSelection selection = ui->tbl_songs->selectionModel()->selection();

    if (selection.isEmpty())
        return;

    QString songName = ui->edt_songName->text();
    QString authorName = ui->cb_songAuthor->currentText();

    if (songName.trimmed().isEmpty()) {
        QMessageBox msg;
        msg.setIcon(QMessageBox::Warning);
        msg.setText(tr("Song name cannot contain whitespaces or be empty."));
        msg.exec();
        ui->edt_songName->setFocus();
        return;
    }

    if (authorName.isEmpty() || ui->cb_songAuthor->findText(authorName) < 0) {
        QMessageBox msg;
        msg.setIcon(QMessageBox::Warning);
        msg.setText(tr("Please select author from the list."));
        msg.exec();
        ui->cb_songAuthor->setFocus();
        return;
    }

    int songRow = selection.indexes().first().row();
    int authorRow = ui->cb_songAuthor->currentIndex();
    int songId = m_songs->getIdByRow(songRow);
    int authorId = m_authors->getIdByRow(authorRow);

    Song song(songId, authorId, songName);
    m_database->updateSong(song);
    ui->tbl_songs->selectRow(songRow);
    ui->tbl_songs->setFocus();
}


#include "disc_editor.h"
#include "ui_disc_editor.h"

DiscEditor::DiscEditor(Database *db, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DiscEditor)
{
    ui->setupUi(this);
    m_database = db;

    m_discs = new VariantMapTableModel(this);
    m_discs->registerColumn(new SimpleColumn("discId"));
    m_discs->registerColumn(new SimpleColumn("name"));
    m_discs->setIdColumn("discId");

    m_discSongs = new VariantMapTableModel(this);
    m_discSongs->registerColumn(new SimpleColumn("discId"));
    m_discSongs->registerColumn(new SimpleColumn("songId"));
    m_discSongs->setIdColumn("songId");

    m_authors = new VariantMapTableModel(this);
    m_authors->registerColumn(new SimpleColumn("authorId"));
    m_authors->registerColumn(new SimpleColumn("displayName"));
    m_authors->setIdColumn("authorId");

    m_discSongColumn = new DiscSongCheckColumn("onDisc", "*");
    m_discSongColumn->setDiscIdColumnKey("discId");
    m_discSongColumn->setSongIdColumnKey("songId");
    m_discSongColumn->setDiscSongsModel(m_discSongs);
    m_discSongColumn->updateFilter(-1);
    m_discSongColumn->setDatabase(m_database);

    m_songs = new VariantMapTableModel(this);
    m_songs->registerColumn(m_discSongColumn);
    m_songs->registerColumn(new SimpleColumn("songId"));
    m_songs->registerColumn(new SimpleColumn("name", "Song"));
    m_songs->registerColumn(new SimpleColumn("authorId"));
    m_songs->registerColumn(new LookupColumn("author", m_authors, "authorId", "displayName", "Author"));
    m_songs->setIdColumn("songId");

    ui->tbl_discs->setModel(m_discs);
    ui->tbl_discs->hideColumn(0);
    ui->tbl_discs->hideColumn(2);

    ui->tbl_discSongs->setModel(m_songs);
    ui->tbl_discSongs->hideColumn(1);
    ui->tbl_discSongs->hideColumn(3);
    ui->tbl_discSongs->setColumnWidth(0, 5);

    QHeaderView *discRow = ui->tbl_discs->verticalHeader();
    discRow->setSectionResizeMode(QHeaderView::Fixed);
    discRow->setMinimumSectionSize(22);
    discRow->setDefaultSectionSize(22);

    QHeaderView *discSongRow = ui->tbl_discSongs->verticalHeader();
    discSongRow->setSectionResizeMode(QHeaderView::Fixed);
    discSongRow->setMinimumSectionSize(22);
    discSongRow->setDefaultSectionSize(22);

    QObject::connect(m_database, &Database::discsChanged, this, &DiscEditor::updateDiscs);
    QObject::connect(m_database, &Database::songsChanged, this, &DiscEditor::updateSongs);
    QObject::connect(m_database, &Database::authorsChanged, this, &DiscEditor::updateAuthors);
    QObject::connect(m_database, &Database::discSongsChanged, this, &DiscEditor::updateDiscSongs);
    QObject::connect(ui->tbl_discs->selectionModel(), &QItemSelectionModel::selectionChanged,
        this, &DiscEditor::handleDiscSelection);
}

DiscEditor::~DiscEditor()
{
    delete ui;
}

void DiscEditor::updateDiscs()
{
    m_discs->replaceRows(m_database->selectDiscs());
    qDebug() << TAG << "Updated discs.";
}

void DiscEditor::updateSongs()
{
    m_songs->replaceRows(m_database->selectSongs());
    qDebug() << TAG << "Updated songs.";
}

void DiscEditor::updateAuthors()
{
    m_authors->replaceRows(m_database->selectAuthors());
    qDebug() << TAG << "Updated authors.";
}

void DiscEditor::updateDiscSongs()
{
    m_discSongs->replaceRows(m_database->select(m_filter));
    qDebug() << TAG << "Updated disc songs.";
}

void DiscEditor::handleDiscSelection(const QItemSelection &selection)
{
    if (selection.indexes().isEmpty()) {
        m_discSongColumn->updateFilter(-1);
        m_songs->update();
        ui->edt_discName->clear();
        ui->edt_discName->setEnabled(false);
        ui->btn_applyDiscChanges->setEnabled(false);
        ui->btn_deleteDisc->setEnabled(false);
        ui->tbl_discSongs->setEnabled(false);
    } else {
        int nameCol = m_discs->getColByKey("name");
        QModelIndex nameIndex = m_discs->index(selection.indexes().first().row(), nameCol);
        QString discName = m_discs->data(nameIndex, Qt::DisplayRole).toString();

        int row = selection.indexes().first().row();
        int discId = m_discs->getIdByRow(row);
        m_discSongColumn->updateFilter(discId);
        m_songs->update();
        ui->edt_discName->setText(discName);
        ui->edt_discName->setEnabled(true);
        ui->btn_applyDiscChanges->setEnabled(true);
        ui->btn_deleteDisc->setEnabled(true);
        ui->tbl_discSongs->setEnabled(true);
    }
}

void DiscEditor::on_btn_newDisc_clicked()
{
    int discId = m_database->insertDisc(tr("New Disc"));
    int row = m_discs->getRowById(discId);
    ui->tbl_discs->selectRow(row);
    ui->tbl_discs->setFocus();
    ui->edt_discName->setText("");
}


void DiscEditor::on_btn_deleteDisc_clicked()
{
    QItemSelection selection = ui->tbl_discs->selectionModel()->selection();

    if (selection.isEmpty())
        return;

    int row = selection.indexes().first().row();
    int discId = m_discs->getIdByRow(row);
    m_database->removeDisc(discId);

    if (row < m_discs->rowCount(QModelIndex()))
        ui->tbl_discs->selectRow(row);
    else if (row > 0)
        ui->tbl_discs->selectRow(row-1);
    else
        handleDiscSelection(QItemSelection());

    ui->tbl_discs->setFocus();
}


void DiscEditor::on_btn_applyDiscChanges_clicked()
{
    QItemSelection selection = ui->tbl_discs->selectionModel()->selection();

    if (selection.isEmpty())
        return;

    QString discName = ui->edt_discName->text();

    if (discName.trimmed().isEmpty()) {
        QMessageBox msg;
        msg.setIcon(QMessageBox::Warning);
        msg.setText(tr("Disc name cannot contain whitespaces or be empty."));
        msg.exec();
        ui->edt_discName->setFocus();
        return;
    }

    int row = selection.indexes().first().row();
    int discId = m_discs->getIdByRow(row);

    Disc disc(discId, discName);
    m_database->updateDisc(disc);
    ui->tbl_discs->selectRow(row);
    ui->tbl_discs->setFocus();
}


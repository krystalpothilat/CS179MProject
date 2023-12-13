#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDir>
#include <QDialog>
#include <QFileDialog>
#include <QFileInfo>
#include <QListWidgetItem>
#include <QMainWindow>
#include "Operation.h"
#include <QString>
#include <QRegularExpression>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    Operation *CurrentOperation;
    vector<Container *> to_be_loaded;
    vector<Container *> to_be_unloaded;
    vector<Move *> to_be_completed_moves;
    vector<Container *> to_be_unloaded_options;
    vector<int> indexVector;
    string moveoutput = "";
    char load_or_balance = ' ';
    string filepath = "";
    string filename = "";
    int time = 0;
    const int MAXCHARLIMIT = 256;
    unsigned long long index = 0;
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void showDialog(const QString &message);
    bool containsNonPrintableCharacters(const QString &text);
    QString removeTxtExtension(const QString &filename);
    void hide_elements();
    void clear_vectors();
    void display_move(unsigned long long i);
    void calculate();
    void get_unload_options();
    void save();

private slots:
    void on_Main_Menu_Load_Unload_clicked();

    void on_Main_Menu_Balance_clicked();

    void on_Upload_Manifest_Confirm_clicked();

    void on_Select_Load_Confirm_clicked();

    void on_Select_Unload_Confirm_clicked();

    void on_X_Moves_X_Minutes_Confirm_clicked();

    void on_Step_X_of_X_Confirm_clicked();

    void on_Download_Manifest_Confirm_clicked();

    void on_Main_Window_Note_clicked();

    void on_Main_Window_Sign_In_clicked();

    void on_NoteInput_returnPressed();

    void on_UserNameInput_returnPressed();

    void on_UploadManifestSelectFile_clicked();

    void on_LoadContainerInput_returnPressed();

    void on_unLoadContainerDisplay_itemClicked(QListWidgetItem *item);

    void on_weightinput_returnPressed();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H

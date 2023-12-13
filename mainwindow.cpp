#include "mainwindow.h"
#include "ui_mainwindow.h"

//TO DO: Implement brand logos and color schemes https://www.dropbox.com/scl/fo/524c89yxb0bwhtulqraw6/h?dl=0&preview=Keoghs+Port-logos_transparent.png&rlkey=k0j63nnq24gdyomiu1v4pc76c
//TO DO: Implement a simple animation for the step x of x screen 

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    CurrentOperation = new Operation();
    ui->setupUi(this);
    hide_elements();
    //note and user name hide
    ui->NoteInput->setVisible(false);
    ui->UserNameInput->setVisible(false);
    ui->stackedWidget->setCurrentIndex(0);
}

MainWindow::~MainWindow()
{
    delete ui;
    CurrentOperation->reset();
    delete CurrentOperation;
    clear_vectors();
}

// Helper Functions
void MainWindow::showDialog(const QString &message) {
    QDialog *dialog = new QDialog();
    dialog->setWindowModality(Qt::WindowModality::ApplicationModal);
    dialog->setMinimumHeight(140);
    dialog->setMinimumWidth(740);

    QLabel *displayInvalidStrings = new QLabel();
    displayInvalidStrings->setParent(dialog);
    QFont font("Segoe UI", 18);
    displayInvalidStrings->setFont(font);
    displayInvalidStrings->setGeometry(10, 10, 720, 100);
    displayInvalidStrings->setText(message);
    displayInvalidStrings->show();

    dialog->show();
}

bool MainWindow::containsNonPrintableCharacters(const QString &text) {
    static QRegularExpression nonPrintableRegex("[^\\x20-\\x7E]");
    return text.contains(nonPrintableRegex);
}

QString MainWindow::removeTxtExtension(const QString &filename)
{
    QFileInfo fileInfo(filename);
    if (fileInfo.suffix().toLower() == "txt") {
        // Remove ".txt" at the end
        return fileInfo.baseName();
    }
    return filename;
}

void MainWindow::hide_elements()
{
    //upload manifest hide
    ui->Upload_Manifest_Confirm->setVisible(false);
    //xmoves hide
    ui->XMovesXminutestextdisplay->setText("Calculating . . .");
    ui->X_Moves_X_Minutes_Confirm->setVisible(false);
    //unload hide
    ui->waitlabel->setVisible(true);
    ui->unLoadContainerDisplay->setVisible(false);
    ui->Select_Unload_Confirm->setVisible(false);
    //download hide
    ui->downloadmanifestdisplay->setText("Please wait while manifest file is being saved . . .");
    ui->Download_Manifest_Confirm->setVisible(false);
    //weight hide
    ui->weightinput->setVisible(false);
    ui->weightprompt->setVisible(false);
}

void MainWindow::clear_vectors()
{
    to_be_loaded.clear();
    to_be_unloaded.clear();
    to_be_unloaded_options.clear();
    to_be_completed_moves.clear();
}

void MainWindow::calculate()
{
    to_be_completed_moves = CurrentOperation->get_moves();
    time = 0;
    for (unsigned long long i = 0; i < to_be_completed_moves.size(); i++) {
        time += to_be_completed_moves.at(i)->get_time();
    }
    string output = "This Operation will take " + to_string(to_be_completed_moves.size()) + " Moves and " + to_string(time) + " Minutes.";
    ui->XMovesXminutestextdisplay->setText(QString::fromStdString(output));
    ui->X_Moves_X_Minutes_Confirm->setVisible(true);
}

void MainWindow::get_unload_options()
{
    to_be_unloaded_options = CurrentOperation->get_containers();
    for (unsigned long long i = 0; i < to_be_unloaded_options.size(); i++) {
        QListWidgetItem *item = new QListWidgetItem();
        item->setText(QString::fromStdString(to_string(i + 1) + " " + to_be_unloaded_options.at(i)->get_description() + ". " + to_string(to_be_unloaded_options.at(i)->get_weight()) + " Kilos."));
        item->setCheckState(Qt::Unchecked);
        ui->unLoadContainerDisplay->addItem(item);
    }
    ui->unLoadContainerDisplay->setVisible(true);
    ui->Select_Unload_Confirm->setVisible(true);
    ui->waitlabel->setVisible(false);
}

void MainWindow::display_move(unsigned long long i)
{
    ui->Step_X_of_X_Confirm->setVisible(false);
    Container *currentContainer = to_be_completed_moves.at(i)->get_container();
    string location = " ";
    string finalLocation = " ";
    if (currentContainer->get_location() == "t") {
        location = "Truck";
    } else if (currentContainer->get_location().at(0) == 'b') {
        location = "Buffer at " + currentContainer->get_location().substr(2, 5);
    } else {
        location = "Ship at " + currentContainer->get_location().substr(2, 5);
    }

    if (to_be_completed_moves.at(i)->get_final_location() == "t") {
        finalLocation = "Truck";
    } else if (to_be_completed_moves.at(i)->get_final_location().at(0) == 'b') {
        finalLocation = "Buffer at " + to_be_completed_moves.at(i)->get_final_location().substr(2, 5);
    } else {
        finalLocation = "Ship at " + to_be_completed_moves.at(i)->get_final_location().substr(2, 5);
    }
    moveoutput = "Step " + to_string(i + 1) + " of " + to_string(to_be_completed_moves.size()) + "\n";
    moveoutput += "Move " + currentContainer->get_description() + " from " + location + " to " + finalLocation + "\n";
    moveoutput += "Time for this move: " + to_string(to_be_completed_moves.at(i)->get_time()) + " Minutes\n";
    moveoutput += "Time remaining: " + to_string(time) + " Minutes\n";
    ui->stepxofxdisplay->setText(QString::fromStdString(moveoutput));
    if (currentContainer->get_weight() == -1) {
        ui->weightinput->setVisible(true);
        ui->weightprompt->setVisible(true);
    } else {
        moveoutput += "Weight: " + to_string(currentContainer->get_weight()) + " Kilograms";
        ui->stepxofxdisplay->setText(QString::fromStdString(moveoutput));
        ui->Step_X_of_X_Confirm->setVisible(true);
    }
}

void MainWindow::save()
{
    filepath = CurrentOperation->get_manifest_path();
    string output = "Manifest: " + filename + "_OUTBOUND has been saved to:\n " + filepath ;
    showDialog("Please send outbound manifest to captain.");
    ui->downloadmanifestdisplay->setText(QString::fromStdString(output));
    ui->Download_Manifest_Confirm->setVisible(true);
}

//Main Menu Slots:
void MainWindow::on_Main_Menu_Load_Unload_clicked()
{
    load_or_balance = 'l';
    CurrentOperation->set_load_or_balance(load_or_balance);
    ui->stackedWidget->setCurrentIndex(6);
}

void MainWindow::on_Main_Menu_Balance_clicked()
{
    load_or_balance = 'b';
    CurrentOperation->set_load_or_balance(load_or_balance);
    ui->stackedWidget->setCurrentIndex(6);
}

//Upload Manifest Slots:
void MainWindow::on_Upload_Manifest_Confirm_clicked()
{
    CurrentOperation->set_manifest_path(filepath); //sends file path to backend now that user has confirmed their choice
    if (load_or_balance == 'l') {
        ui->stackedWidget->setCurrentIndex(2);
        get_unload_options();
    } else {
        ui->stackedWidget->setCurrentIndex(3);
        calculate();
    }
}

void MainWindow::on_UploadManifestSelectFile_clicked()
{
    QString qfilepath = QFileDialog::getOpenFileName(this, "Open Manifest",QDir::homePath(),"Text Files (*.txt)");
    filepath = qfilepath.toStdString();
    if (filepath != "") {
        QFileInfo fileInfo(qfilepath);
        QString qfilename = fileInfo.fileName();
        QString qfilenameWithoutTxt = removeTxtExtension(qfilename);
        filename = qfilenameWithoutTxt.toStdString();
        ui->ManifestDisplay->setText(qfilenameWithoutTxt);
        ui->Upload_Manifest_Confirm->setVisible(true);
    }
}

//Select Unload Slots
void MainWindow::on_Select_Unload_Confirm_clicked()
{
    for (unsigned long long i = 0; i < indexVector.size(); i++) {
        to_be_unloaded.push_back(
            to_be_unloaded_options[static_cast<unsigned long long>(indexVector[i])]);
    }
    CurrentOperation->set_unload(to_be_unloaded);
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_unLoadContainerDisplay_itemClicked(QListWidgetItem *item)
{
    string description = item->text().toStdString();
    int index = int(description.at(0)) - 49;
    if (item->checkState() == Qt::Checked) {
        item->setCheckState(Qt::Unchecked);
        indexVector.erase(remove(indexVector.begin(), indexVector.end(), index), indexVector.end());
    } else {
        item->setCheckState(Qt::Checked);
        indexVector.push_back(index);
    }
}

//Select Load Slots:
void MainWindow::on_LoadContainerInput_returnPressed()
{
    QString qcontainer = ui->LoadContainerInput->text().trimmed();  // Trim whitespace

    // Check for an empty string or non printable string
    if (qcontainer.isEmpty()||containsNonPrintableCharacters(qcontainer)) {
        showDialog("Please enter at least one printable character.");
        ui->LoadContainerInput->setText("");
        return;
    }
    string container = qcontainer.toStdString();
    if(container.length()>MAXCHARLIMIT){
        ui->UserNameInput->setText("");
        string charactersOverLimit = to_string(container.length()-MAXCHARLIMIT);
        string output = "Container Description cannot be greater than " +to_string(MAXCHARLIMIT)+ " Characters.\n You are over the character limit by:\n"+charactersOverLimit+" characters.";
        showDialog(QString::fromStdString(output));
        return;
    }
    QStringList invalidStrings = {"nan", "unused", "empty"};
    for (const QString &forbidden : invalidStrings) {
        if (qcontainer.toLower() == forbidden) {
            // Handle invalid input
            showDialog("Your container description may not solely consist of the words:\n \"NAN\", \"Unused\", or \"Empty\".");
            ui->LoadContainerInput->setText("");
            return;
        }
    }

    // Valid input, add to the list and clear the input
    QListWidgetItem *item = new QListWidgetItem(qcontainer, ui->LoadContainerDisplay);
    ui->LoadContainerDisplay->addItem(item);
    to_be_loaded.push_back(new Container("t", container, -1));
    ui->LoadContainerInput->setText("");
}

void MainWindow::on_Select_Load_Confirm_clicked()
{
    CurrentOperation->set_load(to_be_loaded);
    ui->stackedWidget->setCurrentIndex(3);
    calculate();
}

//X Moves X Minutes Slots:
void MainWindow::on_X_Moves_X_Minutes_Confirm_clicked()
{
    ui->stackedWidget->setCurrentIndex(4);
    display_move(index);
}

//Step X of X Slots:
void MainWindow::on_weightinput_returnPressed()
{
    Container *currentContainer = to_be_completed_moves.at(index)->get_container();
    QString inputText = ui->weightinput->text();

    try {
        // Attempt to convert the input string to an integer
        int weight = inputText.toInt();

        // Check if the integer is within the valid range
        if (weight < 1 || weight > 99999) {
            ui->weightinput->setText("");
            showDialog("Please enter an integer weight within the range:\n 1 - 99999 kilos.");
            return;
        }

        // Set the weight for the current container
        currentContainer->set_weight(weight);

        // Update UI and display information
        moveoutput += "Weight: " + to_string(currentContainer->get_weight()) + " Kilograms";
        ui->stepxofxdisplay->setText(QString::fromStdString(moveoutput));
        ui->Step_X_of_X_Confirm->setVisible(true);
        ui->weightinput->setText("");
        ui->weightinput->setVisible(false);
        ui->weightprompt->setVisible(false);
    } catch (const std::invalid_argument &e) {
        // Handle invalid input (non-integer)
        showDialog("Please enter an integer weight within the range:\n 1 - 99999 kilos.");
        ui->weightinput->setText("");
        return;
    } catch (const std::out_of_range &e) {
        // Handle out-of-range input
        showDialog("Please enter an integer weight within the range:\n 1 - 99999 kilos.");
        ui->weightinput->setText("");
        return;
    }
}

void MainWindow::on_Step_X_of_X_Confirm_clicked()
{
    time -= to_be_completed_moves.at(index)->get_time();
    CurrentOperation->move_complete(index);
    index++;
    if (index == to_be_completed_moves.size()) {
        ui->stackedWidget->setCurrentIndex(5);
        save();
    } else {
        display_move(index);
    }
}

//Download Manifest Slots:
void MainWindow::on_Download_Manifest_Confirm_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    ui->ManifestDisplay->setText("");
    time = 0;
    index = 0;
    filepath = "";
    filename = "";
    load_or_balance = ' ';
    indexVector.clear();
    CurrentOperation->reset();
    hide_elements();
}

//Main Window Slots:
void MainWindow::on_UserNameInput_returnPressed()
{
    QString qname = ui->UserNameInput->text().trimmed();
    if (containsNonPrintableCharacters(qname)||qname.isEmpty()) {
        ui->UserNameInput->setText("");
        showDialog("Please enter at least one printable character.");
        return;
    }
    string name = qname.toStdString();
    if(name.length()>MAXCHARLIMIT){
        ui->UserNameInput->setText("");
        string charactersOverLimit = to_string(name.length()-MAXCHARLIMIT);
        string output = "Name cannot be greater than " +to_string(MAXCHARLIMIT)+ " Characters.\nYou are over the character limit by:\n"+charactersOverLimit+" characters.";
        showDialog(QString::fromStdString(output));
        return;
    }
    ui->UserNameDisplay->setText(qname);
    CurrentOperation->set_username(name);
    ui->UserNameInput->setText("");
    ui->UserNameInput->setVisible(false);
}

void MainWindow::on_NoteInput_returnPressed()
{
    QString qnote = ui->NoteInput->text();
    string note = qnote.toStdString();
    if (containsNonPrintableCharacters(qnote)||qnote.isEmpty()) {
        ui->UserNameInput->setText("");
        showDialog("Please enter at least one printable character.");
        return;
    }
    if(note.length()>MAXCHARLIMIT){
        int numChunks = (note.length() + MAXCHARLIMIT - 1) / MAXCHARLIMIT;

        for (int i = 0; i < numChunks; ++i) {
            int startIdx = i * MAXCHARLIMIT;
            int endIdx = startIdx + MAXCHARLIMIT;
            if (endIdx > note.length()) {
                endIdx = note.length();
            }

            string chunk = note.substr(startIdx, endIdx - startIdx);
            CurrentOperation->set_note(chunk);
        }
        ui->UserNameInput->setText("");
        ui->NoteInput->setVisible(false);
        return;
    }
    CurrentOperation->set_note(note);
    ui->NoteInput->setText("");
    ui->NoteInput->setVisible(false);
}

void MainWindow::on_Main_Window_Note_clicked()
{
    ui->NoteInput->setVisible(true);
}

void MainWindow::on_Main_Window_Sign_In_clicked()
{
    ui->UserNameInput->setVisible(true);
}
